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

class ModScheduler: public Module {
public:
  ModScheduler(): Module("Scheduler") {}

  class SetVerbose: public Builtin<SetVerbose> {
  public:
    SetVerbose(): Builtin("setVerbose") {}

    static void call(VM vm, In verbose) {
      
    }
  };

  class RunNSchedules: public Builtin<RunNSchedules> {
  public:
    RunNSchedules(): Builtin("runNSchedules") {}

    static void call(VM vm, In nSchedulesNode) {
      size_t nSchedules = getArgument<size_t>(vm, nSchedulesNode);

      vm->setExecutionMode(
        ExecutionMode::LimitedSchedules,
        nSchedules + 1 // Current schedule does not count
      );
    }
  };

  class RunNOperations: public Builtin<RunNOperations> {
  public:
    RunNOperations(): Builtin("runNOperations") {}

    static void call(VM vm, In nOperationsNode) {
      size_t nOperations = getArgument<size_t>(vm, nOperationsNode);

      vm->setExecutionMode(
        ExecutionMode::LimitedOperations,
        nOperations // From right now so including caller thread
      );
    }
  };
  
  class RunOperationByOperation: public Builtin<RunOperationByOperation> {
  public:
    RunOperationByOperation(): Builtin("runOperationByOperation") {}

    static void call(VM vm) {
      vm->setExecutionMode(ExecutionMode::OperationByOperation);
    }
  };

  class GetExecutionMode: public Builtin<GetExecutionMode> {
  public:
    GetExecutionMode(): Builtin("getExecutionMode") {}

    static void call(VM vm, Out result) {
      std::string executionMode;
      switch (vm->getExecutionMode()) {
        case ExecutionMode::LimitedSchedules: executionMode = "limitedSchedules"; break;
        case ExecutionMode::LimitedOperations: executionMode = "limitedOperations"; break;
        case ExecutionMode::OperationByOperation: executionMode = "operationByOperation"; break;
        default: executionMode = "normal";
      }
      result = build(vm, executionMode.c_str());
    }
  };

  class GetExecutionCounter: public Builtin<GetExecutionCounter> {
  public:
    GetExecutionCounter(): Builtin("getExecutionCounter") {}

    static void call(VM vm, Out result) {
      result = build(vm, vm->getExecutionCounter());
    }
  };

  class Reset: public Builtin<Reset> {
  public:
    Reset(): Builtin("reset") {}

    static void call(VM vm) {
      vm->resetExecutionMode();
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODVIRTUALMACHINE_H
