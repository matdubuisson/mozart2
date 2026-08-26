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
#include "modidentity.hh"

namespace mozart {

namespace builtins {

void ModIdentity::GetId::call(VM vm, In object, Out result) {
  if (Identifiable(object).is(vm)) {
    result = build(vm, Identifiable(object).getId(vm));
  } else {
    result = build(vm, "none");
  }
}

void ModIdentity::SetId::call(VM vm, In object, In idNode) {
  size_t id = getArgument<size_t>(vm, idNode);

  if (object.is<OptVar>()) {
    UnstableNode variable = Variable::build(vm);
    Identifiable(variable).setId(vm, id);
    DataflowVariable(object).bind(vm, RichNode(variable));
  } else {
    Identifiable identifiable = Identifiable(object);
    if (identifiable.is(vm)) {
      identifiable.setId(vm, id);
    }
  }
}

}

}
