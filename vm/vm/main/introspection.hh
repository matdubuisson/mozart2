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

/* ========== VM state ========== */

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

inline
void Introspection::displayNode(VM vm, UnstableNode& node) {
  std::cout << node.type()->getName() << std::endl;

  const std::string& name = node.type()->getName();
  switch (node.type().getStructuralBehavior()) {
    case sbVariable: {
      if (name == "ReadOnlyVariable") {
        ReadOnlyVariable variable = node.access<ReadOnlyVariable>();
      } else if (name == "ReadOnly") {
        ReadOnly variable = node.access<ReadOnly>();
      } else if (name == "FailedValue") {
        FailedValue variable = node.access<FailedValue>();
      } else if (name == "OptVar") {
        OptVar variable = node.access<OptVar>();
        std::cout << "Is opt var needed ? " << variable.isNeeded(vm) << std::endl;
      } else if (name == "Variable") {
        Variable variable = node.access<Variable>();
      }
      break;
    } case sbValue: {

      break;
    } case sbStructural: {

      break;
    } case sbTokenEq: {

      break;
    } default: assert(false);
  }
}

inline
void Introspection::displayThread(VM vm, Runnable* runnable) {
  if (Thread* thread = static_cast<Thread*>(runnable)) {
    std::cout << "Prout" << std::endl;

    StaticArray<UnstableNode>& array = thread->xregs._array;

    for (size_t i = 0; i < array.size(); i++) {
      UnstableNode& node = array[i];
      displayNode(vm, node);
    }
  }
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

typedef Introspection::NodesProperties NodesProperties;

inline
void updateNodesPropertiesFromNode(VM vm, NodesProperties& properties,
  Node* node) {
  switch (node->type()->getStructuralBehavior()) {
    case sbVariable: properties.nVariableNodes++; return;
    case sbValue: properties.nValueNodes++; return;
    case sbStructural: properties.nStructuralNodes++; return;
    case sbTokenEq: properties.nTokenNodes++; return;
    default: assert(false);
  }
}

inline
void updateNodesPropertiesFromStaticArray(VM vm, NodesProperties& properties,
  StaticArray<StableNode> array) {
  size_t size = array.size();
  properties.nStableNodes += size;
  properties.nNodes += size;

  for (StaticArray<StableNode>::iterator iter = array.begin();
    iter != array.end(); iter++) {
    StableNode* node = static_cast<StableNode*>(*iter);
    updateNodesPropertiesFromNode(vm, properties, node);
  }
}

inline
void updateNodesPropertiesFromStaticArray(VM vm, NodesProperties& properties,
  StaticArray<UnstableNode>& array) {
  size_t size = array.size();
  properties.nUnstableNodes += size;
  properties.nNodes += size;

  for (StaticArray<UnstableNode>::iterator iter = array.begin();
    iter != array.end(); iter++) {
    UnstableNode* node = static_cast<UnstableNode*>(*iter);
    updateNodesPropertiesFromNode(vm, properties, node);
  }
}

inline
void Introspection::getNodesProperties(VM vm, Runnable* runnable, NodesProperties& properties) {
  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    StaticArray<UnstableNode>& xregs = thread->xregs._array;
    properties.nXNodes += xregs.size();
    updateNodesPropertiesFromStaticArray(vm, properties, xregs);

    ThreadStack& stack = thread->stack;
    for (ThreadStack::iterator entry = stack.begin();
      entry != stack.end(); entry++) {
      properties.stackDepth++;

      StaticArray<UnstableNode>& yregs = entry->yregs;
      StaticArray<StableNode>& gregs = entry->gregs;
      StaticArray<StableNode>& kregs = entry->kregs;

      properties.nYNodes += yregs.size();
      properties.nGNodes += gregs.size();
      properties.nKNodes += kregs.size();

      updateNodesPropertiesFromStaticArray(vm, properties, yregs);
      updateNodesPropertiesFromStaticArray(vm, properties, gregs);
      updateNodesPropertiesFromStaticArray(vm, properties, kregs);
    }
  }
}

inline
NodesProperties Introspection::getNodesProperties(VM vm, Runnable* runnable) {
  NodesProperties properties;
  getNodesProperties(vm, runnable, properties);
  return properties;
}

inline
NodesProperties Introspection::getNodesProperties(VM vm) {
  NodesProperties properties;
  forEachThread(vm->aliveThreads, [this, vm, &properties](Runnable* runnable) {
    if (!runnable->isDead() && !runnable->isTerminated())
      getNodesProperties(vm, runnable, properties);
  });
  return properties;
}

typedef Introspection::NodeDescription NodeDescription;

// inline
// NodeDescription Introspection::getNodeDescription(VM vm, Runnable* runnable, size_t index) {

// }

// inline
// StaticArray<NodeDescription> getNodeDescriptions(VM vm, Runnable* runnable,
//   size_t from = 0, size_t to = SIZE_MAX) {
  
// }

// StaticArray<NodeDescription> getNodeDescriptions(VM vm,
//     size_t from = 0, size_t to = SIZE_MAX) {
  
// }

/* ========== Variables stats ========== */

inline
size_t Introspection::getBoundVariablesCount(VM vm) {
  size_t count = 0;
  return count;
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
