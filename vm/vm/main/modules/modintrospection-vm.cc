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

void ModIntrospection::GetNextScheduledThread::call(VM vm, In boolean, Out result) {
  bool includeSystemThreads = getArgument<bool>(vm, boolean);

  if (vm->getThreadPool().empty(includeSystemThreads)) {
    result = build(vm, "none");
  } else {
    result = ReifiedThread::build(vm,
      vm->getIntrospection().getNextScheduledThread(vm, includeSystemThreads));
  }
}

UnstableNode ModIntrospection::buildOperationArgumentRecord(VM vm, OperationArgument argument) {
  using AT = Introspection::ArgumentType;

  std::string type;

  switch (argument.type) {
    case AT::I: type = "Int"; break;
    case AT::X: type = "X"; break;
    case AT::Y: type = "Y"; break;
    case AT::G: type = "G"; break;
    case AT::K: type = "K"; break;
    default: assert(false);
  }

  return buildRecord(vm,
    buildArity(
      vm,
      "operationArgument",
      "image",
      "index",
      "type"
    ),
    build(vm, argument.image.c_str()),
    build(vm, argument.index),
    build(vm, type.c_str())
  );
}

UnstableNode ModIntrospection::buildOperationRecord(VM vm, Operation operation) {
  OzListBuilder builder(vm);

  for (OperationArgument opArgument : operation.arguments) {
    builder.push_back(vm, buildOperationArgumentRecord(vm, opArgument));
  }

  return buildRecord(vm,
    buildArity(vm,
      "operation",
      "arguments",
      "name",
      "opCode"
    ),
    builder.get(vm),
    build(vm, operation.name.c_str()),
    build(vm, operation.opCode)
  );
}

void ModIntrospection::GetNextOperation::call(VM vm, In includeSystemThreadsNode, Out result) {
  bool includeSystemThreads = getArgument<bool>(vm, includeSystemThreadsNode);

  Operation operation = vm->getIntrospection()
    .getNextExecutedOperation(vm, includeSystemThreads);

  result = buildOperationRecord(vm, operation);
}

}

}