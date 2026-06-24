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

#ifndef MOZART_MODVIRTUALMACHINE_H
#define MOZART_MODVIRTUALMACHINE_H

#include "../mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

using ExecutionMode = VirtualMachine::ExecutionMode;

class ModVirtualMachine: public Module {
public:
  ModVirtualMachine(): Module("VirtualMachine") {}

  class SetVerbose: public Builtin<SetVerbose> {
  public:
    SetVerbose(): Builtin("setVerbose") {}

    static void call(VM vm, In verbose) {
      
    }
  };

  class RunNSchedules: public Builtin<RunNSchedules> {
  public:
    RunNSchedules(): Builtin("runNSchedules") {}

    static void call(VM vm, In nSchedulesNode, In skipSystemThreadsNode) {
      size_t nSchedules = getArgument<size_t>(vm, nSchedulesNode);
      bool skipSystemThreads = getArgument<bool>(vm, skipSystemThreadsNode);

      vm->setExecutionMode(
        skipSystemThreads ? ExecutionMode::LimitedSchedulesWithoutSystemThreads
          : ExecutionMode::LimitedSchedules,
        nSchedules + 1 // Current schedule does not count
      );
    }
  };

  class RunNOperations: public Builtin<RunNOperations> {
  public:
    RunNOperations(): Builtin("runNOperations") {}

    static void call(VM vm, In nOperationsNode, In skipSystemThreadsNode) {
      size_t nOperations = getArgument<size_t>(vm, nOperationsNode);
      bool skipSystemThreads = getArgument<bool>(vm, skipSystemThreadsNode);

      vm->setExecutionMode(
        skipSystemThreads ? ExecutionMode::LimitedOperationsWithoutSystemThreads
          : ExecutionMode::LimitedOperations,
        nOperations // From right now so including caller thread
      );
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODVIRTUALMACHINE_H
