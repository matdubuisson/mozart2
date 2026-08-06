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

#ifndef MOZART_MODIDENTIFIABLE_H
#define MOZART_MODIDENTIFIABLE_H

#include "../mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

/////////////////////////
// Identifiable module //
/////////////////////////

class ModIdentifiable: public Module {
public:
  ModIdentifiable(): Module("Identifiable") {}

  class GetId: public Builtin<GetId> {
  public:
    GetId(): Builtin("getId") {}

    static void call(VM vm, In object, Out result) {
      size_t id = 0;
      
      if (object.is<ReifiedThread>())
        id = object.as<ReifiedThread>().value()->getId();
      else if (object.is<OptVar>())
        id = object.as<OptVar>().getId();
      else if (object.is<Variable>())
        id = object.as<Variable>().getId();
      else if (object.is<ReadOnlyVariable>())
        id = object.as<ReadOnlyVariable>().getId();
      else if (object.is<Cons>())
        id = object.as<Cons>().getId();
      
      result = build(vm, id);
    }
  };

  class SetId: public Builtin<SetId> {
  public:
    SetId(): Builtin("setId") {}

    static void call(VM vm, In object, In idNode) {
      size_t id = getArgument<size_t>(vm, idNode);

      if (object.is<ReifiedThread>()) {
        object.as<ReifiedThread>().value()->setId(id);
        vm->getEventManager().announceRunnable(vm,
          getArgument<Runnable*>(vm, object),
          VirtualMachineEventManager::RunnableAnnounce::Updated
        );
      } else if (object.is<OptVar>())
        object.as<OptVar>().setId(id);
      else if (object.is<Variable>())
        object.as<Variable>().setId(id);
      else if (object.is<ReadOnlyVariable>())
        object.as<ReadOnlyVariable>().setId(id);
      else if (object.is<Cons>())
        object.as<Cons>().setId(id);
    }
  };
};

}

}

#endif

#endif
