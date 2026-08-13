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

// Thread state aggregate

UnstableNode ModIntrospection::buildThreadStateRecord(VM vm, Runnable* runnable) {
  UnstableNode id = build(vm, runnable->getId());
  UnstableNode kindId = build(vm, runnable->getKindId());
  UnstableNode generationId = build(vm, runnable->getGenerationId());
  UnstableNode isRunnable = build(vm, runnable->isRunnable());
  UnstableNode isTerminated = build(vm, runnable->isTerminated());
  UnstableNode isDead = build(vm, runnable->isDead());
  UnstableNode isPreempted = build(vm, runnable->isPreempted());
  UnstableNode isPreemptible = build(vm, runnable->isPreemptible());

  UnstableNode type;
  UnstableNode priority;

  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    type = build(vm, "thread");
    switch (thread->getPriority()) {
      case tpLow: priority = build(vm, "low"); break;
      case tpMiddle: priority = build(vm, "medium"); break;
      case tpHi: priority = build(vm, "high"); break;
      case tpSystem: priority = build(vm, "system"); break;
      default: assert(false);
    }
  } else {
    type = build(vm, "runnable");
    priority = build(vm, "none");
  }

  return buildRecord(vm,
    buildArity(vm,
      "state",
      "dead",
      "generationId",
      "id",
      "kindId",
      "preempted",
      "preemptible",
      "priority",
      "runnable",
      "terminated",
      "type"
    ),
    isDead,
    generationId,
    id,
    kindId,
    isPreempted,
    isPreemptible,
    priority,
    isRunnable,
    isTerminated,
    type
  );
}

void ModIntrospection::GetThreadState::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = buildThreadStateRecord(vm, runnable);
}

void ModIntrospection::GetAllThreadStates::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadRecordsList(vm, from, to,
    buildThreadStateRecord);
}

// Thread statistics aggregate

UnstableNode ModIntrospection::buildThreadStatisticsRecord(VM vm, Runnable* runnable) {
  Runnable::Statistics runnableStatistics = runnable->Runnable::getStatistics();

  size_t operationsCount = 0, bindsCount = 0;
  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    Thread::Statistics threadStatistics = thread->getStatistics();
    operationsCount = threadStatistics.operationsCount;
    bindsCount = threadStatistics.bindsCount;
  }

  return buildRecord(vm,
    buildArity(vm,
      "statistics",
      "bindsCount",
      "id",
      "operationsCount",
      "resumesCount",
      "runsCount",
      "suspendsCount",
      "suspendsOnVarCount"
    ),
    build(vm, bindsCount),
    build(vm, runnable->getId()),
    build(vm, operationsCount),
    build(vm, runnableStatistics.resumesCount),
    build(vm, runnableStatistics.runsCount),
    build(vm, runnableStatistics.suspendsCount),
    build(vm, runnableStatistics.suspendsOnVarCount)
  );
}

void ModIntrospection::GetThreadStatistics::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = buildThreadStatisticsRecord(vm, runnable);
}

void ModIntrospection::GetAllThreadStatistics::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadRecordsList(vm, from, to,
    buildThreadStatisticsRecord);
}

// Thread nodes aggregate

UnstableNode ModIntrospection::buildThreadNodesCountsRecord(VM vm, Runnable* runnable) {
  Introspection::NodesCounts properties =
    vm->getIntrospection().getNodesCounts(vm, runnable);
  return buildRecord(vm,
    buildArity(vm,
      "nodes",
      "gNodesCount",
      "id",
      "kNodesCount",
      "nodesCount",
      "stableNodesCount",
      "stackDepth",
      "structuralNodesCount",
      "tokenNodesCount",
      "unstableNodesCount",
      "valueNodesCount",
      "variableNodesCount",
      "xNodesCount",
      "yNodesCount"
    ),
    build(vm, properties.gNodesCount),
    build(vm, runnable->getId()),
    build(vm, properties.kNodesCount),
    build(vm, properties.nodesCount),
    build(vm, properties.stableNodesCount),
    build(vm, properties.stackDepth),
    build(vm, properties.structuralNodesCount),
    build(vm, properties.tokenNodesCount),
    build(vm, properties.unstableNodesCount),
    build(vm, properties.valueNodesCount),
    build(vm, properties.variableNodesCount),
    build(vm, properties.xNodesCount),
    build(vm, properties.yNodesCount)
  );
}

void ModIntrospection::GetThreadNodesCounts::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = buildThreadNodesCountsRecord(vm, runnable);
}

void ModIntrospection::GetAllThreadNodesCounts::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadRecordsList(vm, from, to,
    buildThreadNodesCountsRecord);
}

// Thread status aggregate

UnstableNode ModIntrospection::buildThreadStatusRecord(VM vm, Runnable* runnable) {
  std::string type;
  if (dynamic_cast<Thread*>(runnable)) type = "thread";
  else type = "runnable";

  return buildRecord(vm,
    buildArity(vm,
      "state",
      "nodes",
      "state",
      "statistics"
    ),
    buildThreadNodesCountsRecord(vm, runnable),
    buildThreadStateRecord(vm, runnable),
    buildThreadStatisticsRecord(vm, runnable)
  );
}

void ModIntrospection::GetThreadStatus::call(VM vm, In threadNode, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  result = buildThreadStatusRecord(vm, runnable);
}

void ModIntrospection::GetAllThreadStatus::call(VM vm, In fromNode, In toNode, Out result) {
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  result = buildThreadRecordsList(vm, from, to,
    buildThreadStatusRecord);
}

}

}
