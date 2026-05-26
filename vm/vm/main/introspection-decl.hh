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
  Runnable* getNextScheduledThread(VM vm, bool includeSystemThreads);

public:
  /* ========== Threads state ========== */
  RunnableList& getThreads(VM vm);

  Runnable* getThread(VM vm, size_t id);
  
  void displayNode(VM vm, UnstableNode& node);
  // Tmp function for me testing stuff
  void displayThread(VM vm, Runnable* runnable);

  /* ========== Threads stats ========== */

  size_t getActiveThreadsCount(VM vm);

  size_t getPassiveThreadsCount(VM vm);

  size_t getThreadsCount(VM vm);

  /* ========== Nodes stats ========== */
public:
  struct NodesProperties {
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
  void getNodesProperties(VM vm, Runnable* runnable, NodesProperties& properties);
public:
  UnstableNode* getXNode(VM vm, Runnable* runnable, size_t index);

  UnstableNode* getYNode(VM vm, Runnable* runnable, size_t depth, size_t index);

  StableNode* getGNode(VM vm, Runnable* runnable, size_t depth, size_t index);

  StableNode* getKNode(VM vm, Runnable* runnable, size_t depth, size_t index);

  NodesProperties getNodesProperties(VM vm);

  NodesProperties getNodesProperties(VM vm, Runnable *runnable);

public:
  enum NodeStore {
    xRegister,
    yRegister,
    gRegister,
    kRegister
  };

  struct NodeDescription {
    std::string type;
    StructuralBehavior behavior;
    bool isStable;
    NodeStore store;
    size_t index;
  };
// public:
//   NodeDescription getNodeDescription(VM vm, Runnable* runnable, size_t index);

//   StaticArray<NodeDescription> getNodeDescriptions(VM vm, Runnable* runnable,
//     size_t from = 0, size_t to = SIZE_MAX);

//   StaticArray<NodeDescription> getNodeDescriptions(VM vm,
//     size_t from = 0, size_t to = SIZE_MAX);
public:
  inline
  size_t getVariableNodesCount(VM vm) {
    return getNodesProperties(vm).variableNodesCount;
  }

  inline
  size_t getValueNodesCount(VM vm) {
    return getNodesProperties(vm).valueNodesCount;
  }

  inline
  size_t getStructuralNodesCount(VM vm) {
    return getNodesProperties(vm).structuralNodesCount;
  }

  inline
  size_t getTokenNodesCount(VM vm) {
    return getNodesProperties(vm).tokenNodesCount;
  }

  inline
  size_t getStableNodesCount(VM vm) {
    return getNodesProperties(vm).stableNodesCount;
  }

  inline
  size_t getUnstableNodesCount(VM vm) {
    return getNodesProperties(vm).unstableNodesCount;
  }

  inline
  size_t getXNodesCount(VM vm) {
    return getNodesProperties(vm).xNodesCount;
  }

  inline
  size_t getYNodesCount(VM vm) {
    return getNodesProperties(vm).yNodesCount;
  }

  inline
  size_t getGNodesCount(VM vm) {
    return getNodesProperties(vm).gNodesCount;
  }

  inline
  size_t getKNodesCount(VM vm) {
    return getNodesProperties(vm).kNodesCount;
  }

  inline
  size_t getStackDepth(VM vm) {
    return getNodesProperties(vm).stackDepth;
  }

  inline
  size_t getNodesCount(VM vm) {
    return getNodesProperties(vm).nodesCount;
  }
public:
  /* ========== Variables stats ========== */
  size_t getBoundVariablesCount(VM vm);

  size_t getUnBoundVariablesCount(VM vm);

  size_t getVariablesCount(VM vm);
};

}

#endif // MOZART_INTROSPECTION_DECL_H
