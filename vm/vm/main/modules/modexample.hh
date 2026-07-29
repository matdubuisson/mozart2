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

/**
 * @brief How to add a builtin module :
 * 1) Creates the builtin module file : file, basic macros and basic module class
 * 2) Add #include "modules/modexample.hh" in coremodules.hh
 * 3) Add registerBuiltinModExample(vm); in registerCoreModules of coremodules.cc (IDE might complains but it is normal)
 * 4) Fully recompile Mozart with recreation of pregenerated files
 * 5) Add your methods then recompile after each new update
 */

#ifndef MOZART_MODEXAMPLE_H
#define MOZART_MODEXAMPLE_H

#include "../mozartcore.hh"

#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModExample: public Module {
public:
  ModExample(): Module("Example") {}

  class CallBuiltinMethod: public Builtin<CallBuiltinMethod> {
  public:
    CallBuiltinMethod(): Builtin("callBuiltinMethod") {}

    static void call(VM vm) {
      std::cout << "A builtin method has been called" << std::endl;
    }
  };

  class GetValue: public Builtin<GetValue> {
  public:
    GetValue(): Builtin("getValue") {}

    static void call(VM vm, Out result) {
      result = build(vm, 0);
    }
  };

  class GetIncrementedValue: public Builtin<GetIncrementedValue> {
  public:
    GetIncrementedValue(): Builtin("getIncrementedValue") {}

    static void call(VM vm, In originalValue, Out result) {
      nativeint value = getArgument<nativeint>(vm, originalValue);
      result = build(vm, value + 1);
    }
  };

  class GetCustomList: public Builtin<GetCustomList> {
  public:
    GetCustomList(): Builtin("getCustomList") {}

    static void call(VM vm, In size, In value, Out result) {
      size_t s = getArgument<size_t>(vm, size);
      size_t v = getArgument<size_t>(vm, value);

      OzListBuilder builder(vm);

      for (size_t i = 0; i < s; i++) {
        UnstableNode ozInteger = build(vm, v + i);
        builder.push_back(vm, ozInteger);
      }

      result = builder.get(vm);
    }
  };

  class IncrementAll: public Builtin<IncrementAll> {
  public:
    IncrementAll(): Builtin("incrementAll") {}

    static void call(VM vm, In originalList, Out result) {
      OzListBuilder builder(vm);
      ozListForEach(vm, originalList, [vm, &builder](nativeint integer) {
        builder.push_back(vm, build(vm, integer + 1));
      }, "List of integers");
      result = builder.get(vm);
    }
  };
};

}

}

#endif

#endif