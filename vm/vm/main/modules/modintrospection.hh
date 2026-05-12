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

#ifndef MOZART_MODINTROSPECTION_H
#define MOZART_MODINTROSPECTION_H

#include "../mozartcore.hh"
#include "../introspection.hh"

#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModIntrospection: public Module {
public:
  ModIntrospection(): Module("Introspection") {}

  class Hello: public Builtin<Hello> {
  public:
    Hello(): Builtin("hello") {}

    static void call(VM vm) {
      std::cout << "Hello from introspection module !!" << std::endl;
    }
  };

  class Value: public Builtin<Value> {
  public:
    Value(): Builtin("value") {}

    static void call(VM vm, In inValue, Out outValue) {
      outValue = Numeric(inValue).add(vm, 1);
    }
  };

  class GetActiveThreadsCount: public Builtin<GetActiveThreadsCount> {
  public:
    GetActiveThreadsCount(): Builtin("getActiveThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getActiveThreadsCount(vm));
    }
  };

  class GetPassiveThreadsCount: public Builtin<GetPassiveThreadsCount> {
  public:
    GetPassiveThreadsCount(): Builtin("getPassiveThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getPassiveThreadsCount(vm));
    }
  };

  class GetTotalThreadsCount: public Builtin<GetTotalThreadsCount> {
  public:
    GetTotalThreadsCount(): Builtin("getTotalThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getTotalThreadsCount(vm));
    }
  };

  class GetBoundVariablesCount: public Builtin<GetBoundVariablesCount> {
  public:
    GetBoundVariablesCount(): Builtin("getBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getBoundVariablesCount(vm));
    }
  };

  class GetUnBoundVariablesCount: public Builtin<GetUnBoundVariablesCount> {
  public:
    GetUnBoundVariablesCount(): Builtin("getUnBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getUnBoundVariablesCount(vm));
    }
  };

  class GetTotalVariablesCount: public Builtin<GetTotalVariablesCount> {
  public:
    GetTotalVariablesCount(): Builtin("getTotalVariablesCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, introspection.getTotalVariablesCount(vm));
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTION_H