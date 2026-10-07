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

// Variables getters

void ModIntrospection::GetVariable::call(VM vm, In variableIdNode, Out result) {
  size_t variableId = getArgument<size_t>(vm, variableIdNode);
  Introspection::VariableState variable = vm->getIntrospection().getVariable(vm, variableId);
  result = Introspection::buildVariable(vm, variable);
}

void ModIntrospection::GetThreadVariables::call(VM vm, In runnableNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
  Introspection::IdToVariableStateMap map = vm->getIntrospection()
    .getVariables(vm, runnable);
  result = Introspection::buildMap(vm, map);
}

void ModIntrospection::GetVariables::call(VM vm, Out result) {
  Introspection::IdToVariableStateMap map = vm->getIntrospection()
    .getVariables(vm);
  result = Introspection::buildMap(vm, map);
}

}

}