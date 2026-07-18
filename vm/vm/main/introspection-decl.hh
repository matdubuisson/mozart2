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
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MOZART_INTROSPECTION_DECL_H
#define MOZART_INTROSPECTION_DECL_H

#include "core-forward-decl.hh"
#include "opcodes.hh"
#include "runnable-decl.hh"

namespace mozart {

// Assumption : there is only one VM

class Introspection {
public:
  using RunnableLambda = std::function<void(VM, Runnable*)>;
  using NodeLambda = std::function<void(VM, RichNode)>;
  using RunnableBoolLambda = std::function<bool(VM, Runnable*)>;
  using NodeBoolLambda = std::function<bool(VM, RichNode)>;
  using RunnableAndNodeLambda = std::function<void(VM, Runnable*, RichNode)>;

  inline static const
  RunnableBoolLambda allRunnables = [](VM, Runnable*) { return true; };
  inline static const
  NodeBoolLambda allNodes = [](VM, RichNode) { return true; };

public:
  Introspection() {}

public:
  /* ========== VM state ========== */
  size_t getSchedulesCounter(VM vm);

  size_t getOperationsCounter(VM vm);

  Runnable* getNextScheduledThread(VM vm, bool includeSystemThreads);

  enum ArgumentType {
    I = 0,
    X,
    Y,
    G,
    K,
  };

  struct OperationArgument {
  private:
    inline
    std::string toRepr(VM vm, RichNode value);

  public:
    OperationArgument(VM vm, ArgumentType type, size_t index, ByteCode integer) :
      type(type), index(index), image(std::to_string(integer)) {}

    OperationArgument(VM vm, ArgumentType type, size_t index, StableNode& node) :
      type(type), index(index), image(toRepr(vm, RichNode(node))) {}

    OperationArgument(VM vm, ArgumentType type, size_t index, UnstableNode& node) :
      type(type), index(index), image(toRepr(vm, RichNode(node))) {}

    ArgumentType type;
    size_t index;
    std::string image;
  };

  struct Operation {
    Operation(OpCode opCode) :
      opCode(opCode) {}

    OpCode opCode;
    std::string name;
    
    std::vector<OperationArgument> arguments;
  };

  Operation getNextExecutedOperation(VM vm, bool includeSystemThreads);  
public:
  /* ========== Threads stats ========== */

  struct ThreadsCounts {
    size_t activeThreadsCount = 0;
    size_t passiveThreadsCount = 0;
    size_t threadsCount = 0;
  };

public:
  /* ========== Threads getters ========== */
  Runnable* getThread(VM vm, size_t id);

  RunnableList& getThreads(VM vm);

public:
  /* ========== Threads executers ========== */

  void doForEachThread(VM vm, RunnableBoolLambda valid, RunnableLambda parse);

public:
  /* ========== Threads counters ========== */

  ThreadsCounts getThreadsCounts(VM vm);

  inline
  size_t getActiveThreadsCount(VM vm) {
    return getThreadsCounts(vm).activeThreadsCount;
  }

  inline
  size_t getPassiveThreadsCount(VM vm) {
    return getThreadsCounts(vm).passiveThreadsCount;
  }

  inline
  size_t getThreadsCount(VM vm) {
    return getThreadsCounts(vm).threadsCount;
  }

public:
  /* ========== Registers stats ========== */
  enum NodesRegister {
    xRegister,
    yRegister,
    gRegister,
    kRegister,
    anyRegister
  };

private:
  /* ========== Registers stats ========== */
  size_t getNodesRegisterSize(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth);
public:
  /* ========== Registers stats ========== */
  inline
  size_t getXNodesRegisterSize(VM vm, Runnable* runnable) {
    return getNodesRegisterSize(vm, runnable, xRegister, 0);
  }

  inline
  size_t getYNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, yRegister, depth);
  }

  inline
  size_t getGNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, gRegister, depth);
  }

  inline
  size_t getKNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, kRegister, depth);
  }

public:
  /* ========== Nodes stats ========== */
  struct NodesCounts {
    size_t variableNodesCount = 0;
    size_t valueNodesCount = 0;
    size_t structuralNodesCount = 0;
    size_t tokenNodesCount = 0;

    size_t stableNodesCount = 0;
    size_t unstableNodesCount = 0;

    size_t xNodesCount = 0;
    size_t yNodesCount = 0;
    size_t gNodesCount = 0;
    size_t kNodesCount = 0;

    size_t stackDepth = 0; // Is irrelevant for an aggregate of several threads

    size_t nodesCount = 0;
  };

private:
  /* ========== Nodes properties ========== */
  Type getNodeType(VM vm, Node* node);

  MemWord getNodeValue(VM vm, Node* node);

public:
  bool isVariableNode(VM vm, RichNode node);

  bool isStructuralNode(VM vm, RichNode node);

  bool isValueNode(VM vm, RichNode node);

  bool isTokenNode(VM vm, RichNode node);

private:
  /* ========== Nodes counters ========== */
  void getNodesCounts(VM vm, Runnable* runnable, NodesCounts& counts);

public:
  /* ========== Nodes counters ========== */
  inline
  NodesCounts getNodesCounts(VM vm, Runnable* runnable) {
    NodesCounts counts;
    getNodesCounts(vm, runnable, counts);
    return counts;
  }

  inline
  NodesCounts getNodesCounts(VM vm) {
    NodesCounts counts;
    doForEachThread(vm, allRunnables, [this, &counts](VM vm, Runnable* runnable) {
      this->getNodesCounts(vm, runnable, counts);
    });
    return counts;
  }

  inline
  size_t getVariableNodesCount(VM vm) {
    return getNodesCounts(vm).variableNodesCount;
  }

  inline
  size_t getVariableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).variableNodesCount;
  }

  inline
  size_t getValueNodesCount(VM vm) {
    return getNodesCounts(vm).valueNodesCount;
  }

  inline
  size_t getValueNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).valueNodesCount;
  }

  inline
  size_t getStructuralNodesCount(VM vm) {
    return getNodesCounts(vm).structuralNodesCount;
  }

  inline
  size_t getStructuralNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).structuralNodesCount;
  }

  inline
  size_t getTokenNodesCount(VM vm) {
    return getNodesCounts(vm).tokenNodesCount;
  }

  inline
  size_t getTokenNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).tokenNodesCount;
  }

  inline
  size_t getStableNodesCount(VM vm) {
    return getNodesCounts(vm).stableNodesCount;
  }

  inline
  size_t getStableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).stableNodesCount;
  }

  inline
  size_t getUnstableNodesCount(VM vm) {
    return getNodesCounts(vm).unstableNodesCount;
  }

  inline
  size_t getUnstableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).unstableNodesCount;
  }

  inline
  size_t getXNodesCount(VM vm) {
    return getNodesCounts(vm).xNodesCount;
  }

  inline
  size_t getXNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).xNodesCount;
  }

  inline
  size_t getYNodesCount(VM vm) {
    return getNodesCounts(vm).yNodesCount;
  }

  inline
  size_t getYNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).yNodesCount;
  }

  inline
  size_t getGNodesCount(VM vm) {
    return getNodesCounts(vm).gNodesCount;
  }

  inline
  size_t getGNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).gNodesCount;
  }

  inline
  size_t getKNodesCount(VM vm) {
    return getNodesCounts(vm).kNodesCount;
  }

  inline
  size_t getKNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).kNodesCount;
  }

  inline
  size_t getStackDepth(VM vm) {
    return getNodesCounts(vm).stackDepth;
  }

  inline
  size_t getStackDepth(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).stackDepth;
  }

  inline
  size_t getNodesCount(VM vm) {
    return getNodesCounts(vm).nodesCount;
  }

  inline
  size_t getNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).nodesCount;
  }

private:
  /* ========== Node getters ========== */
  RichNode getNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t index);
public:
  /* ========== Node getters ========== */
  inline
  RichNode getXNode(VM vm, Runnable* runnable, size_t index) {
    return getNode(vm, runnable, xRegister, 0, index);
  }

  inline
  RichNode getYNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, yRegister, depth, index);
  }

  inline
  RichNode getGNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, gRegister, depth, index);
  }

  inline
  RichNode getKNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, kRegister, depth, index);
  }

private:
  /* ========== Nodes executers ========== */
  void doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t from, size_t to, NodeBoolLambda validNode, RunnableAndNodeLambda parse);

public:
  /* ========== Nodes executers ========== */
  inline
  void doForEachXNode(VM vm, Runnable* runnable, size_t from, size_t to,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, xRegister, 0, from, to,
      validNode, parse);
  }

  inline
  void doForEachYNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, yRegister, depth, from, to,
      validNode, parse);
  }

  inline
  void doForEachGNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, gRegister, depth, from, to,
      validNode, parse);
  }

  inline
  void doForEachKNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, kRegister, depth, from, to,
      validNode, parse);
  }

  inline
  void doForEachNode(VM vm, Runnable* runnable,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse,
    size_t from = 0, size_t to = SIZE_MAX) {
    assert(runnable != nullptr);
    
    doForEachXNode(vm, runnable, 0,
      getXNodesRegisterSize(vm, runnable), validNode, parse);

    size_t depth = getStackDepth(vm, runnable);
    for (size_t i = 0; i < depth; i++) {
      doForEachYNode(vm, runnable, i, 0,
        getYNodesRegisterSize(vm, runnable, i), validNode, parse);
      doForEachGNode(vm, runnable, i, 0,
        getGNodesRegisterSize(vm, runnable, i), validNode, parse);
      doForEachKNode(vm, runnable, i, 0,
        getKNodesRegisterSize(vm, runnable, i), validNode, parse);
    }
  }

  inline
  void doForEachNode(VM vm, RunnableBoolLambda validRunnable,
    NodeBoolLambda validNode, RunnableAndNodeLambda parse,
    size_t from = 0, size_t to = SIZE_MAX) {
    doForEachThread(vm, validRunnable, [this, validNode, parse](VM vm, Runnable* runnable) {
      this->doForEachNode(vm, runnable, validNode, parse);
    });
  }

public:
 /* ========== Variables stats ========== */
 struct VariablesCounts {
   size_t boundVariablesCount = 0;
   size_t unBoundVariablesCount = 0;
   size_t neededVariablesCount = 0;
   size_t waitedVariablesCount = 0;
   size_t variablesCount = 0;
 };

private:
  /* ========== Variables properties ========== */
  bool isBoundVariable(VM vm, RichNode node);

  bool isNeededVariable(VM vm, RichNode node);

  bool isWaitedVariable(VM vm, RichNode node);

private:
  /* ========== Variables counters ========== */
  void getVariablesCounts(VM vm, Runnable* runnable, VariablesCounts& counts);

public:
  /* ========== Variables counters ========== */
  inline
  VariablesCounts getVariablesCounts(VM vm, Runnable* runnable) {
    VariablesCounts counts;
    getVariablesCounts(vm, runnable, counts);
    return counts;
  }

  inline
  VariablesCounts getVariablesCounts(VM vm) {
    VariablesCounts counts;
    doForEachThread(vm, allRunnables, [this, &counts](VM vm, Runnable* runnable) {
      this->getVariablesCounts(vm, runnable, counts);
    });
    return counts;
  }

  inline
  size_t getBoundVariablesCount(VM vm) {
    return getVariablesCounts(vm).boundVariablesCount;
  }

  inline
  size_t getUnBoundVariablesCount(VM vm) {
    return getVariablesCounts(vm).unBoundVariablesCount;
  }

  inline
  size_t getNeededVariablesCount(VM vm) {
    return getVariablesCounts(vm).neededVariablesCount;
  }

  inline
  size_t getWaitedVariablesCount(VM vm) {
    return getVariablesCounts(vm).waitedVariablesCount;
  }

  inline
  size_t getVariablesCount(VM vm) {
    return getVariablesCounts(vm).variablesCount;
  }

private:
  /* ========== Variables executers ========== */
  void doForEachVariable(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t from, size_t to, RunnableAndNodeLambda parse);
public:
  /* ========== Variables executers ========== */
  inline
  void doForEachVariable(VM vm, Runnable* runnable, RunnableAndNodeLambda parse) {
    size_t depth = getStackDepth(vm, runnable);
    doForEachVariable(vm, runnable, xRegister, 0, 0,
      getXNodesRegisterSize(vm, runnable), parse);

    for (size_t i = 0; i < depth; i++) {
      doForEachVariable(vm, runnable, yRegister, i, 0,
        getYNodesRegisterSize(vm, runnable, i), parse);
      doForEachVariable(vm, runnable, gRegister, i, 0,
        getGNodesRegisterSize(vm, runnable, i), parse);
      doForEachVariable(vm, runnable, kRegister, i, 0,
        getKNodesRegisterSize(vm, runnable, i), parse);
    }
  }

  inline
  void doForEachVariable(VM vm, RunnableAndNodeLambda parse) {
    doForEachThread(vm, allRunnables, [this, parse](VM vm, Runnable* runnable) {
      this->doForEachVariable(vm, runnable, parse);
    });
  }

public:
  /* ========== Variables candidates ========== */
  using CandidatesList = std::vector<size_t>;

  struct VariableCandidates {
    VariableCandidates(RichNode node) : node(node) {}

    RichNode node;
    CandidatesList candidates;

    bool isNull() {
      return node.isNullNode();
    }

    void setNode(RichNode n) {
      node = n;
    }

    bool has(size_t candidateThreadId);

    void add(size_t candidateThreadId);
  };

  using VariableCandidatesMap = std::unordered_map<size_t, VariableCandidates>;

public:
  /* ========== Variables candidates extractors ========== */
  VariableCandidates getVariable(VM vm, size_t variableId);

  VariableCandidatesMap getVariableCandidatesMap(VM vm, Runnable* runnable);

  VariableCandidatesMap getVariableCandidatesMap(VM vm);

public:
  /* ========== Structures stats ========== */

  struct StructuresCounts {
    size_t consCount = 0;
    size_t tuplesCount = 0;
    size_t aritiesCount = 0;
    size_t recordsCount = 0;
  };

public:
  /* ========== Structures counters ========== */
  StructuresCounts getStructuresCounts(VM vm);

  StructuresCounts getStructuresCounts(VM vm, Runnable* runnable);

  inline
  size_t getConsCount(VM vm) {
    return getStructuresCounts(vm).consCount;
  }

  inline
  size_t getConsCount(VM vm, Runnable* runnable) {
    return getStructuresCounts(vm, runnable).consCount;
  }

  inline
  size_t getTuplesCount(VM vm) {
    return getStructuresCounts(vm).tuplesCount;
  }

  inline
  size_t getTuplesCount(VM vm, Runnable* runnable) {
    return getStructuresCounts(vm, runnable).tuplesCount;
  }

  inline
  size_t getAritiesCount(VM vm) {
    return getStructuresCounts(vm).aritiesCount;
  }

  inline
  size_t getAritiesCount(VM vm, Runnable* runnable) {
    return getStructuresCounts(vm, runnable).aritiesCount;
  }

  inline
  size_t getRecordsCount(VM vm) {
    return getStructuresCounts(vm).recordsCount;
  }

  inline
  size_t getRecordsCount(VM vm, Runnable* runnable) {
    return getStructuresCounts(vm, runnable).recordsCount;
  }

public:
  /* ========== Structures list ========== */
  using NodesList = std::vector<RichNode>;

private:
  static inline
  RunnableAndNodeLambda getAddConsLambda(VM vm, NodesList& list);

public:
  /* ========== Structures list ========== */
  // RichNode getList(VM vm, size_t id);

  NodesList getLists(VM vm, Runnable* runnable) {
    NodesList list;
    std::cout << "A" << std::endl;
    doForEachNode(vm, runnable,
      allNodes,
      getAddConsLambda(vm, list)
    );
    std::cout << "B" << std::endl;
    return list;
  }

  NodesList getLists(VM vm) {
    NodesList list;
    std::cout << "A" << std::endl;
    doForEachNode(vm,
      allRunnables,
      allNodes,
      getAddConsLambda(vm, list)
    );
    std::cout << "B" << std::endl;
    return list;
  }
};

}

#endif // MOZART_INTROSPECTION_DECL_H

