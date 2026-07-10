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

#include "mozart.hh"

#include <iostream>

namespace mozart {

////////////////////
// VirtualMachine //
////////////////////

void VirtualMachine::scheduleThread(bool isSystem) {
  //std::cout  << "VM A+" << std::endl;

  // Select a thread
  Runnable* currentThread;
  do {
    //std::cout  << "VM B+" << std::endl;
    currentThread = threadPool.popNext(isSystem);
    //std::cout  << "VM C+" << std::endl;
  } while (currentThread != nullptr && currentThread->isTerminated());

  //std::cout  << "VM D+" << std::endl;
  // When there is no runnable thread left, return to the external world
  if (currentThread == nullptr)
    return; // break

  //std::cout  << "VM E+" << std::endl;
  // Install the thread's space
  if (!currentThread->getSpace()->install()) {
    // The space is failed, kill the thread now
    currentThread->kill();
    return;
  }

  // Run the thread
  assert(currentThread->isRunnable());
  _currentThread = currentThread;

  // //std::cout  << "VM Schedule thread " << currentThread->getId() << " for maximally "
  //   << getMaxOperations() << " operations" << std::endl;
  //std::cout  << "VM F+" << std::endl;
  size_t nOperations = currentThread->run(getMaxOperationsExecutionMode());

  //std::cout  << "VM G+" << std::endl;
  // Update the execution mode (normally manipulated by system threads for specific purposes)
  updateExecutionMode(nOperations);

  //std::cout  << "VM H+" << std::endl;
  _currentThread = nullptr;

  // Schedule the thread anew if it is still runnable
  if (currentThread->isRunnable()) {
    bool isOperationByOperation = testOperationByOperationExecutionMode();

    // In OBO mode non naturally preempted thread must be rescheduled first
    threadPool.schedule(currentThread,
      currentThread->isPreempted() ||
      !isOperationByOperation);

    if (isOperationByOperation)
      currentThread->preempt();
  }

  return;
}

void VirtualMachine::scheduleSystemThreads() {
  threadPool.resetSchedule(tpSystem);
  size_t n = threadPool.getRunnableCount(tpSystem);
  while (n-- > 0)
    scheduleThread(true);

  journal.clear();
}

VirtualMachine::run_return_type VirtualMachine::run() {
  while (!(testAndClearExitRunRequested() ||
      (_envUseDynamicPreemption && environment.testDynamicExitRun()))) {

    //std::cout  << "VM A" << std::endl;

    // Runs Garbage Collector if needed
    // System threads are run in between
    _gcReady = _gcDone = false;
    while (testAndClearGCRequested() || gc.isGCRequired()) {
      _gcReady = true;
      scheduleSystemThreads();

      //std::cout  << "VM B" << std::endl;

      getTopLevelSpace()->install();
      doGC();

      _gcDone = true;
    }

    //std::cout  << "VM C" << std::endl;

    _gcReady = false;
    scheduleSystemThreads();
    _gcDone = false;

    //std::cout  << "VM D" << std::endl;

    // Trigger alarms
    std::int64_t now = getReferenceTime();
    while (!_alarms.empty() && (_alarms.front().expiration <= now)) {
      getTopLevelSpace()->install();

      Wakeable(*_alarms.front().wakeable).wakeUp(this);

      _alarms.remove_front(this);
    }

    //std::cout  << "VM E" << std::endl;

    // Schedules thread
    scheduleThread(false);
    //std::cout  << "VM F" << std::endl;

    if (threadPool.empty())
      break;
  }

  // Before giving control to the external world, restore the top-level space
  getTopLevelSpace()->install();

  // Tell the external world in how much time I would like to be invoked again
  if (!threadPool.empty())
    return run_return_type(recInvokeAgainNow, 0);
  else if (_alarms.empty())
    return run_return_type(recNeverInvokeAgain, 0);
  else
    return run_return_type(recInvokeAgainLater, _alarms.front().expiration);
}

}
