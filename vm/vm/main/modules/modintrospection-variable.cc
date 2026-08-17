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

void ModIntrospection::buildVariablePendingsList(VM vm, OzListBuilder& builder,
  Pendings& pendings) {
  for (Pendings::iterator iter = pendings.begin(); iter != pendings.end(); ++iter) {
    RichNode node = RichNode(*static_cast<StableNode*>(*iter));
    if (node.is<ReifiedThread>()) {
      Runnable* runnable = getArgument<Runnable*>(vm, node);
      builder.push_back(vm, build(vm, runnable->getId()));
    }
  }
}

UnstableNode ModIntrospection::buildVariableRecord(VM vm, Introspection::VariableCandidates& variableCandidates) {
  if (variableCandidates.isNull())
    return build(vm, "none");

  RichNode& node = variableCandidates.node;

  size_t id = SIZE_MAX, kindId = SIZE_MAX, generationId = SIZE_MAX;
  bool isBound = false, isNeeded = false;
  std::string type = node.type()->getName();
  std::string representation = nodeToString(vm, node);

  OzListBuilder pendingsList(vm);

  if (node.is<OptVar>()) {
    OptVar variable = Accessor<OptVar>::get(node.value());
    isBound = false;
    isNeeded = variable.isNeeded(vm);
  } else if (node.is<Variable>()) {
    Variable variable = Accessor<Variable>::get(node.value());
    id = variable.getId();
    kindId = variable.getKindId();
    generationId = variable.getGenerationId();
    isBound = variable.isBound(vm);
    isNeeded = variable.isNeeded(vm);
    buildVariablePendingsList(vm, pendingsList, variable.getPendings(vm));
  } else if (node.is<ReadOnly>()) {
    ReadOnly variable = Accessor<ReadOnly>::get(node.value());
    isBound = true;
    isNeeded = variable.isNeeded(vm);
  } else if (node.is<ReadOnlyVariable>()) {
    ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
    id = variable.getId();
    kindId = variable.getKindId();
    generationId = variable.getGenerationId();
    isBound = variable.isBound(vm);
    isNeeded = variable.isNeeded(vm);
    buildVariablePendingsList(vm, pendingsList, variable.getPendings(vm));
  } else if (node.is<FailedValue>()) {
    FailedValue variable = Accessor<FailedValue>::get(node.value());
    isBound = true;
    isNeeded = variable.isNeeded(vm);
  } else {
    assert(false);
  }

  Introspection::CandidatesList& candidates = variableCandidates.candidates;
  OzListBuilder candidatesList(vm);

  for (size_t candidateThreadId : candidates) {
    candidatesList.push_back(vm, build(vm, candidateThreadId));
  }

  return buildRecord(vm,
    buildArity(vm,
      "variable",
      "candidates",
      "generationId",
      "id",
      "isBound",
      "isNeeded",
      "kindId",
      "pendings",
      "type",
      "value"
    ),
    candidatesList.get(vm),
    build(vm, generationId),
    build(vm, id),
    build(vm, isBound),
    build(vm, isNeeded),
    build(vm, kindId),
    pendingsList.get(vm),
    build(vm, type.c_str()),
    build(vm, representation.c_str())
  );
}

UnstableNode ModIntrospection::buildVariableRecordsList(VM vm, Introspection::VariableCandidatesMap& map) {
  OzListBuilder builder(vm);

  for (auto iter = map.begin(); iter != map.end(); ++iter) {
    builder.push_back(vm, buildVariableRecord(vm, iter->second));
  }

  return builder.get(vm);
}

void ModIntrospection::GetVariable::call(VM vm, In variableIdNode, Out result) {
  size_t variableId = getArgument<size_t>(vm, variableIdNode);
  Introspection::VariableCandidates variable = vm->getIntrospection().getVariable(vm, variableId);
  result = buildVariableRecord(vm, variable);
}

void ModIntrospection::GetThreadVariables::call(VM vm, In runnableNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
  Introspection::VariableCandidatesMap map = vm->getIntrospection().getVariableCandidatesMap(vm, runnable);
  result = buildVariableRecordsList(vm, map);
}

void ModIntrospection::GetAllVariables::call(VM vm, Out result) {
  Introspection::VariableCandidatesMap map = vm->getIntrospection().getVariableCandidatesMap(vm);
  result = buildVariableRecordsList(vm, map);
}
}

}