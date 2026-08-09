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
  /** @brief Create a introspection object */
  Introspection() {}

public:
  /* ========== VM state ========== */

  /**
   * @brief Get the total number of schedules since VM launch
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  size_t getSchedulesCounter(VM vm);

  /**
   * @brief Get the total number of executed operations since VM launch
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  size_t getOperationsCounter(VM vm);

  /**
   * @brief Get the number of system thread schedules since VM launch
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  size_t getSystemSchedulesCounter(VM vm);

  /**
   * @brief Get the total number of executed system thread operations since VM launch
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  size_t getSystemOperationsCounter(VM vm);

  /**
   * @brief Get the next thread that will be chosen by the scheduler 
   * 
   * @param vm A pointer on the virtual machine
   * @param includeSystemThreads If system threads are included in the threads set or not
   * @return Runnable* The next scheduled thread
   */
  Runnable* getNextScheduledThread(VM vm, bool includeSystemThreads);

  enum ArgumentType {
    I = 0,
    X,
    Y,
    G,
    K,
  };

  /**
   * @brief A simple structure to represent a bytecode instruction's argument containing
   * a type, an index (nth argument) and a value representation.
   */
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

  /**
   * @brief Get the next bytecode instruction executed by a specified thread
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @return Operation A representation of a bytecode instruction and its arguments
   */
  Operation getNextExecutedOperation(VM vm, Runnable* runnable);

  /**
   * @brief Get the next executed bytecode instruction
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @return Operation A representation of a bytecode instruction and its arguments
   */
  Operation getNextExecutedOperation(VM vm, bool includeSystemThreads = false);
public:
  /* ========== Threads stats ========== */

  /** @brief Threads' counts have the following types */
  struct ThreadsCounts {
    size_t activeThreadsCount = 0;
    size_t passiveThreadsCount = 0;
    size_t threadsCount = 0;
  };

public:
  /* ========== Threads getters ========== */

  /**
   * @brief Retrieve a thread with the specified id
   * 
   * @param vm A pointer on the virtual machine
   * @param id The target id
   * @return Runnable* A pointer on the thread or nullptr if not found
   */
  Runnable* getThread(VM vm, size_t id);

  /**
   * @brief Get the threads of the VM
   * 
   * @param vm A pointer on the virtual machine
   * @return RunnableList& A reference on the VM runnables list
   */
  RunnableList& getThreads(VM vm);

public:
  /* ========== Threads executers ========== */

  /**
   * @brief Execute a provided operation on some selected threads
   * 
   * @param vm A pointer on the virtual machine
   * @param valid A predicate lambda to filter threads
   * @param parse A lambda to execute a specific operation on the filtered threads
   */
  void doForEachThread(VM vm, RunnableBoolLambda valid, RunnableLambda parse);

public:
  /* ========== Threads counters ========== */

  /**
   * @brief Get aggregated counts related to threads
   * 
   * @param vm A pointer on the virtual machine
   * @return ThreadsCounts 
   */
  ThreadsCounts getThreadsCounts(VM vm);

  /**
   * @brief Get the number of active threads
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getActiveThreadsCount(VM vm) {
    return getThreadsCounts(vm).activeThreadsCount;
  }

  /**
   * @brief Get the number of passive threads
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getPassiveThreadsCount(VM vm) {
    return getThreadsCounts(vm).passiveThreadsCount;
  }

  /**
   * @brief Get the total number of threads
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
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
  
  /**
   * @brief Get the size of a specified register : a register is dedicated to a thread,
   * it might have a depth (except x registers) and has a type X, Y, G or K.
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @param nodesRegister The target register type
   * @param depth The target register depth (0 for x registers)
   * @return size_t 
   */
  size_t getNodesRegisterSize(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth);
public:
  /* ========== Registers stats ========== */

  /**
   * @brief Get the size of the thread x register
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @return size_t 
   */
  inline
  size_t getXNodesRegisterSize(VM vm, Runnable* runnable) {
    return getNodesRegisterSize(vm, runnable, xRegister, 0);
  }

  /**
   * @brief Get the size of the thread y register
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @param depth A depth to look at
   * @return size_t 
   */
  inline
  size_t getYNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, yRegister, depth);
  }

  /**
   * @brief Get the size of the thread g register
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @param depth A depth to look at
   * @return size_t 
   */
  inline
  size_t getGNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, gRegister, depth);
  }

  /**
   * @brief Get the size of the thread k register
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a thread
   * @param depth A depth to look at
   * @return size_t 
   */
  inline
  size_t getKNodesRegisterSize(VM vm, Runnable* runnable, size_t depth) {
    return getNodesRegisterSize(vm, runnable, kRegister, depth);
  }

public:
  /* ========== Nodes stats ========== */

  /** @brief Nodes' counts have the following types */
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

  /**
   * @brief Get the of the provided node
   * 
   * @param vm A pointer on the virtual machine
   * @param node A pointer on the target node
   * @return Type 
   */
  Type getNodeType(VM vm, Node* node);

  /**
   * @brief Get word of the provided node
   * 
   * @param vm A pointer on the virtual machine
   * @param node A pointer on the target node
   * @return MemWord 
   */
  MemWord getNodeValue(VM vm, Node* node);

public:
  /**
   * @brief Tells if the provided node carries a 'variable' data
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the target rich node
   * @return true 
   * @return false 
   */
  bool isVariableNode(VM vm, RichNode node);

  /**
   * @brief Tells if the provided node carries a 'structural' data
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the target rich node
   * @return true 
   * @return false 
   */
  bool isStructuralNode(VM vm, RichNode node);

  /**
   * @brief Tells if the provided node carries a 'value' data
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the target rich node
   * @return true 
   * @return false 
   */
  bool isValueNode(VM vm, RichNode node);

  /**
   * @brief Tells if the provided node carries a 'token' data
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the target rich node
   * @return true 
   * @return false 
   */
  bool isTokenNode(VM vm, RichNode node);

private:
  /* ========== Nodes counters ========== */

  /**
   * @brief Get aggregated counters about VM thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @param counts A reference on a structure with counters to update
   */
  void getNodesCounts(VM vm, Runnable* runnable, NodesCounts& counts);

public:
  /* ========== Nodes counters ========== */

  /**
   * @brief Get aggregated counters about VM nodes (all threads included)
   * 
   * @param vm A pointer on the virtual machine
   * 
   * @return NodesCounts
   */
  inline
  NodesCounts getNodesCounts(VM vm) {
    NodesCounts counts;
    doForEachThread(vm, allRunnables, [this, &counts](VM vm, Runnable* runnable) {
      this->getNodesCounts(vm, runnable, counts);
    });
    return counts;
  }

  /**
   * @brief Get aggregated counters about VM thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * 
   * @return NodesCounts
   */
  inline
  NodesCounts getNodesCounts(VM vm, Runnable* runnable) {
    NodesCounts counts;
    getNodesCounts(vm, runnable, counts);
    return counts;
  }

  /**
   * @brief Get the count of 'variable' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getVariableNodesCount(VM vm) {
    return getNodesCounts(vm).variableNodesCount;
  }

  /**
   * @brief Get the count of 'variable' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getVariableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).variableNodesCount;
  }

  /**
   * @brief Get the count of 'value' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getValueNodesCount(VM vm) {
    return getNodesCounts(vm).valueNodesCount;
  }

  /**
   * @brief Get the count of 'value' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getValueNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).valueNodesCount;
  }

  /**
   * @brief Get the count of 'structural' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getStructuralNodesCount(VM vm) {
    return getNodesCounts(vm).structuralNodesCount;
  }

  /**
   * @brief Get the count of 'structural' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getStructuralNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).structuralNodesCount;
  }

  /**
   * @brief Get the count of 'token' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getTokenNodesCount(VM vm) {
    return getNodesCounts(vm).tokenNodesCount;
  }

  /**
   * @brief Get the count of 'token' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getTokenNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).tokenNodesCount;
  }

  /**
   * @brief Get the count of 'stable' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getStableNodesCount(VM vm) {
    return getNodesCounts(vm).stableNodesCount;
  }

  /**
   * @brief Get the count of 'stable' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getStableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).stableNodesCount;
  }

  /**
   * @brief Get the count of 'unstable' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getUnstableNodesCount(VM vm) {
    return getNodesCounts(vm).unstableNodesCount;
  }

  /**
   * @brief Get the count of 'unstable' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getUnstableNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).unstableNodesCount;
  }

  /**
   * @brief Get the count of 'x' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getXNodesCount(VM vm) {
    return getNodesCounts(vm).xNodesCount;
  }

  /**
   * @brief Get the count of 'x' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getXNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).xNodesCount;
  }

  /**
   * @brief Get the count of 'y' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getYNodesCount(VM vm) {
    return getNodesCounts(vm).yNodesCount;
  }

  /**
   * @brief Get the count of 'y' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getYNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).yNodesCount;
  }

  /**
   * @brief Get the count of 'g' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getGNodesCount(VM vm) {
    return getNodesCounts(vm).gNodesCount;
  }

  /**
   * @brief Get the count of 'g' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getGNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).gNodesCount;
  }

  /**
   * @brief Get the count of 'k' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getKNodesCount(VM vm) {
    return getNodesCounts(vm).kNodesCount;
  }

  /**
   * @brief Get the count of 'k' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getKNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).kNodesCount;
  }

  /**
   * @brief Get the count of 'stack depth' nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getStackDepth(VM vm) {
    return getNodesCounts(vm).stackDepth;
  }

  /**
   * @brief Get the count of 'stack depth' thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getStackDepth(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).stackDepth;
  }

  /**
   * @brief Get the count of nodes
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getNodesCount(VM vm) {
    return getNodesCounts(vm).nodesCount;
  }

  /**
   * @brief Get the count of thread nodes
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @return size_t 
   */
  inline
  size_t getNodesCount(VM vm, Runnable* runnable) {
    return getNodesCounts(vm, runnable).nodesCount;
  }

private:
  /* ========== Node getters ========== */

  /**
   * @brief Get a specific node from a target register as a RichNode
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param nodesRegister A type of register
   * @param depth A depth where to find the register (if x then depth = 0)
   * @param index A index to find the register from the register
   * @return RichNode 
   */
  RichNode getNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t index);
public:
  /* ========== Node getters ========== */

  /**
   * @brief Get a specific X node from a target register as a RichNode
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param index A index to find the register from the register
   * @return RichNode 
   */
  inline
  RichNode getXNode(VM vm, Runnable* runnable, size_t index) {
    return getNode(vm, runnable, xRegister, 0, index);
  }

  /**
   * @brief Get a specific Y node from a target register as a RichNode
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth A depth where to find the register (if x then depth = 0)
   * @param index A index to find the register from the register
   * @return RichNode 
   */
  inline
  RichNode getYNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, yRegister, depth, index);
  }

  /**
   * @brief Get a specific G node from a target register as a RichNode
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth A depth where to find the register (if x then depth = 0)
   * @param index A index to find the register from the register
   * @return RichNode 
   */
  inline
  RichNode getGNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, gRegister, depth, index);
  }

  /**
   * @brief Get a specific K node from a target register as a RichNode
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth A depth where to find the register (if x then depth = 0)
   * @param index A index to find the register from the register
   * @return RichNode 
   */
  inline
  RichNode getKNode(VM vm, Runnable* runnable, size_t depth, size_t index) {
    return getNode(vm, runnable, kRegister, depth, index);
  }

private:
  /* ========== Nodes executers ========== */

  /**
   * @brief Execute a certain operation on some nodes of a specified register.
   * From a register of a certain thread, at a specified depth it looks up from a node
   * to another and for each execute the operation parse if the predicate valid is
   * satisphied.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param nodesRegister The type of register
   * @param depth The register depth (= 0 if x register)
   * @param from The starting index (< to)
   * @param to The ending index (<= register size)
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  void doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t from, size_t to, NodeBoolLambda valid, RunnableAndNodeLambda parse);

public:
  /* ========== Nodes executers ========== */

  /**
   * @brief Execute operation on some nodes of the x register of the provided thread.
   * From a X register of a certain thread, it looks up from a node
   * to another and for each execute the operation parse if the predicate valid is
   * satisphied.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param from The starting index (< to)
   * @param to The ending index (<= register size)
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachXNode(VM vm, Runnable* runnable, size_t from, size_t to,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, xRegister, 0, from, to,
      valid, parse);
  }

  /**
   * @brief Execute operation on some nodes of the y register of the provided thread.
   * From a Y register of a certain thread, at a specified depth it looks up from a node
   * to another and for each execute the operation parse if the predicate valid is
   * satisphied.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth The register depth (= 0 if x register)
   * @param from The starting index (< to)
   * @param to The ending index (<= register size)
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachYNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, yRegister, depth, from, to,
      valid, parse);
  }

  /**
   * @brief Execute operation on some nodes of the g register of the provided thread.
   * From a G register of a certain thread, at a specified depth it looks up from a node
   * to another and for each execute the operation parse if the predicate valid is
   * satisphied.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth The register depth (= 0 if x register)
   * @param from The starting index (< to)
   * @param to The ending index (<= register size)
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachGNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, gRegister, depth, from, to,
      valid, parse);
  }

  /**
   * @brief Execute operation on some nodes of the k register of the provided thread.
   * From a K register of a certain thread, at a speified depth it looks up from a node
   * to another and for each execute the operation parse if the predicate valid is
   * satisphied.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param depth The register depth (= 0 if x register)
   * @param from The starting index (< to)
   * @param to The ending index (<= register size)
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachKNode(VM vm, Runnable* runnable, size_t depth, size_t from, size_t to,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    doForEachNode(vm, runnable, kRegister, depth, from, to,
      valid, parse);
  }

  /**
   * @brief Execute operation on some nodes of a specified thread.
   * It is like all registers are assembled in order as a huge register.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachNode(VM vm, Runnable* runnable,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    assert(runnable != nullptr);
    
    doForEachXNode(vm, runnable, 0,
      getXNodesRegisterSize(vm, runnable), valid, parse);

    size_t depth = getStackDepth(vm, runnable);
    for (size_t i = 0; i < depth; i++) {
      doForEachYNode(vm, runnable, i, 0,
        getYNodesRegisterSize(vm, runnable, i), valid, parse);
      doForEachGNode(vm, runnable, i, 0,
        getGNodesRegisterSize(vm, runnable, i), valid, parse);
      doForEachKNode(vm, runnable, i, 0,
        getKNodesRegisterSize(vm, runnable, i), valid, parse);
    }
  }

  /**
   * @brief Execute operation on some nodes of a specified thread.
   * It is like for each thread, all registers are assembled in order as a huge register.
   * 
   * @param vm A pointer on a virtual machine
   * @param runnable A pointer on a specific thread
   * @param valid A predicate to valid a node
   * @param parse An operation to execute on filtered nodes
   */
  inline
  void doForEachNode(VM vm, RunnableBoolLambda validRunnable,
    NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    doForEachThread(vm, validRunnable, [this, valid, parse](VM vm, Runnable* runnable) {
      this->doForEachNode(vm, runnable, valid, parse);
    });
  }

public:
 /* ========== Variables stats ========== */

 /** @brief Aggregated counters about variables */
 struct VariablesCounts {
   size_t boundVariablesCount = 0;
   size_t unBoundVariablesCount = 0;
   size_t neededVariablesCount = 0;
   size_t waitedVariablesCount = 0;
   size_t variablesCount = 0;
 };

private:
  /* ========== Variables properties ========== */

  /**
   * @brief Tells if a rich node carries a 'variable' node and if true
   * if this variable is bound.
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the rich node
   * @return true 
   * @return false 
   */
  bool isBoundVariable(VM vm, RichNode node);

  /**
   * @brief Tells if a rich node carries a 'variable' node and if true
   * if this variable is needed.
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the rich node
   * @return true 
   * @return false 
   */
  bool isNeededVariable(VM vm, RichNode node);

  /**
   * @brief Tells if a rich node carries a 'variable' node and if true
   * if this variable is waited.
   * 
   * @param vm A pointer on the virtual machine
   * @param node A reference on the rich node
   * @return true 
   * @return false 
   */
  bool isWaitedVariable(VM vm, RichNode node);

private:
  /* ========== Variables counters ========== */

  /**
   * @brief Get aggregated counters about VM thread variables
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * @param counts A reference on a structure with counters to update
   */
  void getVariablesCounts(VM vm, Runnable* runnable, VariablesCounts& counts);

public:
  /* ========== Variables counters ========== */

  /**
   * @brief Get the aggregated counters about VM thread variables
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specific thread
   * 
   * @return VariablesCounts 
   */
  inline
  VariablesCounts getVariablesCounts(VM vm, Runnable* runnable) {
    VariablesCounts counts;
    getVariablesCounts(vm, runnable, counts);
    return counts;
  }

  /**
   * @brief Get the aggregated counters about VM variables
   * 
   * @param vm A pointer on the virtual machine
   * 
   * @return VariablesCounts 
   */
  inline
  VariablesCounts getVariablesCounts(VM vm) {
    VariablesCounts counts;
    doForEachThread(vm, allRunnables, [this, &counts](VM vm, Runnable* runnable) {
      this->getVariablesCounts(vm, runnable, counts);
    });
    return counts;
  }

  /**
   * @brief Get the bound variables cout
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getBoundVariablesCount(VM vm) {
    return getVariablesCounts(vm).boundVariablesCount;
  }

  /**
   * @brief Get the unbound variables cout
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getUnBoundVariablesCount(VM vm) {
    return getVariablesCounts(vm).unBoundVariablesCount;
  }

  /**
   * @brief Get the needed variables cout
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getNeededVariablesCount(VM vm) {
    return getVariablesCounts(vm).neededVariablesCount;
  }

  /**
   * @brief Get the waited variables cout
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getWaitedVariablesCount(VM vm) {
    return getVariablesCounts(vm).waitedVariablesCount;
  }

  /**
   * @brief Get the variables cout
   * 
   * @param vm A pointer on the virtual machine
   * @return size_t 
   */
  inline
  size_t getVariablesCount(VM vm) {
    return getVariablesCounts(vm).variablesCount;
  }

private:
  /* ========== Variables executers ========== */

  /**
   * @brief 
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specified thread
   * @param nodesRegister A register type
   * @param depth A depth where to find the register
   * @param from 
   * @param to 
   * @param parse 
   */
  void doForEachVariable(VM vm, Runnable* runnable, NodesRegister nodesRegister,
    size_t depth, size_t from, size_t to, RunnableAndNodeLambda parse);
public:
  /* ========== Variables executers ========== */

  /**
   * @brief Execute an operation on each variable of a VM thread
   * 
   * @param vm A pointer on the virtual machine
   * @param runnable A pointer on a specified thread
   * @param parse An operation to execute on a thread variable
   */
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

  /**
   * @brief Execute an operation on each variable of the VM
   * 
   * @param vm A pointer on the virtual machine
   * @param parse An operation to execute on a thread variable
   */
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
  /* ========== Reachability graph ========== */

  using IdsVector = std::vector<size_t>;
  using IdToIdsMap = std::unordered_map<size_t, IdsVector>;

  struct ReachabilityGraph {
    IdToIdsMap threadToVariables;
    IdToIdsMap variableToThreads;
  };

private:
  using Pendings = VMAllocatedList<StableNode*>;
  
  void computeReachabilityGraph(VM vm, ReachabilityGraph& graph, size_t variableId, Pendings& pendings);

public:
  ReachabilityGraph computeReachabilityGraph(VM vm);

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
  using RunnableVector = std::vector<Runnable*>;

  /* ========== Structures list ========== */
  struct OwnedRichNode {
    OwnedRichNode(RichNode node) : node(node) {}

    RunnableVector runnables;
    RichNode node;
  };
  using NodesMap = std::unordered_map<size_t, OwnedRichNode>;

private:
  RunnableAndNodeLambda getAddConsLambda(VM vm, NodesMap& map);

public:
  /* ========== Structures list ========== */
  // RichNode getList(VM vm, size_t id);

  NodesMap getLists(VM vm, Runnable* runnable) {
    NodesMap map;
    doForEachNode(vm, runnable,
      allNodes,
      getAddConsLambda(vm, map)
    );
    return map;
  }

  NodesMap getLists(VM vm) {
    NodesMap map;
    doForEachNode(vm,
      allRunnables,
      allNodes,
      getAddConsLambda(vm, map)
    );
    return map;
  }
};

}

#endif // MOZART_INTROSPECTION_DECL_H

