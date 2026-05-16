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

inline void Introspection::signalThreadCreation(Runnable *thread) {
  threads.insert(thread);
}

inline void Introspection::signalThreadDeletion(Runnable *thread) {
  threads.remove(thread);
}

inline void Introspection::signalThreadOperation(Runnable *thread, OpCode op) {

}

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

inline size_t Introspection::getActiveThreadsCount() {
  return getThreadsCount(threads, [](Runnable* thread){
    return thread->isEmulatedThread() && thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline size_t Introspection::getPassiveThreadsCount() {
  return getThreadsCount(threads, [](Runnable* thread){
    return thread->isEmulatedThread() && !thread->isRunnable() && !thread->isDead() && !thread->isTerminated();
  });
}

inline size_t Introspection::getTotalThreadsCount() {
  return getThreadsCount(threads, [](Runnable* thread){
    return thread->isEmulatedThread() && !thread->isDead() && !thread->isTerminated();
  });
}

/* ========== Variables stats ========== */

inline size_t Introspection::getBoundVariablesCount() {
  return 0;
}

inline size_t Introspection::getUnBoundVariablesCount() {
  return 0;
}

inline size_t Introspection::getTotalVariablesCount() {
  return 0;
}

}

#endif // MOZART_INTROSPECTION_H
