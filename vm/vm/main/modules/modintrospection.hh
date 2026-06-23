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
      Runnable* runnable = vm->getIntrospection().getThread(vm, id);

      if (runnable)
        result = ReifiedThread::build(vm, runnable);
      else
        result = build(vm, "none");
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
      } else if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
        if (matches(vm, aggregate, "variableNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).variableNodesCount);
        } else if (matches(vm, aggregate, "valueNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).valueNodesCount);
        } else if (matches(vm, aggregate, "structuralNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).structuralNodesCount);
        } else if (matches(vm, aggregate, "tokenNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).tokenNodesCount);
        } else if (matches(vm, aggregate, "stableNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).stableNodesCount);
        } else if (matches(vm, aggregate, "unstableNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).unstableNodesCount);
        } else if (matches(vm, aggregate, "xNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).xNodesCount);
        } else if (matches(vm, aggregate, "yNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).yNodesCount);
        } else if (matches(vm, aggregate, "gNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).gNodesCount);
        } else if (matches(vm, aggregate, "kNodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).kNodesCount);
        } else if (matches(vm, aggregate, "stackDepth")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).stackDepth);
        } else if (matches(vm, aggregate, "nodesCount")) {
          result = build(vm,
            vm->getIntrospection().getNodesCounts(vm).nodesCount);
        } else {
          result = build(vm, "none");
        }
      } else {
        result = build(vm, "none");
      }
    }
  };

  /* =================================== */

  static inline
  UnstableNode buildThreadIdentityRecord(VM vm, Runnable* runnable) {
    UnstableNode id = build(vm, runnable->getId());
    UnstableNode kindId = build(vm, runnable->getKindId());
    UnstableNode generationId = build(vm, runnable->getGenerationId());

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
        "identity",
        "generationId",
        "id",
        "kindId",
        "priority",
        "type"
      ),
      generationId,
      id,
      kindId,
      priority,
      type
    );
  }

  static inline
  UnstableNode buildThreadStatusRecord(VM vm, Runnable* runnable) {
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
        "status",
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

  static inline
  UnstableNode buildThreadNodesCountsRecord(VM vm, Runnable* runnable) {
    Introspection::NodesCounts properties =
      vm->getIntrospection().getNodesCounts(vm, runnable);
    return buildNodesCountsRecord(vm, properties);
  }

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
        "operationsCount",
        "resumesCount",
        "runsCount",
        "suspendsCount",
        "suspendsOnVarCount"
      ),
      build(vm, bindsCount),
      build(vm, operationsCount),
      build(vm, runnableStatistics.resumesCount),
      build(vm, runnableStatistics.runsCount),
      build(vm, runnableStatistics.suspendsCount),
      build(vm, runnableStatistics.suspendsOnVarCount)
    );
  }

  static inline
  UnstableNode buildThreadStateRecord(VM vm, Runnable* runnable) {
    std::string type;
    if (dynamic_cast<Thread*>(runnable)) type = "thread";
    else type = "runnable";

    return buildRecord(vm,
      buildArity(vm,
        "state",
        "nodes",
        "statistics",
        "status"
      ),
      buildThreadNodesCountsRecord(vm, runnable),
      buildThreadStatisticsRecord(vm, runnable),
      buildThreadStatusRecord(vm, runnable)
    );
  }

  class GetThreadIdentity: public Builtin<GetThreadIdentity> {
  public:
    GetThreadIdentity(): Builtin("getThreadIdentity") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadIdentityRecord(vm, runnable);
    }
  };

  class GetThreadStatus: public Builtin<GetThreadStatus> {
  public:
    GetThreadStatus(): Builtin("getThreadStatus") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
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

  class GetThreadNodesCounts: public Builtin<GetThreadNodesCounts> {
  public:
    GetThreadNodesCounts(): Builtin("getThreadNodesCounts") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = buildThreadNodesCountsRecord(vm, runnable);
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
  std::string getMemWordString(VM vm, MemWord& word) {
    nativeint value = word.get<nativeint>();
    std::stringstream stream;
    stream << std::hex <<
      std::uppercase << std::setfill('0') <<
      std::setw(2 * sizeof(nativeint)) << value;
    return stream.str();
  }

  enum NodesAggregate {
    AggregateNone,
    AggregateIdentity,
    AggregateStatus,
    AggregateValue,
    AggregateState
  };

  // static inline
  // NodesAggregate getNodeAggregate(VM vm, In aggregate) {
  //   using namespace patternmatching;

  //   if (matches(vm, aggregate, "identity"))
  //     return Identity;
  //   else if (matches(vm, aggregate, "status"))
  //     return Status;
  //   else if (matches(vm, aggregate, "value"))
  //     return Value;
  //   else if (matches(vm, aggregate, "all"))
  //     return All;
  //   else
  //     return None;
  // }

  static inline
  std::string nodeStructuralBehaviorToString(StructuralBehavior behavior) {
    switch (behavior) {
      case sbVariable: return "variable";
      case sbValue: return "value";
      case sbStructural: return "structural";
      case sbTokenEq: return "tokenEq";
      default: assert(false);
    }
  }

  static inline // TODO make a simplification of aggregates, is state not enough ?
  UnstableNode buildNodeRecord(VM vm, Node* node, NodesAggregate aggregate) {
    if (node == nullptr)
      return build(vm, "none");

    Introspection& introspection = vm->getIntrospection();
    Type type = introspection.getNodeType(vm, node);
    MemWord value = introspection.getNodeValue(vm, node);

    switch (aggregate) {
      case AggregateNone: return build(vm, "none");
      case AggregateIdentity: return
        buildRecord(vm,
          buildArity(vm,
            "node",
            "name",
            "uuid"
          ),
          build(vm, type->getName().c_str()),
          build(vm, type->getUUID())
        );
      case AggregateStatus: return
        buildRecord(vm,
          buildArity(vm,
            "node",
            "bindingPriority",
            "name",
            "structuralBehavior"
          ),
          build(vm, type->getBindingPriority()),
          build(vm, type->getName().c_str()),
          build(vm,
            nodeStructuralBehaviorToString(
              type->getStructuralBehavior()
            ).c_str()
          )
        );
      case AggregateValue: return
        buildRecord(vm,
          buildArity(vm,
            "node",
            "copyable",
            "feature",
            "name",
            "transient",
            "value"
          ),
          build(vm, type->isCopyable()),
          build(vm, type->isFeature()),
          build(vm, type->getName().c_str()),
          build(vm, type->isTransient()),
          build(vm, getMemWordString(vm, value).c_str())
        );
      case AggregateState: return
        buildRecord(vm,
          buildArity(vm,
            "node",
            "bindingPriority",
            "copyable",
            "feature",
            "name",
            "structuralBehavior",
            "transient",
            "uuid",
            "value"
          ),
          build(vm, type->getBindingPriority()),
          build(vm, type->isCopyable()),
          build(vm, type->isFeature()),
          build(vm, type->getName().c_str()),
          build(vm,
            nodeStructuralBehaviorToString(
              type->getStructuralBehavior()
            ).c_str()
          ),
          build(vm, type->isTransient()),
          build(vm, type->getUUID()),
          build(vm, getMemWordString(vm, value).c_str())
        );
      default: assert(false);
    }
  }

  typedef Introspection::NodesRegister NodesRegister;

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

  static inline
  std::string getAggregateName(NodesAggregate nodesAggregate) {
    switch (nodesAggregate) {
      case AggregateIdentity: return "Identity";
      case AggregateStatus: return "Status";
      case AggregateValue: return "Value";
      case AggregateState: return "State";
      default: assert(false);
    }
  }

  static inline
  NodesAggregate getAggregate(VM vm, In aggregateNode) {
    using namespace patternmatching;

    if (matches(vm, aggregateNode, "identity"))
      return AggregateIdentity;
    else if (matches(vm, aggregateNode, "status"))
      return AggregateStatus;
    else if (matches(vm, aggregateNode, "value"))
      return AggregateValue;
    else if (matches(vm, aggregateNode, "state"))
      return AggregateState;
    else assert(false);
  }

  // For future work allowing declaration like that :
  // template<NodesRegister nodesRegister, NodesAggregate nodesAggregate>
  // template<NodesRegister nodesRegister>
  // class GetThreadNodesCount: public Builtin<GetThreadNodesCount<nodesRegister>> {
  // private:
  //   static std::string getName() {
  //     return std::string("GetThread") + getRegisterName(nodesRegister)
  //       + std::string("NodesCount");
  //   }
  // public:
  //   GetThreadNodesCount():
  //     Builtin<GetThreadNodesCount<nodesRegister>>(getName()) {}

  //   static void call(VM vm, In threadNode, Out result) {
  //     Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  //     Introspection& introspection = vm->getIntrospection();

  //     size_t count;
  //     switch (nodesRegister) {
  //       case NodesRegister::xRegister:
  //         count = introspection.getXNodesCount(vm, runnable); break;
  //       case NodesRegister::yRegister:
  //         count = introspection.getYNodesCount(vm, runnable); break;
  //       case NodesRegister::gRegister:
  //         count = introspection.getGNodesCount(vm, runnable); break;
  //       case NodesRegister::kRegister:
  //         count = introspection.getKNodesCount(vm, runnable); break;
  //       default: assert(false);
  //     }

  //     result = build(vm, count);
  //   }
  // };

  static inline
  UnstableNode getThreadNodesCount(VM vm, In threadNode, NodesRegister nodesRegister) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    Introspection& introspection = vm->getIntrospection();

    size_t count;
    switch (nodesRegister) {
      case NodesRegister::xRegister:
        count = introspection.getXNodesCount(vm, runnable); break;
      case NodesRegister::yRegister:
        count = introspection.getYNodesCount(vm, runnable); break;
      case NodesRegister::gRegister:
        count = introspection.getGNodesCount(vm, runnable); break;
      case NodesRegister::kRegister:
        count = introspection.getKNodesCount(vm, runnable); break;
      case NodesRegister::anyRegister:
        count = introspection.getNodesCount(vm, runnable);
      default: assert(false);
    }

    return build(vm, count);
  }

  class GetThreadXNodesCount: public Builtin<GetThreadXNodesCount> {
  public:
    GetThreadXNodesCount(): Builtin("getThreadXNodesCount") {}

    static void call(VM vm, In threadNode, Out result) {
      result = getThreadNodesCount(vm, threadNode, NodesRegister::xRegister);
    }
  };

  class GetThreadYNodesCount: public Builtin<GetThreadYNodesCount> {
  public:
    GetThreadYNodesCount(): Builtin("getThreadYNodesCount") {}

    static void call(VM vm, In threadNode, Out result) {
      result = getThreadNodesCount(vm, threadNode, NodesRegister::yRegister);
    }
  };

  class GetThreadGNodesCount: public Builtin<GetThreadGNodesCount> {
  public:
    GetThreadGNodesCount(): Builtin("getThreadGNodesCount") {}

    static void call(VM vm, In threadNode, Out result) {
      result = getThreadNodesCount(vm, threadNode, NodesRegister::gRegister);
    }
  };

  class GetThreadKNodesCount: public Builtin<GetThreadKNodesCount> {
  public:
    GetThreadKNodesCount(): Builtin("getThreadKNodesCount") {}

    static void call(VM vm, In threadNode, Out result) {
      result = getThreadNodesCount(vm, threadNode, NodesRegister::kRegister);
    }
  };

  class GetThreadNodesCount: public Builtin<GetThreadNodesCount> {
  public:
    GetThreadNodesCount(): Builtin("getThreadNodesCount") {}

    static void call(VM vm, In threadNode, Out result) {
      result = getThreadNodesCount(vm, threadNode, NodesRegister::anyRegister);
    }
  };

  class GetThreadStackDepth: public Builtin<GetThreadStackDepth> {
  public:
    GetThreadStackDepth(): Builtin("getThreadStackDepth") {}

    static void call(VM vm, In threadNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = build(vm,
        vm->getIntrospection().getStackDepth(vm, runnable));
    }
  };

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
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::yRegister);
    }
  };

  class GetThreadGNodesRegisterSize: public Builtin<GetThreadGNodesRegisterSize> {
  public:
    GetThreadGNodesRegisterSize(): Builtin("getThreadGNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::gRegister);
    }
  };

  class GetThreadKNodesRegisterSize: public Builtin<GetThreadKNodesRegisterSize> {
  public:
    GetThreadKNodesRegisterSize(): Builtin("getThreadKNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        NodesRegister::kRegister);
    }
  };

  static inline
  UnstableNode getThreadXNodeAggregate(VM vm, In threadNode, In indexNode, In aggregateNode) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t index = getArgument<size_t>(vm, indexNode);
    NodesAggregate aggregate = getAggregate(vm, aggregateNode);
    Introspection& introspection = vm->getIntrospection();

    assert(index < introspection.getXNodesRegisterSize(vm, runnable));
    Node* node = introspection.getXNode(vm, runnable, index);
    return buildNodeRecord(vm, node, aggregate);
  }

  static inline
  UnstableNode getThreadNodeAggregate(VM vm, In threadNode, In depthNode, In indexNode,
    In aggregateNode, NodesRegister nodesRegister) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t depth = getArgument<size_t>(vm, depthNode);
    size_t index = getArgument<size_t>(vm, indexNode);
    NodesAggregate aggregate = getAggregate(vm, aggregateNode);
    Introspection& introspection = vm->getIntrospection();

    assert(depth < introspection.getStackDepth(vm, runnable));
    Node* node;
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

    return buildNodeRecord(vm, node, aggregate);
  }

  class GetThreadXNodeAggregate: public Builtin<GetThreadXNodeAggregate> {
  public:
    GetThreadXNodeAggregate(): Builtin("getThreadXNodeAggregate") {}

    static void call(VM vm, In threadNode, In indexNode, In aggregateNode, Out result) {
      result = getThreadXNodeAggregate(vm, threadNode, indexNode, aggregateNode);
    }
  };

  class GetThreadYNodeAggregate: public Builtin<GetThreadYNodeAggregate> {
  public:
    GetThreadYNodeAggregate(): Builtin("getThreadYNodeAggregate") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregate(vm, threadNode, depthNode, indexNode, aggregateNode,
        NodesRegister::yRegister);
    }
  };

  class GetThreadGNodeAggregate: public Builtin<GetThreadGNodeAggregate> {
  public:
    GetThreadGNodeAggregate(): Builtin("getThreadGNodeAggregate") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregate(vm, threadNode, depthNode, indexNode, aggregateNode,
        NodesRegister::gRegister);
    }
  };

  class GetThreadKNodeAggregate: public Builtin<GetThreadKNodeAggregate> {
  public:
    GetThreadKNodeAggregate(): Builtin("getThreadKNodeAggregate") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregate(vm, threadNode, depthNode, indexNode, aggregateNode,
        NodesRegister::kRegister);
    }
  };

  static inline
  UnstableNode getThreadXNodeAggregates(VM vm, In threadNode, In fromNode, In toNode, In aggregateNode) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t from = getArgument<size_t>(vm, fromNode);
    size_t to = getArgument<size_t>(vm, toNode);
    NodesAggregate aggregate = getAggregate(vm, aggregateNode);
    Introspection& introspection = vm->getIntrospection();

    assert(from < to);
    assert(to < introspection.getXNodesRegisterSize(vm, runnable));

    OzListBuilder builder(vm);
    introspection.doForEachXNode(vm, runnable, from, to, [vm, &builder, aggregate](Node& node) {
      builder.push_back(vm, buildNodeRecord(vm, &node, aggregate));
    });
    return builder.get(vm);
  }

  static inline
  UnstableNode getThreadNodeAggregates(VM vm, In threadNode, In depthNode, In fromNode, In toNode, In aggregateNode,
    NodesRegister nodesRegister) {
    Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
    size_t depth = getArgument<size_t>(vm, depthNode);
    size_t from = getArgument<size_t>(vm, fromNode);
    size_t to = getArgument<size_t>(vm, toNode);
    NodesAggregate aggregate = getAggregate(vm, aggregateNode);
    Introspection& introspection = vm->getIntrospection();

    assert(depth < introspection.getStackDepth(vm, runnable));
    assert(from < to);

    OzListBuilder builder(vm);

    switch (nodesRegister) {
      case NodesRegister::yRegister: {
        assert(to < introspection.getYNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachYNode(vm, runnable, depth, from, to, [vm, &builder, aggregate](Node& node) {
          builder.push_back(vm, buildNodeRecord(vm, &node, aggregate));
        });
        break;
      } case NodesRegister::gRegister: {
        assert(to < introspection.getGNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachGNode(vm, runnable, depth, from, to, [vm, &builder, aggregate](Node& node) {
          builder.push_back(vm, buildNodeRecord(vm, &node, aggregate));
        });
        break;
      } case NodesRegister::kRegister: {
        assert(to < introspection.getKNodesRegisterSize(vm, runnable, depth));
        introspection.doForEachKNode(vm, runnable, depth, from, to, [vm, &builder, aggregate](Node& node) {
          builder.push_back(vm, buildNodeRecord(vm, &node, aggregate));
        });
        break;
      } default: assert(false);
    }

    return builder.get(vm);
  }

  class GetThreadXNodeAggregates: public Builtin<GetThreadXNodeAggregates> {
  public:
    GetThreadXNodeAggregates(): Builtin("getThreadXNodeAggregates") {}

    static void call(VM vm, In threadNode, In fromNode, In toNode, In aggregateNode, Out result) {
      result = getThreadXNodeAggregates(vm, threadNode, fromNode, toNode, aggregateNode);
    }
  };

  class GetThreadYNodeAggregates: public Builtin<GetThreadYNodeAggregates> {
  public:
    GetThreadYNodeAggregates(): Builtin("getThreadYNodeAggregates") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregates(vm, threadNode, depthNode, fromNode, toNode, aggregateNode,
        NodesRegister::yRegister);
    }
  };

  class GetThreadGNodeAggregates: public Builtin<GetThreadGNodeAggregates> {
  public:
    GetThreadGNodeAggregates(): Builtin("getThreadGNodeAggregates") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregates(vm, threadNode, depthNode, fromNode, toNode, aggregateNode,
        NodesRegister::gRegister);
    }
  };

  class GetThreadKNodeAggregates: public Builtin<GetThreadKNodeAggregates> {
  public:
    GetThreadKNodeAggregates(): Builtin("getThreadKNodeAggregates") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, In aggregateNode, Out result) {
      result = getThreadNodeAggregates(vm, threadNode, depthNode, fromNode, toNode, aggregateNode,
        NodesRegister::kRegister);
    }
  };

  static inline
  UnstableNode buildNodesCountsRecord(VM vm, NodesCounts& properties) {
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