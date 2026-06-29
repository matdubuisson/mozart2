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
// THIS SOFTWARE IS PROVIdED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIdENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MOZART_INTROSPECTION_H
#define MOZART_INTROSPECTION_H

#include <iostream>
#include "mozartcore.hh"
#include "emulate.hh"

namespace mozart {

typedef RunnableList::iterator iterator;

inline
std::string Introspection::OperationArgument::toRepr(VM vm, RichNode value) {
  auto& config = vm->getPropertyRegistry().config;
  std::basic_stringstream<char> buffer;
  buffer << repr(vm, value, config.printDepth, config.printWidth);
  return buffer.str();
}

/* ========== VM state ========== */

inline
size_t Introspection::getSchedulesCounter(VM vm) {
  return vm->_schedulesCounter;
}

inline
size_t Introspection::getOperationsCounter(VM vm) {
  return vm->_operationsCounter;
}

inline
Runnable* Introspection::getNextScheduledThread(VM vm, bool includeSystemThreads) {
  return vm->getThreadPool().getNext(includeSystemThreads);
}

/* ========== Threads state ========== */

inline
RunnableList& Introspection::getThreads(VM vm) {
  return vm->aliveThreads;
}

inline
Runnable* Introspection::getThread(VM vm, size_t id) {
  RunnableList& list = vm->aliveThreads;
  for (iterator iter = list.begin(); iter != list.end(); iter++) {
    Runnable *runnable = *iter;
    if (runnable->getId() == id)
      return runnable;
  }
  return nullptr;
}

/* ========== Threads stats ========== */

inline
size_t getThreadsAggregate(RunnableList& threads, std::function<bool(Runnable*)> condition) {
  size_t count = 0;

  for (iterator iter = threads.begin(); iter != threads.end(); iter++) {
    Runnable *thread = *iter;
    if (condition(thread))
      count++;
  }

  return count;
}

inline
size_t Introspection::getActiveThreadsCount(VM vm) {
  return getThreadsAggregate(vm->aliveThreads, [](Runnable* thread) {
    return thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline
size_t Introspection::getPassiveThreadsCount(VM vm) {
  return getThreadsAggregate(vm->aliveThreads, [](Runnable* thread) {
    return !thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline
size_t Introspection::getThreadsCount(VM vm) {
  return getThreadsAggregate(vm->aliveThreads, [](Runnable* thread) {
    return !thread->isDead() && !thread->isTerminated();
  });
}

/* ========== Nodes stats ========== */

inline
Type Introspection::getNodeType(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.type;
}

inline
MemWord Introspection::getNodeValue(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.value;
}

inline
void forEachThread(RunnableList& threads,
  std::function<void(Runnable*)> lambda) {

  for (iterator iter = threads.begin(); iter != threads.end(); iter++) {
    Runnable *thread = *iter;
    lambda(thread);
  }
}

inline
void parseStaticArray(StaticArray<UnstableNode>& array,
  std::function<void(UnstableNode& unstableNode)> lambda) {
  for (size_t i = 0; i < array.size(); i++) {
    lambda(array[i]);
  }
}

typedef Introspection::NodesCounts NodesCounts;

inline
void updateNodesCountsFromNode(VM vm, NodesCounts& properties,
  Node* node) {
  switch (node->type()->getStructuralBehavior()) {
    case sbVariable: properties.variableNodesCount++; return;
    case sbValue: properties.valueNodesCount++; return;
    case sbStructural: properties.structuralNodesCount++; return;
    case sbTokenEq: properties.tokenNodesCount++; return;
    default: assert(false);
  }
}

template<class T>
inline
void updateNodesCountsFromNodes(VM vm, NodesCounts& properties,
  StaticArray<T> array) {
  properties.nodesCount += array.size();

  for (typename StaticArray<T>::iterator iter = array.begin();
    iter != array.end(); iter++) {
    T* node = static_cast<T*>(*iter);
    updateNodesCountsFromNode(vm, properties, node);
  }
}

inline
void updateNodesCountsFromStaticArray(VM vm, NodesCounts& properties,
  StaticArray<StableNode> array) {
  properties.stableNodesCount += array.size();
  updateNodesCountsFromNodes<StableNode>(vm, properties, array);
}

inline
void updateNodesCountsFromStaticArray(VM vm, NodesCounts& properties,
  StaticArray<UnstableNode> array) {
  properties.unstableNodesCount += array.size();
  updateNodesCountsFromNodes<UnstableNode>(vm, properties, array);
}

inline
void Introspection::getNodesCounts(VM vm, Runnable* runnable, NodesCounts& properties) {
  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    StaticArray<UnstableNode>& xregs = thread->xregs._array;
    properties.xNodesCount += xregs.size();
    updateNodesCountsFromStaticArray(vm, properties, xregs);

    ThreadStack& stack = thread->stack;
    for (ThreadStack::iterator entry = stack.begin();
      entry != stack.end(); entry++) {
      properties.stackDepth++;

      StaticArray<UnstableNode>& yregs = entry->yregs;
      StaticArray<StableNode>& gregs = entry->gregs;
      StaticArray<StableNode>& kregs = entry->kregs;

      properties.yNodesCount += yregs.size();
      properties.gNodesCount += gregs.size();
      properties.kNodesCount += kregs.size();

      updateNodesCountsFromStaticArray(vm, properties, yregs);
      updateNodesCountsFromStaticArray(vm, properties, gregs);
      updateNodesCountsFromStaticArray(vm, properties, kregs);
    }
  }
}

inline
NodesCounts Introspection::getNodesCounts(VM vm) {
  NodesCounts properties;
  forEachThread(vm->aliveThreads, [this, vm, &properties](Runnable* runnable) {
    if (!runnable->isDead() && !runnable->isTerminated())
      getNodesCounts(vm, runnable, properties);
  });
  return properties;
}

typedef Introspection::NodesRegister NodesRegister;

inline
size_t Introspection::getNodesRegisterSize(VM vm, Runnable* runnable,
  NodesRegister nodesRegister, size_t depth) {
  Thread* thread = dynamic_cast<Thread*>(runnable);
  if (!thread)
    return 0;

  assert(depth < thread->stack.size());
  StackEntry& entry = thread->stack[depth];
  
  switch (nodesRegister) {
    case xRegister: {
      assert(depth == 0);
      return thread->xregs._array.size();
    } case yRegister: {
      return entry.yregs.size();
    } case gRegister: {
      return entry.gregs.size();
    } case kRegister: {
      return entry.kregs.size();
    } default: assert(false);
  }
}

inline
RichNode Introspection::getNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t index) {
  Thread* thread = dynamic_cast<Thread*>(runnable);
  if (!thread)
    return nullptr;

  assert(depth < thread->stack.size());
  StackEntry& entry = thread->stack[depth];
  
  switch (nodesRegister) {
    case xRegister: {
      assert(depth == 0);
      StaticArray<UnstableNode> xregs = thread->xregs._array;
      assert(index < xregs.size());
      return RichNode(xregs[index]);
    } case yRegister: {
      StaticArray<UnstableNode> yregs = entry.yregs;
      assert(index < yregs.size());
      return RichNode(yregs[index]);
    } case gRegister: {
      StaticArray<StableNode> gregs = entry.gregs;
      assert(index < gregs.size());
      return RichNode(gregs[index]);
    } case kRegister: {
      StaticArray<StableNode> kregs = entry.kregs;
      assert(index < kregs.size());
      return RichNode(kregs[index]);
    } default: assert(false);
  }
}

template<class T>
inline
void doForEachNodeFromStaticArray(VM vm, StaticArray<T> array,
  size_t from, size_t to, std::function<void(RichNode)> lambda) {
  for (size_t i = from; i < to; i++) {
    lambda(array[i]);
  }
}

inline
void Introspection::doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t from, size_t to, std::function<void(RichNode)> lambda) {
  assert(from < to);

  Thread* thread = dynamic_cast<Thread*>(runnable);
  if (!thread)
    return;
  
  assert(depth < thread->stack.size());
  StackEntry& entry = thread->stack[depth];
  
  switch (nodesRegister) {
    case xRegister: {
      assert(depth == 0);
      StaticArray<UnstableNode> xregs = thread->xregs._array;
      assert(to < xregs.size());
      doForEachNodeFromStaticArray(vm, xregs, from, to, lambda);
      return;
    } case yRegister: {
      StaticArray<UnstableNode> yregs = entry.yregs;
      assert(to < yregs.size());
      doForEachNodeFromStaticArray(vm, yregs, from, to, lambda);
      return;
    } case gRegister: {
      StaticArray<StableNode> gregs = entry.gregs;
      assert(to < gregs.size());
      doForEachNodeFromStaticArray(vm, gregs, from, to, lambda);
      return;
    } case kRegister: {
      StaticArray<StableNode> kregs = entry.kregs;
      assert(to < kregs.size());
      doForEachNodeFromStaticArray(vm, kregs, from, to, lambda);
      return;
    } default: assert(false);
  }
}

/* ========== Variables stats ========== */

inline
size_t Introspection::getBoundVariablesCount(VM vm) {
  return 0;
}

inline
size_t Introspection::getUnBoundVariablesCount(VM vm) {
  return 0;
}

inline
size_t Introspection::getVariablesCount(VM vm) {
  return 0;
}

}

#endif // MOZART_INTROSPECTION_H
