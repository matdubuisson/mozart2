// Copyright © 2011, Université catholique de Louvain
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MOZART_RUNNABLE_H
#define MOZART_RUNNABLE_H

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

///////////////////////
// IntermediateState //
///////////////////////

IntermediateState::IntermediateState(VM vm) {
  _list.push_front_new(vm, vm, unit);
  _last = _list.begin();
}

IntermediateState::IntermediateState(VM vm, GR gr, IntermediateState& from) {
  assert(from._last == from._list.begin());

  for (auto iter = from._list.begin(); iter != from._list.end(); ++iter) {
    _list.push_back_new(vm);
    gr->copyUnstableNode(_list.back(), *iter);
  }

  _last = _list.begin();
}

void IntermediateState::reset(VM vm, CheckPoint checkPoint) {
  _last = checkPoint;
  _list.remove_after(vm, checkPoint, _list.end());
}

void IntermediateState::reset(VM vm) {
  reset(vm, _list.begin());
}

template <typename... Args>
bool IntermediateState::fetch(VM vm, const char* identity, Args... args) {
  auto iter = _last + 1;
  if (iter == _list.end()) {
    return false;
  } else if (patternmatching::matchesTuple(vm, *iter, identity, args...)) {
    _last = iter;
    return true;
  } else {
    return false;
  }
}

template <typename... Args>
void IntermediateState::store(VM vm, const char* identity, Args&&... args) {
  _list.push_back_new(vm, vm,
                      buildTuple(vm, identity, std::forward<Args>(args)...));
  ++_last;
}

template <typename... Args>
void IntermediateState::resetAndStore(VM vm, CheckPoint checkPoint,
                                      const char* identity, Args&&... args) {
  reset(vm, checkPoint);
  store(vm, identity, std::forward<Args>(args)...);
}

void IntermediateState::rewind(VM vm) {
  _last = _list.begin();
}

//////////////
// Runnable //
//////////////

Runnable::Runnable(VM vm, Space* space, ThreadPriority priority) :
  vm(vm),  _id(_everCreatedThreadsCount++),
  _kindId(0), _generationId(0), _space(space), _priority(priority),
  _runnable(false), _terminated(false),
  _dead(false), _preempted(false), _preemptible(true),
  _raiseOnBlock(false), _intermediateState(vm),
  _replicate(nullptr) {

  _reification.init(vm, ReifiedThread::build(vm, this));

  _space->notifyThreadCreated();

  vm->threads.insert(this);
  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Inserted);
}

Runnable::Runnable(GR gr, Runnable& from) :
  vm(gr->vm),  _id(_everCreatedThreadsCount++),
  _kindId(0), _generationId(0),
  _intermediateState(vm, gr, from._intermediateState),
  _replicate(nullptr) {

  gr->copySpace(_space, from._space);
  _priority = from._priority;
  _runnable = from._runnable;
  _terminated = from._terminated;
  _dead = from._dead;
  _preempted = from._preempted;
  _preemptible = from._preemptible;

  _raiseOnBlock = from._raiseOnBlock;

  _reification.init(vm, ReifiedThread::build(vm, this));

  if (!_dead) {
    vm->threads.insert(this);
  }
}

Runnable::~Runnable() {}

size_t Runnable::run(size_t maxInstructionsNumber) {
  _statistics.runsCount++;
  _statistics.resumesCount++;

  _preempted = false;

  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Updated);

  return doRun(maxInstructionsNumber);
}

void Runnable::setPriority(ThreadPriority priority) {
  if (priority != _priority) {
    _priority = priority;

    if (_runnable && vm->getCurrentThread() != this)
      vm->threadPool.reschedule(this);

    vm->journal.announceRunnable(this,
      VirtualMachineJournal::RunnableAnnounce::Updated);
  }
}

void Runnable::setPreemptible(bool preemptible) {
  _preemptible = preemptible;

  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Updated);
}

void Runnable::resume(bool skipSchedule) {
  assert(!_dead && !_terminated);
  assert(!_runnable);

  _statistics.resumesCount++;

  _preempted = false;
  _runnable = true;
  _space->notifyThreadResumed();

  if (!skipSchedule)
    vm->getThreadPool().schedule(this);

  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Updated);
}

void Runnable::suspend(bool skipUnschedule) {
  assert(!_dead && !_terminated);
  assert(_runnable);

  _statistics.suspendsCount++;

  _preempted = true;
  _runnable = false;
  _space->notifyThreadSuspended();

  if (!skipUnschedule)
    vm->getThreadPool().unschedule(this);

  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Updated);
}

void Runnable::suspendOnVar(VM vm, RichNode variable, bool skipUnschedule) {
  assert(variable.isTransient() && !variable.is<FailedValue>());

  _statistics.suspendsOnVarCount++;

  suspend(skipUnschedule);

  DataflowVariable(variable).addToSuspendList(vm, _reification);
}

void Runnable::preempt() {
  _preempted = true;

  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Updated);
}

void Runnable::kill() {
  assert(!_dead && !_terminated);

  dispose();
}

Runnable* Runnable::gCollectOuter(GC gc) {
  if (_replicate != nullptr) {
    return _replicate;
  } else {
    _replicate = gCollect(gc);
    return _replicate;
  }
}

Runnable* Runnable::sCloneOuter(SC sc) {
  if (_replicate != nullptr) {
    return _replicate;
  } else if (getSpace()->shouldBeCloned()) {
    _replicate = sClone(sc);
    return _replicate;
  } else {
    return this;
  }
}

void Runnable::restoreAfterGR() {
  _replicate = nullptr;
}

void Runnable::terminate() {
  assert(!_dead && !_terminated);
  assert(_runnable);

  _preempted = true;
  _runnable = false;
  _terminated = true;

  _space->notifyThreadTerminated();

  dispose();
}

void Runnable::dispose() {
  _preempted = true;
  _runnable = false;
  _dead = true;

  // vm->threads.remove(this);
  vm->journal.announceRunnable(this,
    VirtualMachineJournal::RunnableAnnounce::Removed);
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_RUNNABLE_H
