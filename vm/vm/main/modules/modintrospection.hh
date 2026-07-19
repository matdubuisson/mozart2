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

#ifndef MOZART_MODINTROSPECTION_H
#define MOZART_MODINTROSPECTION_H

#include "../mozartcore.hh"
#include "../introspection.hh"

#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModIntrospection: public Module {
public:
  ModIntrospection(): Module("Introspection") {}

  class Hello: public Builtin<Hello> {
  public:
    Hello(): Builtin("hello") {}

    static void call(VM vm) {
      std::cout << "Hello from introspection module !!" << std::endl;
    }
  };

  class Value: public Builtin<Value> {
  public:
    Value(): Builtin("value") {}

    static void call(VM vm, In inValue, Out outValue) {
      outValue = buildRecord(vm,
        buildArity(vm, "test", "a", "b", "c"),
        build(vm, 1),
        build(vm, 2),
        build(vm, 3)
      );
    }
  };

  static inline
  std::string nodeToString(VM vm, RichNode node) {
    auto& config = vm->getPropertyRegistry().config;
    std::basic_stringstream<char> buffer;
    buffer << repr(vm, node, config.printDepth, config.printWidth);
    return buffer.str();
  }

  /* ========== Virtual Machine stats ========== */  

  class GetSchedulesCounter: public Builtin<GetSchedulesCounter> {
  public:
    GetSchedulesCounter(): Builtin("getSchedulesCounter") {}

    static void call(VM vm, Out result) {
      Introspection& introspection = vm->getIntrospection();
      result = build(vm, introspection.getSchedulesCounter(vm));
    }
  };

  class GetOperationsCounter: public Builtin<GetOperationsCounter> {
  public:
    GetOperationsCounter(): Builtin("getOperationsCounter") {}

    static void call(VM vm, Out result) {
      Introspection& introspection = vm->getIntrospection();
      result = build(vm, introspection.getOperationsCounter(vm));
    }
  };


  class GetNextScheduledThread: public Builtin<GetNextScheduledThread> {
  public:
    GetNextScheduledThread(): Builtin("getNextScheduledThread") {}

    static void call(VM vm, In boolean, Out result) {
      bool includeSystemThreads = getArgument<bool>(vm, boolean);

      if (vm->getThreadPool().empty(includeSystemThreads)) {
        result = build(vm, "none");
      } else {
        result = ReifiedThread::build(vm,
          vm->getIntrospection().getNextScheduledThread(vm, includeSystemThreads));
      }
    }
  };

  using Operation = Introspection::Operation;
  using OperationArgument = Introspection::OperationArgument;

  static inline
  UnstableNode buildOperationArgumentRecord(VM vm, OperationArgument argument) {
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

  static inline
  UnstableNode buildOperationRecord(VM vm, Operation operation) {
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

  class GetNextOperation: public Builtin<GetNextOperation> {
  public:
    GetNextOperation(): Builtin("getNextOperation") {}

    static void call(VM vm, In includeSystemThreadsNode, Out result) {
      bool includeSystemThreads = getArgument<bool>(vm, includeSystemThreadsNode);

      Operation operation = vm->getIntrospection()
        .getNextExecutedOperation(vm, includeSystemThreads);

      result = buildOperationRecord(vm, operation);
    }
  };

  /* ========== Threads ========== */

  // Thread accessors

  class GetThreadIds: public Builtin<GetThreadIds> {
  public:
    GetThreadIds(): Builtin("getThreadIds") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadAggregatesList(vm, from, to,
        [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, build(vm, runnable->getId()));
      });
    }
  };


  class GetThread: public Builtin<GetThread> {
  public:
    GetThread(): Builtin("getThread") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);

      if (runnable)
        result = ReifiedThread::build(vm, runnable);
      else
        result = build(vm, "none");
    }
  };

  class GetThreads: public Builtin<GetThreads> {
  public:
    GetThreads(): Builtin("getThreads") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadAggregatesList(vm, from, to,
        [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, ReifiedThread::build(vm, runnable));
      });
    }
  };

  // Thread counters

  class GetActiveThreadsCount: public Builtin<GetActiveThreadsCount> {
  public:
    GetActiveThreadsCount(): Builtin("getActiveThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, vm->getIntrospection().getActiveThreadsCount(vm));
    }
  };

  class GetPassiveThreadsCount: public Builtin<GetPassiveThreadsCount> {
  public:
    GetPassiveThreadsCount(): Builtin("getPassiveThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, vm->getIntrospection().getPassiveThreadsCount(vm));
    }
  };

  class GetThreadsCount: public Builtin<GetThreadsCount> {
  public:
    GetThreadsCount(): Builtin("getThreadsCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, vm->getIntrospection().getThreadsCount(vm));
    }
  };

  // Thread aggregates

  static inline
  UnstableNode buildThreadAggregatesList(VM vm, size_t from, size_t to,
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

  static inline
  UnstableNode buildThreadRecordsList(VM vm, size_t from, size_t to,
      std::function<UnstableNode(VM vm, Runnable* runnable)> recordBuilder) {
    return buildThreadAggregatesList(vm, from, to,
      [recordBuilder](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, recordBuilder(vm, runnable));
      }
    );
  }

  // Thread state aggregate

  static inline
  UnstableNode buildThreadStateRecord(VM vm, Runnable* runnable) {
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

  class GetThreadState: public Builtin<GetThreadState> {
  public:
    GetThreadState(): Builtin("getThreadState") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadStateRecord(vm, runnable);
    }
  };

  class GetAllThreadStates: public Builtin<GetAllThreadStates> {
  public:
    GetAllThreadStates(): Builtin("getAllThreadStates") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadRecordsList(vm, from, to,
        buildThreadStateRecord);
    }
  };

  // Thread statistics aggregate

  static inline
  UnstableNode buildThreadStatisticsRecord(VM vm, Runnable* runnable) {
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

  class GetThreadStatistics: public Builtin<GetThreadStatistics> {
  public:
    GetThreadStatistics(): Builtin("getThreadStatistics") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadStatisticsRecord(vm, runnable);
    }
  };

  class GetAllThreadStatistics: public Builtin<GetAllThreadStatistics> {
  public:
    GetAllThreadStatistics(): Builtin("getAllThreadStatistics") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadRecordsList(vm, from, to,
        buildThreadStatisticsRecord);
    }
  };

  // Thread nodes aggregate

  static inline
  UnstableNode buildThreadNodesCountsRecord(VM vm, Runnable* runnable) {
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

  class GetThreadNodesCounts: public Builtin<GetThreadNodesCounts> {
  public:
    GetThreadNodesCounts(): Builtin("getThreadNodesCounts") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadNodesCountsRecord(vm, runnable);
    }
  };

  class GetAllThreadNodesCounts: public Builtin<GetAllThreadNodesCounts> {
  public:
    GetAllThreadNodesCounts(): Builtin("getAllThreadNodesCounts") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadRecordsList(vm, from, to,
        buildThreadNodesCountsRecord);
    }
  };

  // Thread status aggregate

  static inline
  UnstableNode buildThreadStatusRecord(VM vm, Runnable* runnable) {
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

  class GetThreadStatus: public Builtin<GetThreadStatus> {
  public:
    GetThreadStatus(): Builtin("getThreadStatus") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadStatusRecord(vm, runnable);
    }
  };

  class GetAllThreadStatus: public Builtin<GetAllThreadStatus> {
  public:
    GetAllThreadStatus(): Builtin("getAllThreadStatus") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = buildThreadRecordsList(vm, from, to,
        buildThreadStatusRecord);
    }
  };

  /* ========== Nodes ========== */

  static inline
  std::string nodeStructuralBehaviorToString(StructuralBehavior behavior) {
    switch (behavior) {
      case sbVariable: return "variable";
      case sbValue: return "value";
      case sbStructural: return "structural";
      case sbTokenEq: return "tokenEq";
      default: assert(false); return "";
    }
  }

  using NodesRegister = Introspection::NodesRegister;

  static inline
  std::string getRegisterName(NodesRegister nodesRegister) {
    switch (nodesRegister) {
      case NodesRegister::xRegister: return "X";
      case NodesRegister::yRegister: return "Y";
      case NodesRegister::gRegister: return "G";
      case NodesRegister::kRegister: return "K";
      default: assert(false);
    }
  }

  // Nodes counters

  static inline
  UnstableNode buildNodesCountsRecord(VM vm, Introspection::NodesCounts& properties) {
    return buildRecord(vm,
      buildArity(vm,
        "nodes",
        "gNodesCount",
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
  
  class GetNodesCounts: public Builtin<GetNodesCounts> {
  public:
    GetNodesCounts(): Builtin("getNodesCounts") {}

    static void call(VM vm, Out result) {
      Introspection::NodesCounts properties =
        vm->getIntrospection().getNodesCounts(vm);
      result = buildNodesCountsRecord(vm, properties);
    }
  };

  class GetVariableNodesCount: public Builtin<GetVariableNodesCount> {
  public:
    GetVariableNodesCount(): Builtin("getVariableNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getVariableNodesCount(vm));
    }
  };

  class GetThreadVariableNodesCount: public Builtin<GetThreadVariableNodesCount> {
  public:
    GetThreadVariableNodesCount(): Builtin("getThreadVariableNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getVariableNodesCount(vm, runnable));
    }
  };

  class GetValueNodesCount: public Builtin<GetValueNodesCount> {
  public:
    GetValueNodesCount(): Builtin("getValueNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getValueNodesCount(vm));
    }
  };

  class GetThreadValueNodesCount: public Builtin<GetThreadValueNodesCount> {
  public:
    GetThreadValueNodesCount(): Builtin("getThreadValueNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getValueNodesCount(vm, runnable));
    }
  };

  class GetStructuralNodesCount: public Builtin<GetStructuralNodesCount> {
  public:
    GetStructuralNodesCount(): Builtin("getStructuralNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getStructuralNodesCount(vm));
    }
  };

  class GetThreadStructuralNodesCount: public Builtin<GetThreadStructuralNodesCount> {
  public:
    GetThreadStructuralNodesCount(): Builtin("getThreadStructuralNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getStructuralNodesCount(vm, runnable));
    }
  };

  class GetTokenNodesCount: public Builtin<GetTokenNodesCount> {
  public:
    GetTokenNodesCount(): Builtin("getTokenNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getTokenNodesCount(vm));
    }
  };

  class GetThreadTokenNodesCount: public Builtin<GetThreadTokenNodesCount> {
  public:
    GetThreadTokenNodesCount(): Builtin("getThreadTokenNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getTokenNodesCount(vm, runnable));
    }
  };

  class GetStableNodesCount: public Builtin<GetStableNodesCount> {
  public:
    GetStableNodesCount(): Builtin("getStableNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getStableNodesCount(vm));
    }
  };

  class GetThreadStableNodesCount: public Builtin<GetThreadStableNodesCount> {
  public:
    GetThreadStableNodesCount(): Builtin("getThreadStableNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getStableNodesCount(vm, runnable));
    }
  };

  class GetUnstableNodesCount: public Builtin<GetUnstableNodesCount> {
  public:
    GetUnstableNodesCount(): Builtin("getUnstableNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getUnstableNodesCount(vm));
    }
  };

  class GetThreadUnstableNodesCount: public Builtin<GetThreadUnstableNodesCount> {
  public:
    GetThreadUnstableNodesCount(): Builtin("getThreadUnstableNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getUnstableNodesCount(vm, runnable));
    }
  };

  class GetXNodesCount: public Builtin<GetXNodesCount> {
  public:
    GetXNodesCount(): Builtin("getXNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getXNodesCount(vm));
    }
  };

  class GetThreadXNodesCount: public Builtin<GetThreadXNodesCount> {
  public:
    GetThreadXNodesCount(): Builtin("getThreadXNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getXNodesCount(vm, runnable));
    }
  };

  class GetYNodesCount: public Builtin<GetYNodesCount> {
  public:
    GetYNodesCount(): Builtin("getYNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getYNodesCount(vm));
    }
  };

  class GetThreadYNodesCount: public Builtin<GetThreadYNodesCount> {
  public:
    GetThreadYNodesCount(): Builtin("getThreadYNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getYNodesCount(vm, runnable));
    }
  };

  class GetGNodesCount: public Builtin<GetGNodesCount> {
  public:
    GetGNodesCount(): Builtin("getGNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getGNodesCount(vm));
    }
  };

  class GetThreadGNodesCount: public Builtin<GetThreadGNodesCount> {
  public:
    GetThreadGNodesCount(): Builtin("getThreadGNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getGNodesCount(vm, runnable));
    }
  };

  class GetKNodesCount: public Builtin<GetKNodesCount> {
  public:
    GetKNodesCount(): Builtin("getKNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getKNodesCount(vm));
    }
  };

  class GetThreadKNodesCount: public Builtin<GetThreadKNodesCount> {
  public:
    GetThreadKNodesCount(): Builtin("getThreadKNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getKNodesCount(vm, runnable));
    }
  };

  class GetStackDepth: public Builtin<GetStackDepth> {
  public:
    GetStackDepth(): Builtin("getStackDepth") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getStackDepth(vm));
    }
  };

  class GetThreadStackDepth: public Builtin<GetThreadStackDepth> {
  public:
    GetThreadStackDepth(): Builtin("getThreadStackDepth") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getStackDepth(vm, runnable));
    }
  };

  class GetNodesCount: public Builtin<GetNodesCount> {
  public:
    GetNodesCount(): Builtin("getNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getNodesCount(vm));
    }
  };

  class GetThreadNodesCount: public Builtin<GetThreadNodesCount> {
  public:
    GetThreadNodesCount(): Builtin("getThreadNodesCount") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      result = build(vm,
        vm->getIntrospection().getNodesCount(vm, runnable));
    }
  };

  // Register types sizes

  static inline
  UnstableNode getThreadNodesRegisterSize(VM vm, In threadNode, In depthNode,
    NodesRegister nodesRegister) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t depth = getArgument<size_t>(vm, depthNode);
    Introspection& introspection = vm->getIntrospection();

    size_t count;
    switch (nodesRegister) {
      case NodesRegister::yRegister:
        count = introspection.getYNodesRegisterSize(vm, runnable, depth); break;
      case NodesRegister::gRegister:
        count = introspection.getGNodesRegisterSize(vm, runnable, depth); break;
      case NodesRegister::kRegister:
        count = introspection.getKNodesRegisterSize(vm, runnable, depth); break;
      default: assert(false);
    }

    return build(vm, count);
  }

  class GetThreadXNodesRegisterSize: public Builtin<GetThreadXNodesRegisterSize> {
  public:
    GetThreadXNodesRegisterSize(): Builtin("getThreadXNodesRegisterSize") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = build(vm,
        vm->getIntrospection().getXNodesRegisterSize(vm, runnable));
    }
  };

  class GetThreadYNodesRegisterSize: public Builtin<GetThreadYNodesRegisterSize> {
  public:
    GetThreadYNodesRegisterSize(): Builtin("getThreadYNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::yRegister);
    }
  };

  class GetThreadGNodesRegisterSize: public Builtin<GetThreadGNodesRegisterSize> {
  public:
    GetThreadGNodesRegisterSize(): Builtin("getThreadGNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::gRegister);
    }
  };

  class GetThreadKNodesRegisterSize: public Builtin<GetThreadKNodesRegisterSize> {
  public:
    GetThreadKNodesRegisterSize(): Builtin("getThreadKNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::kRegister);
    }
  };

  // Nodes getters

  static inline
  UnstableNode buildNodeRecord(VM vm, RichNode node) {
    Type type = node.type();

    return buildRecord(vm,
      buildArity(vm,
        "node",
        "bindingPriority",
        "copyable",
        "feature",
        "id",
        "name",
        "structuralBehavior",
        "transient",
        "uuid",
        "value"
      ),
      build(vm, type->getBindingPriority()),
      build(vm, type->isCopyable()),
      build(vm, type->isFeature()),
      build(vm, node.getId()),
      build(vm, type->getName().c_str()),
      build(vm,
        nodeStructuralBehaviorToString(
          type->getStructuralBehavior()
        ).c_str()
      ),
      build(vm, type->isTransient()),
      build(vm, type->getUUID()),
      build(vm, nodeToString(vm, node).c_str())
    );
  }

  static inline
  UnstableNode getThreadXNode(VM vm, In threadNode, In indexNode) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t index = getArgument<size_t>(vm, indexNode);
    Introspection& introspection = vm->getIntrospection();

    assert(index < introspection.getXNodesRegisterSize(vm, runnable));
    RichNode node = introspection.getXNode(vm, runnable, index);
    return buildNodeRecord(vm, node);
  }

  static inline
  UnstableNode getThreadNode(VM vm, In threadNode, In depthNode,
    In indexNode, NodesRegister nodesRegister) {
    
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t depth = getArgument<size_t>(vm, depthNode);
    size_t index = getArgument<size_t>(vm, indexNode);
    Introspection& introspection = vm->getIntrospection();

    assert(depth < introspection.getStackDepth(vm, runnable));
    RichNode node;
    switch (nodesRegister) {
      case NodesRegister::yRegister: {
        assert(index < introspection.getYNodesRegisterSize(vm, runnable, depth));
        node = introspection.getYNode(vm, runnable, depth, index);
        break;
      } case NodesRegister::gRegister: {
        assert(index < introspection.getGNodesRegisterSize(vm, runnable, depth));
        node = introspection.getGNode(vm, runnable, depth, index);
        break;
      } case NodesRegister::kRegister: {
        assert(index < introspection.getKNodesRegisterSize(vm, runnable, depth));
        node = introspection.getKNode(vm, runnable, depth, index);
        break;
      } default: assert(false);
    }

    return buildNodeRecord(vm, node);
  }

  class GetThreadXNode: public Builtin<GetThreadXNode> {
  public:
    GetThreadXNode(): Builtin("getThreadXNode") {}

    static void call(VM vm, In threadNode, In indexNode, Out result) {
      result = getThreadXNode(vm, threadNode, indexNode);
    }
  };

  class GetThreadYNode: public Builtin<GetThreadYNode> {
  public:
    GetThreadYNode(): Builtin("getThreadYNode") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, Out result) {
      result = getThreadNode(vm, threadNode, depthNode, indexNode, NodesRegister::yRegister);
    }
  };

  class GetThreadGNode: public Builtin<GetThreadGNode> {
  public:
    GetThreadGNode(): Builtin("getThreadGNode") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, Out result) {
      result = getThreadNode(vm, threadNode, depthNode, indexNode, NodesRegister::gRegister);
    }
  };

  class GetThreadKNode: public Builtin<GetThreadKNode> {
  public:
    GetThreadKNode(): Builtin("getThreadKNode") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, Out result) {
      result = getThreadNode(vm, threadNode, depthNode, indexNode, NodesRegister::kRegister);
    }
  };

  // Nodes lists getters

  static inline
  UnstableNode getThreadXNodes(VM vm, In threadNode, In fromNode, In toNode) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t from = getArgument<size_t>(vm, fromNode);
    size_t to = getArgument<size_t>(vm, toNode);
    Introspection& introspection = vm->getIntrospection();

    assert(from < to);
    // assert(to < introspection.getXNodesRegisterSize(vm, runnable));

    OzListBuilder builder(vm);
    introspection.doForEachXNode(vm, runnable, from, to,
      Introspection::allNodes,
      [&builder](VM vm, Runnable* runnable, RichNode node) {
        builder.push_back(vm, buildNodeRecord(vm, node));
      }
    );
    return builder.get(vm);
  }

  static inline
  UnstableNode getThreadNodes(VM vm, In threadNode, In depthNode, In fromNode, In toNode,
    NodesRegister nodesRegister) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t depth = getArgument<size_t>(vm, depthNode);
    size_t from = getArgument<size_t>(vm, fromNode);
    size_t to = getArgument<size_t>(vm, toNode);
    Introspection& introspection = vm->getIntrospection();

    assert(depth < introspection.getStackDepth(vm, runnable));
    assert(from < to);

    OzListBuilder builder(vm);

    switch (nodesRegister) {
      case NodesRegister::yRegister: {
        // assert(to < introspection.getYNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachYNode(vm, runnable, depth, from, to,
          Introspection::allNodes,
          [&builder](VM vm, Runnable* runnable, RichNode node) {
            builder.push_back(vm, buildNodeRecord(vm, node));
          }
        );
        break;
      } case NodesRegister::gRegister: {
        // assert(to < introspection.getGNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachGNode(vm, runnable, depth, from, to,
          Introspection::allNodes,
          [&builder](VM vm, Runnable* runnable, RichNode node) {
            builder.push_back(vm, buildNodeRecord(vm, node));
          }
        );
        break;
      } case NodesRegister::kRegister: {
        // assert(to < introspection.getKNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachKNode(vm, runnable, depth, from, to,
          Introspection::allNodes,
          [&builder](VM vm, Runnable* runnable, RichNode node) {
            builder.push_back(vm, buildNodeRecord(vm, node));
          }
        );
        break;
      } default: assert(false);
    }

    return builder.get(vm);
  }

  class GetThreadXNodes: public Builtin<GetThreadXNodes> {
  public:
    GetThreadXNodes(): Builtin("getThreadXNodes") {}

    static void call(VM vm, In threadNode, In fromNode, In toNode, Out result) {
      result = getThreadXNodes(vm, threadNode, fromNode, toNode);
    }
  };

  class GetThreadYNodes: public Builtin<GetThreadYNodes> {
  public:
    GetThreadYNodes(): Builtin("getThreadYNodes") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, Out result) {
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, NodesRegister::yRegister);
    }
  };

  class GetThreadGNodes: public Builtin<GetThreadGNodes> {
  public:
    GetThreadGNodes(): Builtin("getThreadGNodes") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, Out result) {
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, NodesRegister::gRegister);
    }
  };

  class GetThreadKNodes: public Builtin<GetThreadKNodes> {
  public:
    GetThreadKNodes(): Builtin("getThreadKNodes") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, Out result) {
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, NodesRegister::kRegister);
    }
  };

  class GetNodes: public Builtin<GetNodes> {
  public:
    GetNodes(): Builtin("getNodes") {}

    static void call(VM vm, In nodeFamily, In fromNode, In toNode, Out result) {
      using namespace patternmatching;

      Introspection& introspection = vm->getIntrospection();
      Introspection::NodeBoolLambda filter;
      if (matches(vm, nodeFamily, "variable")) {
        filter = [&introspection](VM vm, RichNode node) {
          return introspection.isVariableNode(vm, node);
        };
      } else if (matches(vm, nodeFamily, "token")) {
        filter = [&introspection](VM vm, RichNode node) {
          return introspection.isTokenNode(vm, node);
        };
      } else if (matches(vm, nodeFamily, "structural")) {
        filter = [&introspection](VM vm, RichNode node) {
          return introspection.isStructuralNode(vm, node);
        };
      } else if (matches(vm, nodeFamily, "value")) {
        filter = [&introspection](VM vm, RichNode node) {
          return introspection.isValueNode(vm, node);
        };
      } else {
        filter = Introspection::allNodes;
      }

      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      size_t i = 0;

      OzListBuilder builder(vm);
      vm->getIntrospection().doForEachNode(vm,
        Introspection::allRunnables,
        filter,
        [&builder, from, to, &i](VM vm, Runnable* _, RichNode node) {
          if (i >= from && i < to) { // TODO Ugly make it better
            builder.push_back(vm, buildNodeRecord(vm, node));
          }
          i++;
        }
      );

      result = builder.get(vm);
    }
  };

  /* ========== Variables stats ========== */

  // Variables counters

  class GetBoundVariablesCount: public Builtin<GetBoundVariablesCount> {
  public:
    GetBoundVariablesCount(): Builtin("getBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm, vm->getIntrospection().getBoundVariablesCount(vm));
    }
  };

  class GetUnBoundVariablesCount: public Builtin<GetUnBoundVariablesCount> {
  public:
    GetUnBoundVariablesCount(): Builtin("getUnBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm, vm->getIntrospection().getUnBoundVariablesCount(vm));
    }
  };

  class GetVariablesCount: public Builtin<GetVariablesCount> {
  public:
    GetVariablesCount(): Builtin("getVariablesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm, vm->getIntrospection().getVariablesCount(vm));
    }
  };

  // Variables getters

  static inline
  void buildVariablePendingsList(VM vm, OzListBuilder& builder,
    VMAllocatedList<StableNode*>& pendings) {
    for (size_t i = 0; i < pendings.size(); i++) {
      RichNode node = RichNode(*pendings[i]);
      if (node.is<ReifiedThread>()) {
        Runnable* runnable = getArgument<Runnable*>(vm, node);
        builder.push_back(vm, build(vm, runnable->getId()));
      }
    }
  }

  static inline
  UnstableNode buildVariableRecord(VM vm, Introspection::VariableCandidates& variableCandidates) {
    if (variableCandidates.isNull())
      return build(vm, "none");

    RichNode& node = variableCandidates.node;

    size_t id = 0;
    bool isBound = false, isNeeded = false;
    std::string type = node.type()->getName();
    std::string representation = nodeToString(vm, node);

    OzListBuilder pendingsList(vm);

    if (node.is<OptVar>()) {
      OptVar variable = Accessor<OptVar>::get(node.value());
      id = SIZE_MAX;
      isBound = false;
      isNeeded = variable.isNeeded(vm);
    } else if (node.is<Variable>()) {
      Variable variable = Accessor<Variable>::get(node.value());
      id = variable.getId();
      isBound = variable.isBound(vm);
      isNeeded = variable.isNeeded(vm);
      buildVariablePendingsList(vm, pendingsList, variable.getPendings(vm));
    } else if (node.is<ReadOnly>()) {
      ReadOnly variable = Accessor<ReadOnly>::get(node.value());
      id = SIZE_MAX;
      isBound = true;
      isNeeded = variable.isNeeded(vm);
    } else if (node.is<ReadOnlyVariable>()) {
      ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
      id = variable.getId();
      isBound = variable.isBound(vm);
      isNeeded = variable.isNeeded(vm);
      buildVariablePendingsList(vm, pendingsList, variable.getPendings(vm));
    } else if (node.is<FailedValue>()) {
      FailedValue variable = Accessor<FailedValue>::get(node.value());
      id = SIZE_MAX;
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
        "id",
        "isBound",
        "isNeeded",
        "pendings",
        "type",
        "value"
      ),
      candidatesList.get(vm),
      build(vm, id),
      build(vm, isBound),
      build(vm, isNeeded),
      pendingsList.get(vm),
      build(vm, type.c_str()),
      build(vm, representation.c_str())
    );
  }

  static inline
  UnstableNode buildVariableRecordsList(VM vm, Introspection::VariableCandidatesMap& map) {
    OzListBuilder builder(vm);

    for (auto iter = map.begin(); iter != map.end(); ++iter) {
      builder.push_back(vm, buildVariableRecord(vm, iter->second));
    }

    return builder.get(vm);
  }

  class GetVariable: public Builtin<GetVariable> {
  public:
    GetVariable(): Builtin("getVariable") {}

    static void call(VM vm, In variableIdNode, Out result) {
      size_t variableId = getArgument<size_t>(vm, variableIdNode);
      Introspection::VariableCandidates variable = vm->getIntrospection().getVariable(vm, variableId);
      result = buildVariableRecord(vm, variable);
    }
  };

  class GetThreadVariables: public Builtin<GetThreadVariables> {
  public:
    GetThreadVariables(): Builtin("getThreadVariables") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      Introspection::VariableCandidatesMap map = vm->getIntrospection().getVariableCandidatesMap(vm, runnable);
      result = buildVariableRecordsList(vm, map);
    }
  };

  class GetAllVariables: public Builtin<GetAllVariables> {
  public:
    GetAllVariables(): Builtin("getAllVariables") {}

    static void call(VM vm, Out result) {
      Introspection::VariableCandidatesMap map = vm->getIntrospection().getVariableCandidatesMap(vm);
      result = buildVariableRecordsList(vm, map);
    }
  };

  /* ========== Structures list ========== */

  static inline
  UnstableNode buildListNodeRecord(VM vm, RichNode node) {
    return buildRecord(vm,
      buildArity(vm,
        "list",
        "hash",
        "id",
        "repr"
      ),
      build(vm, ozListHash(vm, node)),
      build(vm, node.getId()),
      build(vm, nodeToString(vm, node).c_str())
    );
  }

  static inline
  UnstableNode buildListNodesListRecord(VM vm, Introspection::NodesList& list) {
    OzListBuilder builder(vm);
    for (size_t i = 0; i < list.size(); i++) {
      builder.push_back(vm, buildListNodeRecord(vm, list[i]));
    }
    return builder.get(vm);
  }

  class GetThreadLists: public Builtin<GetThreadLists> {
  public:
    GetThreadLists(): Builtin("getThreadLists") {}

    static void call(VM vm, In runnableNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
      Introspection::NodesList list = vm->getIntrospection()
        .getLists(vm, runnable);
      result = buildListNodesListRecord(vm, list);
    }
  };

  class GetLists: public Builtin<GetLists> {
  public:
    GetLists(): Builtin("getLists") {}

    static void call(VM vm, Out result) {
      Introspection::NodesList list = vm->getIntrospection().getLists(vm);
      result = buildListNodesListRecord(vm, list);
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTION_H