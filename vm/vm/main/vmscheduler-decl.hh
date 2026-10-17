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

#ifndef MOZART_VMSCHEDULER_DECL_H
#define MOZART_VMSCHEDULER_DECL_H

#include "core-forward-decl.hh"

namespace mozart {

/////////////////////////////
// VirtualMachineScheduler //
/////////////////////////////

class VirtualMachineScheduler {
public:
  enum ExecutionMode {
    Normal = 0,
    LimitedSchedules,
    LimitedOperations,
    OperationByOperation, // Without system threads by definition: they are responsible to advance the VM
    FollowingJournal
  };

public:
  VirtualMachineScheduler() {}

private:
  /**
   * @brief Flow execution control
   * No atomic variables as only the POSIX thread running the scheduler should be able
   * (through a builtin module) to manipulate this state and for performance reasons as
   * these counters will be updated very very often.
   */
  ExecutionMode _executionMode;
  size_t _executionCounter;
  size_t _schedulesCounter;
  size_t _operationsCounter;
  size_t _systemSchedulesCounter;
  size_t _systemOperationsCounter;
};

}

#endif // MOZART_VMSCHEDULER_DECL_H
