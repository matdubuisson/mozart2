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

#ifndef MOZART_MODDEBUGINFOMANAGER_H
#define MOZART_MODDEBUGINFOMANAGER_H

#include "../introspection-decl.hh"
#include "mozartcore-module.hh"

#include <unordered_set>
#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModDebugInfoManager: public Module {
public:
  ModDebugInfoManager(): Module("DebugInfoManager") {}

  class Enable: public Builtin<Enable> {
  public:
    Enable(): Builtin("enable") {}

    static void call(VM vm) {
      vm->getDebugInfoManager().enable();
    }
  };

  class Disable: public Builtin<Disable> {
  public:
    Disable(): Builtin("disable") {}

    static void call(VM vm) {
      vm->getDebugInfoManager().disable();
    }
  };

  class GetVariables: public Builtin<GetVariables> {
  public:
    GetVariables(): Builtin("getVariables") {}

    static void call(VM vm, Out result) {
      result = DebugInfoManager::buildMap(vm,
        vm->getDebugInfoManager().getVariables());
    }
  };

  class GetAbstractions: public Builtin<GetAbstractions> {
  public:
    GetAbstractions(): Builtin("getAbstractions") {}

    static void call(VM vm, Out result) {
      result = DebugInfoManager::buildMap(vm,
        vm->getDebugInfoManager().getAbstractions());
    }
  };

  class ResolveName: public Builtin<ResolveName> {
  public:
    ResolveName(): Builtin("resolveName") {}

    static void call(VM vm, In name, Out result) {
      atom_t atomName = AtomLike(name).getValue(vm);
      
      result = DebugInfoManager::buildVector(vm,
        vm->getDebugInfoManager().resoleName(atomName));
    }
  };

  class ResolveCompleteName: public Builtin<ResolveCompleteName> {
  public:
    ResolveCompleteName(): Builtin("resolveCompleteName") {}

    static void call(VM vm, In name, In filename, In line, In column, Out result) {
      atom_t atomName = AtomLike(name).getValue(vm);
      atom_t atomFilename = AtomLike(filename).getValue(vm);
      size_t intLine = getArgument<size_t>(vm, line);
      size_t intColumn = getArgument<size_t>(vm, column);

      result = DebugInfoManager::buildVector(vm,
        vm->getDebugInfoManager().resoleName(atomName, atomFilename, intLine, intColumn));
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODDEBUGINFOMANAGER_H
