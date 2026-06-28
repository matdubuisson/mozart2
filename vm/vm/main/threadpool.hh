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

#ifndef MOZART_THREADPOOL_H
#define MOZART_THREADPOOL_H

#include "mozartcore.hh"

namespace mozart {

/////////////////
// ThreadQueue //
/////////////////

void ThreadQueue::gCollect(GC gc) {
  for (auto iterator = begin(); iterator != end(); iterator++) {
    Runnable*& thread = *iterator;
    gc->copyThread(thread, thread);
  }
}

void ThreadQueue::dump() {
  for (auto iterator = begin(); iterator != end(); iterator++) {
    Runnable* runnable = *iterator;
    runnable->dump();
  }
}

////////////////
// ThreadPool //
////////////////

Runnable* ThreadPool::getNext(bool includeSystemThreads) {
  // Avoid infinite loops when counters need to be modified
  int remainingsCopy[tpCount];
  for (int i = 0; i < tpCount; i++)
    remainingsCopy[i] = remainings[i];

  do {
    if (includeSystemThreads
      && !queues[tpSystem].empty() && remainingsCopy[tpSystem] > 0) {
      return getNext(tpSystem);
    }

    // Reset remainings[tpSystem] to the maximum value
    remainingsCopy[tpSystem] = queues[tpSystem].size();

    // While remainings[tpHi] > 0, return the first Hi-priority thread
    if (!queues[tpHi].empty() && remainingsCopy[tpHi] > 0) {
      return getNext(tpHi);
    }

    // Reset remainings[tpHi] for subsequent calls
    remainingsCopy[tpHi] = HiToMiddlePriorityRatio;

    // While remainings[tpMiddle] > 0, return the first Middle-priority thread
    if (!queues[tpMiddle].empty() && remainingsCopy[tpMiddle] > 0) {
      return getNext(tpMiddle);
    }

    // Reset remainings[tpMiddle] for subsequent calls
    remainingsCopy[tpMiddle] = MiddleToLowPriorityRatio;

    // remainings[tpLow] is not used, always return the first Low-priority thread
    if (!queues[tpLow].empty()) {
      return getNext(tpLow);
    }
  } while (!empty(includeSystemThreads)); // might not be empty if all remainings were 0

  return nullptr;
}

Runnable* ThreadPool::getNext(ThreadPriority priority) {
  return queues[priority].front();
}

Runnable* ThreadPool::popNext(bool includeSystemThreads) {
  do {
    // Thread system must execute once before all others
    if (includeSystemThreads
      && !queues[tpSystem].empty() && remainings[tpSystem] > 0) {
      remainings[tpSystem]--;
      return popNext(tpSystem);
    }

    // Reset remainings[tpSystem] to the maximum value
    remainings[tpSystem] = queues[tpSystem].size();

    // While remainings[tpHi] > 0, return the first Hi-priority thread
    if (!queues[tpHi].empty() && remainings[tpHi] > 0) {
      remainings[tpHi]--;
      return popNext(tpHi);
    }

    // Reset remainings[tpHi] for subsequent calls
    remainings[tpHi] = HiToMiddlePriorityRatio;

    // While remainings[tpMiddle] > 0, return the first Middle-priority thread
    if (!queues[tpMiddle].empty() && remainings[tpMiddle] > 0) {
      remainings[tpMiddle]--;
      return popNext(tpMiddle);
    }

    // Reset remainings[tpMiddle] for subsequent calls
    remainings[tpMiddle] = MiddleToLowPriorityRatio;

    // remainings[tpLow] is not used, always return the first Low-priority thread
    if (!queues[tpLow].empty()) {
      return popNext(tpLow);
    }
  } while (!empty(includeSystemThreads)); // might not be empty if all remainings were 0

  return nullptr;
}

Runnable* ThreadPool::popNext(ThreadPriority priority) {
  Runnable* result = queues[priority].front();
  queues[priority].pop_front();
  return result;
}

}

#endif // MOZART_THREADPOOL_H
