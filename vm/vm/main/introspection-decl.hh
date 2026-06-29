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
  /* ========== Threads state ========== */
  RunnableList& getThreads(VM vm);

  Runnable* getThread(VM vm, size_t id);

  /* ========== Threads stats ========== */

  size_t getActiveThreadsCount(VM vm);

  size_t getPassiveThreadsCount(VM vm);

  size_t getThreadsCount(VM vm);

  /* ========== Nodes stats ========== */
public:
  inline
  Type getNodeType(VM vm, Node* node);

  inline
  MemWord getNodeValue(VM vm, Node* node);

public:
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
  void getNodesCounts(VM vm, Runnable* runnable, NodesCounts& properties);

public:
  NodesCounts getNodesCounts(VM vm);

  inline
  NodesCounts getNodesCounts(VM vm, Runnable *runnable) {
    NodesCounts properties;
    getNodesCounts(vm, runnable, properties);
    return properties;
  }

public:
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

public:
  enum NodesRegister {
    xRegister,
    yRegister,
    gRegister,
    kRegister,
    anyRegister
  };

private:
  size_t getNodesRegisterSize(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth);
public:
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

private:
  RichNode getNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t index);
public:
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
  void doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t from, size_t to, std::function<void(RichNode)> lambda);
public:
  inline
  void doForEachXNode(VM vm, Runnable* runnable, size_t from, size_t to,
    std::function<void(RichNode)> lambda) {
    doForEachNode(vm, runnable, xRegister, 0, from, to, lambda);
  }

  inline
  void doForEachYNode(VM vm, Runnable* runnable, size_t depth,
    size_t from, size_t to, std::function<void(RichNode)> lambda) {
    doForEachNode(vm, runnable, yRegister, depth, from, to, lambda);
  }

  inline
  void doForEachGNode(VM vm, Runnable* runnable, size_t depth,
    size_t from, size_t to, std::function<void(RichNode)> lambda) {
    doForEachNode(vm, runnable, gRegister, depth, from, to, lambda);
  }

  inline
  void doForEachKNode(VM vm, Runnable* runnable, size_t depth,
    size_t from, size_t to, std::function<void(RichNode)> lambda) {
    doForEachNode(vm, runnable, kRegister, depth, from, to, lambda);
  }

// public:
//   NodeDescription getNodeDescription(VM vm, Runnable* runnable, size_t index);

//   StaticArray<NodeDescription> getNodeDescriptions(VM vm, Runnable* runnable,
//     size_t from = 0, size_t to = SIZE_MAX);

//   StaticArray<NodeDescription> getNodeDescriptions(VM vm,
//     size_t from = 0, size_t to = SIZE_MAX);

public:
  /* ========== Variables stats ========== */
  size_t getBoundVariablesCount(VM vm);

  size_t getUnBoundVariablesCount(VM vm);

  size_t getVariablesCount(VM vm);
};

}

#endif // MOZART_INTROSPECTION_DECL_H
