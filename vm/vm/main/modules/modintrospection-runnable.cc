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

// Thread aggregates

UnstableNode ModIntrospection::buildThreadAggregatesList(VM vm, size_t from, size_t to,
  std::function<void(VM vm, OzListBuilder& builder, Runnable* runnable)> lambda) {
  OzListBuilder builder(vm);

  RunnableList& runnables = vm->getIntrospection().getThreads(vm);
  size_t i = 0;
  for (RunnableList::iterator iter = runnables.begin();
    iter != runnables.end() && i < to; iter++, i++) {
    if (i < from) continue;

    Runnable* runnable = static_cast<Runnable*>(*iter);
    lambda(vm, builder, runnable);
  }

  return builder.get(vm);
}

UnstableNode ModIntrospection::buildThreadRecordsList(VM vm, size_t from, size_t to,
    std::function<UnstableNode(VM vm, Runnable* runnable)> recordBuilder) {
  return buildThreadAggregatesList(vm, from, to,
    [recordBuilder](VM vm, OzListBuilder& builder, Runnable* runnable) {
      builder.push_back(vm, recordBuilder(vm, runnable));
    }
  );
}

// Thread accessors

void ModIntrospection::GetThreadIds::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadAggregatesList(vm, from, to,
    [](VM vm, OzListBuilder& builder, Runnable* runnable) {
    builder.push_back(vm, build(vm, runnable->getId()));
  });
}

void ModIntrospection::GetThread::call(VM vm, In threadId, Out result) {
  size_t id = getArgument<size_t>(vm, threadId);
  Runnable* runnable = vm->getIntrospection().getThread(vm, id);

  if (runnable)
    result = ReifiedThread::build(vm, runnable);
  else
    result = build(vm, "none");
}

void ModIntrospection::GetThreads::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadAggregatesList(vm, from, to,
    [](VM vm, OzListBuilder& builder, Runnable* runnable) {
    builder.push_back(vm, ReifiedThread::build(vm, runnable));
  });
}

// Thread state aggregate

void ModIntrospection::GetThreadState::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = Introspection::buildThreadState(vm, runnable);
}

// Thread statistics aggregate

void ModIntrospection::GetThreadStatistics::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = Introspection::buildThreadStatistics(vm, runnable);
}

// Thread nodes aggregate

void ModIntrospection::GetThreadNodesCounts::call(VM vm, In threadNode, Out result) {
  Introspection introspection = vm->getIntrospection();
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = Introspection::buildThreadNodesCounts(vm,
    runnable->getId(),
    introspection.getNodesCounts(vm, runnable));
}

}

}
