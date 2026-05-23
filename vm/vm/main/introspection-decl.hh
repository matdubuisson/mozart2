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

  size_t getTotalThreadsCount(VM vm);

  /* ========== Nodes stats ========== */
public:
  struct NodesProperties {
    size_t nVariableNodes = 0;
    size_t nValueNodes = 0;
    size_t nStructuralNodes = 0;
    size_t nTokenNodes = 0;

    size_t nStableNodes = 0;
    size_t nUnstableNodes = 0;

    size_t nXNodes = 0;
    size_t nYNodes = 0;
    size_t nGNodes = 0;
    size_t nKNodes = 0;

    size_t stackDepth = 0; // Is irrelevant for an aggregate of several threads

    size_t nNodes = 0;
  };
private:
  void getNodesProperties(VM vm, Runnable* runnable, NodesProperties& properties);
public:
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
    return getNodesProperties(vm).nVariableNodes;
  }

  inline
  size_t getValueNodesCount(VM vm) {
    return getNodesProperties(vm).nValueNodes;
  }

  inline
  size_t getStructuralNodesCount(VM vm) {
    return getNodesProperties(vm).nStructuralNodes;
  }

  inline
  size_t getTokenNodesCount(VM vm) {
    return getNodesProperties(vm).nTokenNodes;
  }

  inline
  size_t getStableNodesCount(VM vm) {
    return getNodesProperties(vm).nStableNodes;
  }

  inline
  size_t getUnstableNodesCount(VM vm) {
    return getNodesProperties(vm).nUnstableNodes;
  }

  inline
  size_t getNodesCount(VM vm) {
    return getNodesProperties(vm).nNodes;
  }
public:
  /* ========== Variables stats ========== */
  size_t getBoundVariablesCount(VM vm);

  size_t getUnBoundVariablesCount(VM vm);

  size_t getTotalVariablesCount(VM vm);
};

}

#endif // MOZART_INTROSPECTION_DECL_H
