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

#include "../introspection-decl.hh"
#include "mozartcore-module.hh"

#include <unordered_set>
#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModIntrospection: public Module {
public:
  ModIntrospection(): Module("Introspection") {}

  /* ========== Virtual Machine stats ========== */  

  class GetSchedulesCount: public Builtin<GetSchedulesCount> {
  public:
    GetSchedulesCount(): Builtin("getSchedulesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getSchedulesCount(vm));
    }
  };

  class GetOperationsCount: public Builtin<GetOperationsCount> {
  public:
    GetOperationsCount(): Builtin("getOperationsCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getOperationsCount(vm));
    }
  };

  class GetSystemSchedulesCount: public Builtin<GetSystemSchedulesCount> {
  public:
    GetSystemSchedulesCount(): Builtin("getSystemSchedulesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getSystemSchedulesCount(vm));
    }
  };

  class GetSystemOperationsCount: public Builtin<GetSystemOperationsCount> {
  public:
    GetSystemOperationsCount(): Builtin("getSystemOperationsCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getSystemOperationsCount(vm));
    }
  };

  class GetGCSchedulesCount: public Builtin<GetGCSchedulesCount> {
  public:
    GetGCSchedulesCount(): Builtin("getGCSchedulesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getGCSchedulesCount(vm));
    }
  };

  class GetNextScheduledThread: public Builtin<GetNextScheduledThread> {
  public:
    GetNextScheduledThread(): Builtin("getNextScheduledThread") {}

    static void call(VM vm, In boolean, Out result);
  };

  class GetNextOperation: public Builtin<GetNextOperation> {
  public:
    GetNextOperation(): Builtin("getNextOperation") {}

    static void call(VM vm, In includeSystemThreadsNode, Out result);
  };

  /* ========== Garbage Collector ========== */

  class GetGarbageCollectedThreads: public Builtin<GetGarbageCollectedThreads> {
  public:
    GetGarbageCollectedThreads(): Builtin("getGarbageCollectedThreads") {}

    static void call(VM vm, Out result);
  };

  class GetGarbageCollectorTodos: public Builtin<GetGarbageCollectorTodos> {
  public:
    GetGarbageCollectorTodos(): Builtin("getGarbageCollectorTodos") {}

    static void call(VM vm, Out result);
  };

  /* ========== Threads ========== */

  // Thread aggregates

  static inline
  UnstableNode buildThreadAggregatesList(VM vm, size_t from, size_t to,
    std::function<void(VM vm, OzListBuilder& builder, Runnable* runnable)> lambda);

  static inline
  UnstableNode buildThreadRecordsList(VM vm, size_t from, size_t to,
      std::function<UnstableNode(VM vm, Runnable* runnable)> recordBuilder);

  // Thread accessors

  class GetThreadIds: public Builtin<GetThreadIds> {
  public:
    GetThreadIds(): Builtin("getThreadIds") {}

    static void call(VM vm, In fromNode, In toNode, Out result);
  };

  class GetThread: public Builtin<GetThread> {
  public:
    GetThread(): Builtin("getThread") {}

    static void call(VM vm, In threadId, Out result);
  };

  class GetThreads: public Builtin<GetThreads> {
  public:
    GetThreads(): Builtin("getThreads") {}

    static void call(VM vm, In fromNode, In toNode, Out result);
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

  // Thread state aggregate

  class GetThreadState: public Builtin<GetThreadState> {
  public:
    GetThreadState(): Builtin("getThreadState") {}

    static void call(VM vm, In threadNode, Out result);
  };

  // Thread statistics aggregate

  class GetThreadStatistics: public Builtin<GetThreadStatistics> {
  public:
    GetThreadStatistics(): Builtin("getThreadStatistics") {}

    static void call(VM vm, In threadNode, Out result);
  };

  // Thread nodes aggregate

  class GetThreadNodesCounts: public Builtin<GetThreadNodesCounts> {
  public:
    GetThreadNodesCounts(): Builtin("getThreadNodesCounts") {}

    static void call(VM vm, In threadNode, Out result);
  };

  /* ========== Nodes ========== */

  // Nodes counters
  
  class GetNodesCounts: public Builtin<GetNodesCounts> {
  public:
    GetNodesCounts(): Builtin("getNodesCounts") {}

    static void call(VM vm, Out result);
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

  static
  UnstableNode getThreadNodesRegisterSize(VM vm, In threadNode, In depthNode,
    Introspection::NodesRegister nodesRegister);

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
        Introspection::NodesRegister::yRegister);
    }
  };

  class GetThreadGNodesRegisterSize: public Builtin<GetThreadGNodesRegisterSize> {
  public:
    GetThreadGNodesRegisterSize(): Builtin("getThreadGNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        Introspection::NodesRegister::gRegister);
    }
  };

  class GetThreadKNodesRegisterSize: public Builtin<GetThreadKNodesRegisterSize> {
  public:
    GetThreadKNodesRegisterSize(): Builtin("getThreadKNodesRegisterSize") {}

    static void call(VM vm, In threadNode, In depthNode, Out result) {
      result = getThreadNodesRegisterSize(vm, threadNode, depthNode,
        Introspection::NodesRegister::kRegister);
    }
  };

  // Nodes getters

  static
  UnstableNode getThreadXNode(VM vm, In threadNode, In indexNode);

  static
  UnstableNode getThreadNode(VM vm, In threadNode, In depthNode,
    In indexNode, Introspection::NodesRegister nodesRegister);

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
      result = getThreadNode(vm, threadNode, depthNode, indexNode,
        Introspection::NodesRegister::yRegister);
    }
  };

  class GetThreadGNode: public Builtin<GetThreadGNode> {
  public:
    GetThreadGNode(): Builtin("getThreadGNode") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, Out result) {
      result = getThreadNode(vm, threadNode, depthNode, indexNode,
        Introspection::NodesRegister::gRegister);
    }
  };

  class GetThreadKNode: public Builtin<GetThreadKNode> {
  public:
    GetThreadKNode(): Builtin("getThreadKNode") {}

    static void call(VM vm, In threadNode, In depthNode, In indexNode, Out result) {
      result = getThreadNode(vm, threadNode, depthNode, indexNode,
        Introspection::NodesRegister::kRegister);
    }
  };

  // Nodes lists getters

  static
  UnstableNode getThreadXNodes(VM vm, In threadNode, In fromNode, In toNode);

  static
  UnstableNode getThreadNodes(VM vm, In threadNode, In depthNode, In fromNode, In toNode,
    Introspection::NodesRegister nodesRegister);

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
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, Introspection::NodesRegister::yRegister);
    }
  };

  class GetThreadGNodes: public Builtin<GetThreadGNodes> {
  public:
    GetThreadGNodes(): Builtin("getThreadGNodes") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, Out result) {
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, Introspection::NodesRegister::gRegister);
    }
  };

  class GetThreadKNodes: public Builtin<GetThreadKNodes> {
  public:
    GetThreadKNodes(): Builtin("getThreadKNodes") {}

    static void call(VM vm, In threadNode, In depthNode, In fromNode, In toNode, Out result) {
      result = getThreadNodes(vm, threadNode, depthNode, fromNode, toNode, Introspection::NodesRegister::kRegister);
    }
  };

  /* ========== Variables stats ========== */

  // Variables counters

  class GetBoundVariablesCount: public Builtin<GetBoundVariablesCount> {
  public:
    GetBoundVariablesCount(): Builtin("getBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      Introspection introspection;
      result = build(vm,
        vm->getIntrospection().getBoundVariablesCount(vm));
    }
  };

  class GetUnBoundVariablesCount: public Builtin<GetUnBoundVariablesCount> {
  public:
    GetUnBoundVariablesCount(): Builtin("getUnBoundVariablesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getUnBoundVariablesCount(vm));
    }
  };

  class GetVariablesCount: public Builtin<GetVariablesCount> {
  public:
    GetVariablesCount(): Builtin("getVariablesCount") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getIntrospection().getVariablesCount(vm));
    }
  };

  // Variables getters

  class GetVariable: public Builtin<GetVariable> {
  public:
    GetVariable(): Builtin("getVariable") {}

    static void call(VM vm, In variableIdNode, Out result);
  };

  class GetThreadVariables: public Builtin<GetThreadVariables> {
  public:
    GetThreadVariables(): Builtin("getThreadVariables") {}

    static void call(VM vm, In runnableNode, Out result);
  };

  class GetVariables: public Builtin<GetVariables> {
  public:
    GetVariables(): Builtin("getVariables") {}

    static void call(VM vm, Out result);
  };

  /* ========== Reachability graph ========== */

  static
  UnstableNode buildReachabilityMapRecord(VM vm, Introspection::IdToIdsMap& map);

  static
  UnstableNode buildReachabilityGraphRecord(VM vm, Introspection::ReachabilityGraph& graph);

  class GetReachabilityGraph: public Builtin<GetReachabilityGraph> {
  public:
    GetReachabilityGraph(): Builtin("getReachabilityGraph") {}

    static void call(VM vm, Out result);
  };

  /* ========== Structures list ========== */

  static
  UnstableNode buildListNodeRecord(VM vm, Introspection::OwnedRichNode& ownedNode);

  static
  UnstableNode buildListNodesListRecord(VM vm, RichNode idsList, Introspection::IdToOwnedRichNodeMap& map);

  class GetThreadLists: public Builtin<GetThreadLists> {
  public:
    GetThreadLists(): Builtin("getThreadLists") {}

    static void call(VM vm, In runnableNode, In idsList, Out result);
  };

  class GetLists: public Builtin<GetLists> {
  public:
    GetLists(): Builtin("getLists") {}

    static void call(VM vm, In idsList, Out result);
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTION_H
