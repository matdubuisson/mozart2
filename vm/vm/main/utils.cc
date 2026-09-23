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

void initArrayAt(VM vm, RichNode structure,
  StaticArray<StableNode>& array, size_t index) {
  UnstableNode variable = Variable::build(vm);

  array[index].init(vm, variable);

  AdvancedIdentifiable identifiableStructure = AdvancedIdentifiable(structure);
  if (identifiableStructure.is(vm)) {
    AdvancedIdentifiable(variable).followIdentity(vm,
      identifiableStructure.getAdvancedIdentity(vm), false);
  } else if (structure.is<Reference>()) {
    initArrayAt(vm,
      RichNode(*structure.as<Reference>().dest()),
      array, index
    );
  }
}

static inline
void initArrayAtAux(VM vm, RichNode structure,
  StaticArray<StableNode>& array, size_t index, RichNode value) {

  AdvancedIdentifiable identifiableValue = AdvancedIdentifiable(value);
  AdvancedIdentifiable identifiableStructure = AdvancedIdentifiable(structure);
  
  if (identifiableValue.is(vm) && identifiableStructure.is(vm)) {
    identifiableValue.followIdentity(vm,
      identifiableStructure.getAdvancedIdentity(vm), false);
  } else if (structure.is<Reference>()) {
    initArrayAtAux(vm,
      RichNode(*structure.as<Reference>().dest()),
      array, index, value
    );
  } else if (value.is<Reference>()) {
    initArrayAtAux(vm,
      structure,
      array, index,
      *value.as<Reference>().dest()
    );
  }
}

void initArrayAt(VM vm, RichNode structure,
  StaticArray<StableNode>& array, size_t index, StableNode& value) {

  array[index].init(vm, value);

  initArrayAtAux(vm, structure, array, index, RichNode(value));
}

void initArrayAt(VM vm, RichNode structure,
  StaticArray<StableNode>& array, size_t index, UnstableNode& value) {

  array[index].init(vm, value);

  initArrayAtAux(vm, structure, array, index, RichNode(value));
}


}