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

#include "../mozart.hh"
#include "modintrospection.hh"

namespace mozart {

namespace builtins {

void ModIntrospection::GetGarbageCollectedThreads::call(VM vm, Out result) {
  Introspection introspection = vm->getIntrospection();
  
  OzListBuilder builder(vm);
  for (Runnable** runnable : introspection.getGarbageCollectedThreads(vm)) {
    builder.push_back(vm, build(vm, (*runnable)->getId()));
  }
  result = builder.get(vm);
}

static inline
UnstableNode buildIdsList(VM vm, Introspection::IdsVector& vector) {
  OzListBuilder builder(vm);
  for (size_t id : vector) {
    builder.push_back(vm, build(vm, id));
  }
  return builder.get(vm);
}

void ModIntrospection::GetGarbageCollectorTodos::call(VM vm, Out result) {
  Introspection introspection = vm->getIntrospection();
  Introspection::GarbageCollectorTodos todos = introspection.getGarbageCollectorTodos(vm);

  result = buildRecord(vm,
    buildArity(vm,
      "todos",
      "runnables",
      "structures",
      "variables"
    ),
    buildIdsList(vm, todos.runnableIds),
    buildIdsList(vm, todos.structureIds),
    buildIdsList(vm, todos.variableIds)
  );
}

}

}
