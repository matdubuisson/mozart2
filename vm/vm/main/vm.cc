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

int VirtualMachine::scheduleThread() {
  //std::cout << "A+" << std::endl;

  // Select a thread
  Runnable* currentThread;
  do {
    //std::cout << "B+" << std::endl;
    currentThread = threadPool.popNext();
    //std::cout << "C+" << std::endl;
  } while (currentThread != nullptr && currentThread->isTerminated());

  //std::cout << "D+" << std::endl;
  // When there is no runnable thread left, return to the external world
  if (currentThread == nullptr)
    return 1; // break

  //std::cout << "E+" << std::endl;
  // Install the thread's space
  if (!currentThread->getSpace()->install()) {
    // The space is failed, kill the thread now
    currentThread->kill();
    return 0;
  }

  // Run the thread
  assert(currentThread->isRunnable());
  _currentThread = currentThread;

  // //std::cout << "Schedule thread " << currentThread->getId() << " for maximally "
  //   << getMaxOperations() << " operations" << std::endl;
  //std::cout << "F+" << std::endl;
  size_t nOperations = currentThread->run(getMaxOperationsExecutionMode());

  //std::cout << "G+" << std::endl;
  // Update the execution mode (normally manipulated by system threads for specific purposes)
  updateExecutionMode(nOperations);

  //std::cout << "H+" << std::endl;
  _currentThread = nullptr;

  // Schedule the thread anew if it is still runnable
  if (currentThread->isRunnable())
    threadPool.schedule(currentThread,
      currentThread->isPreempted() || // In OBO mode non naturally preempted thread must be rescheduled first
      !testOperationByOperationExecutionMode());

  return 0;
}

int VirtualMachine::scheduleSystemThreads() {
  threadPool.resetSchedule(tpSystem);
  unsigned int n = threadPool.getSchedule(tpSystem);
  while (n > 0) {
    //std::cout << "ALED: " << n << std::endl;
    if (scheduleThread())
      break;
    n--;
  }
  return 0;
}

VirtualMachine::run_return_type VirtualMachine::run() {
  while (!(testAndClearExitRunRequested() ||
      (_envUseDynamicPreemption && environment.testDynamicExitRun()))) {

    //std::cout << "A" << std::endl;

    // Runs Garbage Collector if needed
    // System threads are run in between
    while (testAndClearGCRequested() || gc.isGCRequired()) {
      _isPerformingGC = true;
      scheduleSystemThreads();

      //std::cout << "B" << std::endl;

      getTopLevelSpace()->install();
      doGC();
    }

    //std::cout << "C" << std::endl;

    if (_isPerformingGC) {
      _isPerformingGC = false;
      scheduleSystemThreads();
    }

    //std::cout << "D" << std::endl;

    // Trigger alarms
    std::int64_t now = getReferenceTime();
    while (!_alarms.empty() && (_alarms.front().expiration <= now)) {
      getTopLevelSpace()->install();

      Wakeable(*_alarms.front().wakeable).wakeUp(this);

      _alarms.remove_front(this);
    }

    //std::cout << "E" << std::endl;

    // Schedules thread
    if (scheduleThread())
      break;

    //std::cout << "F" << std::endl;
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
