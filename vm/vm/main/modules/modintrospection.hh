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
      outValue = Numeric(inValue).add(vm, 1);
    }
  };

  /* ========== Virtual Machine stats ========== */
  
  class GetNextScheduledThread: public Builtin<GetNextScheduledThread> {
  public:
    GetNextScheduledThread(): Builtin("getNextScheduledThread") {}

    static void call(VM vm, In boolean, Out result) {
      bool includeSystemThreads = getArgument<bool>(vm, boolean);
      result = ReifiedThread::build(vm,
        vm->getIntrospection().getNextScheduledThread(vm, includeSystemThreads));
    }
  };

  // class Magic: public Builtin<Magic> {
  // public:
  //   Magic(): Builtin("magic") {}

  //   static void call(VM vm, In recordNode) {
  //     Record* record = getArgument<Record*>(vm, recordNode);
  //     std::cout << record->getElement(0) << std::endl;
  //   }
  // };

  /* ========== Threads stats ========== */

  static inline
  UnstableNode getThreadAggregatesList(VM vm, size_t from, size_t to,
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

  class GetThreadIds: public Builtin<GetThreadIds> {
  public:
    GetThreadIds(): Builtin("getThreadIds") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = getThreadAggregatesList(vm, from, to,
        [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, build(vm, runnable->getId()));
      });
    }
  };

  class GetThreads: public Builtin<GetThreads> {
  public:
    GetThreads(): Builtin("getThreads") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = getThreadAggregatesList(vm, from, to,
        [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm,
          ReifiedThread::build(vm, runnable));
      });
    }
  };

  class GetThread: public Builtin<GetThread> {
  public:
    GetThread(): Builtin("getThread") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      result = ReifiedThread::build(vm,
        vm->getIntrospection().getThread(vm, id));
    }
  };

  class GetThreadInfo: public Builtin<GetThreadInfo> {
  public:
    GetThreadInfo(): Builtin("getThreadInfo") {}

    static void call(VM vm, In threadNode, In aggregate, Out result) {
      using namespace patternmatching;

      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      
      if (matches(vm, aggregate, "id")) {
        result = build(vm, runnable->getId());
      } else if (matches(vm, aggregate, "kindId")) {
        result = build(vm, runnable->getKindId());
      } else if (matches(vm, aggregate, "generationId")) {
        result = build(vm, runnable->getGenerationId());
      } else if (matches(vm, aggregate, "priority")) {
        switch (runnable->getPriority()) {
          case tpLow: result = build(vm, "low"); break;
          case tpMiddle: result = build(vm, "medium"); break;
          case tpHi: result = build(vm, "high"); break;
          case tpSystem: result = build(vm, "system"); break;
          default: assert(false);
        }
      } else if (matches(vm, aggregate, "runnable")) {
        result = build(vm, runnable->isRunnable());
      } else if (matches(vm, aggregate, "terminated")) {
        result = build(vm, runnable->isTerminated());
      } else if (matches(vm, aggregate, "dead")) {
        result = build(vm, runnable->isDead());
      } else if (matches(vm, aggregate, "preempted")) {
        result = build(vm, runnable->isPreempted());
      } else if (matches(vm, aggregate, "preemptible")) {
        result = build(vm, runnable->isPreemptible());
      } else if (matches(vm, aggregate, "runsCount")) {
        result = build(vm, runnable->Runnable::getStatistics().runsCount);
      } else if (matches(vm, aggregate, "resumesCount")) {
        result = build(vm, runnable->Runnable::getStatistics().resumesCount);
      } else if (matches(vm, aggregate, "suspendsCount")) {
        result = build(vm, runnable->Runnable::getStatistics().suspendsCount);
      } else if (matches(vm, aggregate, "suspendsOnVarCount")) {
        result = build(vm, runnable->Runnable::getStatistics().suspendsOnVarCount);
      } else if (matches(vm, aggregate, "operationsCount")) {
        if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
          result = build(vm, thread->getStatistics().operationsCount);
        } else {
          result = build(vm, 0);
        }
      } else if (matches(vm, aggregate, "bindsCount")) {
        if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
          result = build(vm, thread->getStatistics().bindsCount);
        } else {
          result = build(vm, 0);
        }
      } else if (matches(vm, aggregate, "type")) {
        if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
          result = build(vm, "thread");
        } else {
          result = build(vm, "runnable");
        }
      } else {
        result = build(vm, "none");
      }
    }
  };

  /* =================================== */

  static inline
  UnstableNode buildThreadStatusRecord(VM vm, Runnable* runnable) {
    UnstableNode id = build(vm, (nativeint) runnable->getId());
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
        "status",
        "id", "type",
        "runnable", "terminated", "dead",
        "preempted", "preemptible",
        "priority"
      ),
      std::move(id), std::move(type),
      std::move(isRunnable), std::move(isTerminated), std::move(isDead),
      std::move(isPreempted), std::move(isPreemptible),
      std::move(priority)
    );
  }

  static inline
  UnstableNode buildThreadNodesPropertiesRecord(VM vm, Runnable* runnable) {
    Introspection::NodesProperties properties =
      vm->getIntrospection().getNodesProperties(vm, runnable);
    return buildNodesPropertiesRecord(vm, properties);
  }

  static inline
  UnstableNode buildThreadStatisticsRecord(VM vm, Runnable* runnable) {
    Runnable::Statistics runnableStatistics = runnable->Runnable::getStatistics();

    if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
      Thread::Statistics threadStatistics = thread->getStatistics();

      return buildRecord(vm,
        buildArity(vm,
          "statistics",
          "runsCount",
          "resumesCount",
          "suspendsCount",
          "suspendsOnVarCount",
          "operationsCount",
          "bindsCount"
        ),
        build(vm, runnableStatistics.runsCount),
        build(vm, runnableStatistics.resumesCount),
        build(vm, runnableStatistics.suspendsCount),
        build(vm, runnableStatistics.suspendsOnVarCount),
        build(vm, threadStatistics.operationsCount),
        build(vm, threadStatistics.bindsCount)
      );
    } else {
      return buildRecord(vm,
        buildArity(vm,
          "statistics",
          "runsCount",
          "resumesCount",
          "suspendsCount",
          "suspendsOnVarCount"
        ),
        build(vm, runnableStatistics.runsCount),
        build(vm, runnableStatistics.resumesCount),
        build(vm, runnableStatistics.suspendsCount),
        build(vm, runnableStatistics.suspendsOnVarCount)
      );
    }
  }

  static inline
  UnstableNode buildThreadStateRecord(VM vm, Runnable* runnable) {
    std::string type;
    if (dynamic_cast<Thread*>(runnable)) type = "thread";
    else type = "runnable";

    return buildRecord(vm,
      buildArity(vm,
        "state",
        "type",
        "status",
        "statistics",
        "nodesProperties"
      ),
      dynamic_cast<Thread*>(runnable) ? build(vm, "thread") :
        build(vm, "runnable"),
      buildThreadStatusRecord(vm, runnable),
      buildThreadStatisticsRecord(vm, runnable),
      buildThreadNodesPropertiesRecord(vm, runnable)
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

  class GetThreadStatusById: public Builtin<GetThreadStatusById> {
  public:
    GetThreadStatusById(): Builtin("getThreadStatusById") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);
      result = buildThreadStatusRecord(vm, runnable);
    }
  };

  class GetThreadStatistics: public Builtin<GetThreadStatistics> {
  public:
    GetThreadStatistics(): Builtin("getThreadStatistics") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadStatisticsRecord(vm, runnable);
    }
  };

  class GetThreadStatisticsById: public Builtin<GetThreadStatisticsById> {
  public:
    GetThreadStatisticsById(): Builtin("getThreadStatisticsById") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);
      result = buildThreadStatisticsRecord(vm, runnable);
    }
  };

  class GetThreadNodesProperties: public Builtin<GetThreadNodesProperties> {
  public:
    GetThreadNodesProperties(): Builtin("getThreadNodesProperties") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadNodesPropertiesRecord(vm, runnable);
    }
  };

  class GetThreadNodesPropertiesById: public Builtin<GetThreadNodesPropertiesById> {
  public:
    GetThreadNodesPropertiesById(): Builtin("getThreadNodesPropertiesById") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);
      result = buildThreadNodesPropertiesRecord(vm, runnable);
    }
  };

  class GetThreadState: public Builtin<GetThreadState> {
  public:
    GetThreadState(): Builtin("getThreadState") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadStateRecord(vm, runnable);
    }
  };

  class GetThreadStateById: public Builtin<GetThreadStateById> {
  public:
    GetThreadStateById(): Builtin("getThreadStateById") {}

    static void call(VM vm, In threadId, Out result) {
      size_t id = getArgument<size_t>(vm, threadId);
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);
      result = buildThreadStateRecord(vm, runnable);
    }
  };

  static inline
  UnstableNode getThreadStatusesList(VM vm, size_t from, size_t to) {
    return getThreadAggregatesList(vm, from, to,
      [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, buildThreadStatusRecord(vm, runnable));
      });
  }

  class GetThreadStatuses: public Builtin<GetThreadStatuses> {
  public:
    GetThreadStatuses(): Builtin("getThreadStatuses") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = getThreadStatusesList(vm, from, to);
    }
  };

  class GetAllThreadStatuses: public Builtin<GetAllThreadStatuses> {
  public:
    GetAllThreadStatuses(): Builtin("getAllThreadStatuses") {}

    static void call(VM vm, Out result) {
      result = getThreadStatusesList(vm, 0, SIZE_MAX);
    }
  };

  static inline
  UnstableNode getThreadStatesList(VM vm, size_t from, size_t to) {
    return getThreadAggregatesList(vm, from, to,
      [](VM vm, OzListBuilder& builder, Runnable* runnable) {
        builder.push_back(vm, buildThreadStateRecord(vm, runnable));
      });
  }

  class GetThreadStates: public Builtin<GetThreadStates> {
  public:
    GetThreadStates(): Builtin("getThreadStates") {}

    static void call(VM vm, In fromNode, In toNode, Out result) {
      size_t from = getArgument<size_t>(vm, fromNode);
      size_t to = getArgument<size_t>(vm, toNode);
      result = getThreadStatesList(vm, from, to);
    }
  };

  class GetAllThreadStates: public Builtin<GetAllThreadStates> {
  public:
    GetAllThreadStates(): Builtin("getAllThreadStates") {}

    static void call(VM vm, Out result) {
      result = getThreadStatesList(vm, 0, SIZE_MAX);
    }
  };

  class DisplayThread: public Builtin<DisplayThread> {
  public:
    DisplayThread(): Builtin("displayThread") {}

    static void call(VM vm, In threadNode) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      vm->getIntrospection().displayThread(vm, runnable);
    }
  };

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

  /* ========== Nodes stats ========== */

  static inline
  UnstableNode buildNodesPropertiesRecord(VM vm, NodesProperties& properties) {
    return buildRecord(vm,
      buildArity(vm,
        "nodesProperties",
        "nVariableNodes",
        "valueNodesCount",
        "structuralNodesCount",
        "tokenNodesCount",
        "stableNodesCount",
        "unstableNodesCount",
        "xNodesCount",
        "yNodesCount",
        "gNodesCount",
        "kNodesCount",
        "stackDepth",
        "nodesCount"
      ),
      build(vm, properties.variableNodesCount),
      build(vm, properties.valueNodesCount),
      build(vm, properties.structuralNodesCount),
      build(vm, properties.tokenNodesCount),
      build(vm, properties.stableNodesCount),
      build(vm, properties.unstableNodesCount),
      build(vm, properties.xNodesCount),
      build(vm, properties.yNodesCount),
      build(vm, properties.gNodesCount),
      build(vm, properties.kNodesCount),
      build(vm, properties.stackDepth),
      build(vm, properties.nodesCount)
    );
  }

  class GetNodesProperties: public Builtin<GetNodesProperties> {
  public:
    GetNodesProperties(): Builtin("getNodesProperties") {}

    static void call(VM vm, Out result) {
      Introspection::NodesProperties properties =
        vm->getIntrospection().getNodesProperties(vm);
      result = buildNodesPropertiesRecord(vm, properties);
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

  class GetValueNodesCount: public Builtin<GetValueNodesCount> {
  public:
    GetValueNodesCount(): Builtin("getValueNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getValueNodesCount(vm));
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

  class GetTokenNodesCount: public Builtin<GetTokenNodesCount> {
  public:
    GetTokenNodesCount(): Builtin("getTokenNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getTokenNodesCount(vm));
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

  class GetUnstableNodesCount: public Builtin<GetUnstableNodesCount> {
  public:
    GetUnstableNodesCount(): Builtin("getUnstableNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getUnstableNodesCount(vm));
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

  class GetYNodesCount: public Builtin<GetYNodesCount> {
  public:
    GetYNodesCount(): Builtin("getYNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getYNodesCount(vm));
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

  class GetKNodesCount: public Builtin<GetKNodesCount> {
  public:
    GetKNodesCount(): Builtin("getKNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getKNodesCount(vm));
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

  class GetNodesCount: public Builtin<GetNodesCount> {
  public:
    GetNodesCount(): Builtin("getNodesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getNodesCount(vm));
    }
  };

  /* ========== Variables stats ========== */

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
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTION_H