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

#ifndef MOZART_INTROSPECTION_H
#define MOZART_INTROSPECTION_H

#include <iostream>
#include "mozartcore.hh"

namespace mozart {

/* ========== Threads stats ========== */

typedef RunnableList::iterator iterator;
typedef bool (*Condition)(Runnable *thread);

inline size_t getThreadsCount(RunnableList& threads, std::function<bool(Runnable*)> condition) {
  size_t count = 0;

  for (iterator iter = threads.begin(); iter != threads.end(); iter++) {
    Runnable *thread = *iter;
    if (condition(thread))
      count++;
  }

  std::cout << count << std::endl;
  return count;
}

inline size_t Introspection::getActiveThreadsCount(VM vm) {
  return getThreadsCount(vm->aliveThreads, [](Runnable* thread) {
    return thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline size_t Introspection::getPassiveThreadsCount(VM vm) {
  return getThreadsCount(vm->aliveThreads, [](Runnable* thread) {
    return !thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline size_t Introspection::getTotalThreadsCount(VM vm) {
  return getThreadsCount(vm->aliveThreads, [](Runnable* thread) {
    return !thread->isDead() && !thread->isTerminated();
  });
}

/* ========== Variables stats ========== */

inline void forEachThread(RunnableList& threads,
  std::function<void(Runnable*)> lambda) {

  for (iterator iter = threads.begin(); iter != threads.end(); iter++) {
    Runnable *thread = *iter;
    lambda(thread);
  }
}

inline void parseStaticArray(StaticArray<UnstableNode>& array,
  std::function<void(UnstableNode& unstableNode)> lambda) {
  for (size_t i = 0; i < array.size(); i++) {
    lambda(array[i]);
  }
}

inline size_t Introspection::getBoundVariablesCount(VM vm) {
  size_t count = 0;
  forEachThread(vm->aliveThreads, [&count](Runnable* runnable) {
    if (runnable->isDead() || runnable->isTerminated())
      return;
    else if (Thread *thread = dynamic_cast<Thread*>(runnable)) {
      std::cout << "Parse thread " << thread->getID() << std::endl;
      StackEntry& entry = thread->stack.front();
      for (StaticArray<UnstableNode> array : {
        thread->xregs._array, 
        entry.yregs
      }) {
        std::cout << "TEST" << std::endl;
        parseStaticArray(array, [&count](UnstableNode& node) {
          Type type = node.type();
          switch (type.getStructuralBehavior()) {
            case StructuralBehavior::sbValue: {
              std::cout << "Value: " << type.info()->_name << std::endl;
              break;
            } case StructuralBehavior::sbStructural: {
              std::cout << "Structure: " << type.info()->_name << std::endl;
              break;
            } case StructuralBehavior::sbTokenEq: {
              std::cout << "Token: " << type.info()->_name << std::endl;
              break;
            } case StructuralBehavior::sbVariable: {
              std::cout << "Variable: " << type.info()->_name << std::endl;
              break;
            }
          }
        });
      }
    }
  });
  return count;
}

inline size_t Introspection::getUnBoundVariablesCount(VM vm) {
  return 0;
}

inline size_t Introspection::getTotalVariablesCount(VM vm) {
  return 0;
}

}

#endif // MOZART_INTROSPECTION_H
