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

#ifndef MOZART_INTROSPECTION_H
#define MOZART_INTROSPECTION_H

#include <iostream>
#include <algorithm>

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

std::string Introspection::OperationArgument::toRepr(VM vm, RichNode value) {
  auto& config = vm->getPropertyRegistry().config;
  std::basic_stringstream<char> buffer;
  buffer << repr(vm, value, config.printDepth, config.printWidth);
  return buffer.str();
}

template<class Value>
UnstableNode Introspection::build(VM vm, Value value) {
  // If there are not build() defined for Value inside Introspection
  // then go look outside if there exists one.
  return mozart::build(vm, value);
}

template<class Value>
UnstableNode Introspection::buildVector(VM vm, const Vector<Value>& vector) {
  OzListBuilder builder(vm);
  for (Value value : vector) {
    builder.push_back(vm, build<Value>(vm, value));
  }
  return builder.get(vm);
}

template<class Key, class Value>
UnstableNode Introspection::buildMap(VM vm, const Map<Key, Value>& map) {
  UnstableNode label = Atom::build(vm, "map");
  size_t i = 0, width = map.size();
  UnstableField fields[width];

  for (auto pair : map) {
    Key key = pair.first;
    Value value = pair.second;
    fields[i].feature = build<Key>(vm, key);

    if constexpr (is_Vector_v<Value>)
      fields[i].value = buildVector(vm, value);
    else if constexpr (is_Map_v<Value>)
      fields[i].value = buildMap(vm, value);
    else
      fields[i].value = build<Value>(vm, value);
    
    i++;
  }

  return buildRecordDynamic(vm, label, width, fields);
}

/* ========== VM state ========== */

UnstableNode Introspection::buildOperationArgument(VM vm, const OperationArgument argument) {
  std::string type;

  switch (argument.type) {
    case I: type = "Int"; break;
    case X: type = "X"; break;
    case Y: type = "Y"; break;
    case G: type = "G"; break;
    case K: type = "K"; break;
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

UnstableNode Introspection::buildOperation(VM vm, const Operation operation) {
  OzListBuilder builder(vm);

  for (OperationArgument opArgument : operation.arguments) {
    builder.push_back(vm, buildOperationArgument(vm, opArgument));
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

/* ========== VM getters ========== */

GarbageCollector& Introspection::getGarbageCollector(VM vm) {
  return vm->gc;
}

MemManagedList<Runnable**>& Introspection::getGarbageCollectedThreads(VM vm) {
  return vm->gc.todos.threads;
}

template<class StableOrUnstableNode>
void Introspection::getGarbageCollectorTodos(VM vm,
  GarbageCollectorTodos& todos, Node* nodes) {
  Node* current = nodes;
  while (current != nullptr) {
    StableOrUnstableNode* node = static_cast<StableOrUnstableNode*>(current->grFrom);

    std::cout << "A: " << current->type()->getName().c_str() << std::endl;
    std::cout << "B: " << current->grFrom->type()->getName().c_str() << std::endl;

    if (node->type() == ReifiedThread::type()) {
      size_t id = AdvancedIdentifiable(*node).getId(vm);
      todos.runnableIds.push_back(id);
    } else if (node->type() == Variable::type()
      || node->type() == ReadOnlyVariable::type()) {
      size_t id = AdvancedIdentifiable(*node).getId(vm);
      todos.variableIds.push_back(id);
    } else if (node->type() == Cons::type()) {
      size_t id = AdvancedIdentifiable(*node).getId(vm);
      todos.variableIds.push_back(id);
    }

    current = current->grNext;
  }
}

Introspection::GarbageCollectorTodos Introspection::getGarbageCollectorTodos(VM vm) {
  GarbageCollector& gc = vm->gc;
  GarbageCollectorTodos todos;

  // for (Runnable** runnablePointer : todos.threads) {
  //   if (*runnablePointer != nullptr) {
  //     size_t id = (*runnablePointer)->getId();
  //     todos.runnableIds.push_back(id);
  //   }
  // }

  getGarbageCollectorTodos<StableNode>(vm, todos, gc.todos.stableNodes);
  getGarbageCollectorTodos<UnstableNode>(vm, todos, gc.todos.unstableNodes);
  return todos;
}

/* ========== VM state ========== */

size_t Introspection::getSchedulesCount(VM vm) {
  return vm->_statistics.schedulesCount;
}

size_t Introspection::getOperationsCount(VM vm) {
  return vm->_statistics.operationsCount;
}

size_t Introspection::getSystemSchedulesCount(VM vm) {
  return vm->_statistics.systemSchedulesCount;
}

size_t Introspection::getSystemOperationsCount(VM vm) {
  return vm->_statistics.systemOperationsCount;
}

size_t Introspection::getGCSchedulesCount(VM vm) {
  return vm->_statistics.gcSchedulesCount;
}

Runnable* Introspection::getNextScheduledThread(VM vm, bool includeSystemThreads) {
  return vm->threadPool.getNext(includeSystemThreads);
}

/* ========== Threads stats ========== */

UnstableNode Introspection::buildThreadState(VM vm, const Runnable* runnable) {
  UnstableNode id = build(vm, runnable->getId());
  UnstableNode kindId = build(vm, runnable->getKindId());
  UnstableNode generationId = build(vm, runnable->getGenerationId());
  UnstableNode isRunnable = build(vm, runnable->isRunnable());
  UnstableNode isTerminated = build(vm, runnable->isTerminated());
  UnstableNode isDead = build(vm, runnable->isDead());
  UnstableNode isPreempted = build(vm, runnable->isPreempted());
  UnstableNode isPreemptible = build(vm, runnable->isPreemptible());

  UnstableNode priority;
  switch (runnable->getPriority()) {
    case tpLow: priority = build(vm, "low"); break;
    case tpMiddle: priority = build(vm, "medium"); break;
    case tpHi: priority = build(vm, "high"); break;
    case tpSystem: priority = build(vm, "system"); break;
    default: assert(false);
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
      "terminated"
    ),
    isDead,
    generationId,
    id,
    kindId,
    isPreempted,
    isPreemptible,
    priority,
    isRunnable,
    isTerminated
  );
}

UnstableNode Introspection::buildThreadStatistics(VM vm, const Runnable* runnable) {
  Runnable::Statistics statistics = runnable->Runnable::getStatistics();

  size_t operationsCount = 0, bindsCount = 0;
  if (const Thread* thread = dynamic_cast<const Thread*>(runnable)) {
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
    build(vm, statistics.resumesCount),
    build(vm, statistics.runsCount),
    build(vm, statistics.suspendsCount),
    build(vm, statistics.suspendsOnVarCount)
  );
}

UnstableNode Introspection::buildThreadsCounts(VM vm, const ThreadsCounts& counts) {
  return buildRecord(vm,
    buildArity(vm,
      "threadsCounts",
      "active",
      "passive",
      "total"
    ),
    build(vm, counts.activeThreadsCount),
    build(vm, counts.passiveThreadsCount),
    build(vm, counts.threadsCount)
  );
}

/* ========== Threads getters ========== */

Runnable* Introspection::getThread(VM vm, size_t id) {
  using iterator = RunnableList::iterator;

  RunnableList& list = getThreads(vm);
  for (iterator iter = list.begin(); iter != list.end(); ++iter) {
    Runnable* runnable = *iter;
    if (runnable->getId() == id)
      return runnable;
  }
  return nullptr;
}

RunnableList& Introspection::getThreads(VM vm) {
  return vm->threads;
}

/* ========== Threads executers ========== */

void Introspection::doForEachThread(VM vm, Introspection::RunnableBoolLambda valid, Introspection::RunnableLambda parse) {
  using iterator = RunnableList::iterator;

  RunnableList& list = getThreads(vm);
  for (iterator iter = list.begin(); iter != list.end(); ++iter) {
    Runnable* runnable = *iter;
    if (valid(vm, runnable))
      parse(vm, runnable);
  }
}

/* ========== Threads counters ========== */

Introspection::ThreadsCounts Introspection::getThreadsCounts(VM vm) {
  ThreadsCounts counts;
  doForEachThread(vm, allRunnables, [&counts](VM vm, Runnable* runnable) {
    if (runnable->isRunnable() && !runnable->isTerminated() && !runnable->isDead())
      counts.activeThreadsCount++;
    else
      counts.passiveThreadsCount++;
    counts.threadsCount++;
  });
  return counts;
}

/* ========== Registers stats ========== */

size_t Introspection::getNodesRegisterSize(VM vm, Runnable* runnable,
  NodesRegister nodesRegister, size_t depth) {
  Thread* thread = dynamic_cast<Thread*>(runnable);
  if (!thread)
    return 0;

  assert(depth < thread->stack.size());
  StackEntry& entry = thread->stack[depth];
  
  switch (nodesRegister) {
    case xRegister: {
      assert(depth == 0);
      return thread->xregs._array.size();
    } case yRegister: {
      return entry.yregs.size();
    } case gRegister: {
      return entry.gregs.size();
    } case kRegister: {
      return entry.kregs.size();
    } default: assert(false); return 0;
  }
}

/* ========== Nodes stats ========== */

UnstableNode Introspection::buildNodesCounts(VM vm, const NodesCounts& counts) {
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
    build(vm, counts.gNodesCount),
    build(vm, counts.kNodesCount),
    build(vm, counts.nodesCount),
    build(vm, counts.stableNodesCount),
    build(vm, counts.stackDepth),
    build(vm, counts.structuralNodesCount),
    build(vm, counts.tokenNodesCount),
    build(vm, counts.unstableNodesCount),
    build(vm, counts.valueNodesCount),
    build(vm, counts.variableNodesCount),
    build(vm, counts.xNodesCount),
    build(vm, counts.yNodesCount)
  );
}

/* ========== Nodes properties ========== */

std::string Introspection::nodeToString(VM vm, RichNode node) {
  auto& config = vm->getPropertyRegistry().config;
  std::basic_stringstream<char> buffer;
  buffer << repr(vm, node, config.printDepth, config.printWidth);
  return buffer.str();
}

std::string Introspection::nodeStructuralBehaviorToString(StructuralBehavior behavior) {
  switch (behavior) {
    case sbVariable: return "variable";
    case sbValue: return "value";
    case sbStructural: return "structural";
    case sbTokenEq: return "tokenEq";
    default: assert(false); return "";
  }
}

UnstableNode Introspection::buildNode(VM vm, RichNode node) {
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

Type Introspection::getNodeType(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.type;
}

MemWord Introspection::getNodeValue(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.value;
}

bool Introspection::isVariableNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbVariable;
}

bool Introspection::isStructuralNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbStructural;
}

bool Introspection::isValueNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbValue;
}

bool Introspection::isTokenNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbTokenEq;
}

/* ========== Nodes getters ========== */

RichNode Introspection::getNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t index) {
  Thread* thread = dynamic_cast<Thread*>(runnable);
  if (!thread)
    return RichNode(nullptr);

  assert(depth < thread->stack.size());
  StackEntry& entry = thread->stack[depth];
  
  switch (nodesRegister) {
    case xRegister: {
      assert(depth == 0);
      StaticArray<UnstableNode> xregs = thread->xregs._array;
      assert(index < xregs.size());
      return RichNode(xregs[index]);
    } case yRegister: {
      StaticArray<UnstableNode> yregs = entry.yregs;
      assert(index < yregs.size());
      return RichNode(yregs[index]);
    } case gRegister: {
      StaticArray<StableNode> gregs = entry.gregs;
      assert(index < gregs.size());
      return RichNode(gregs[index]);
    } case kRegister: {
      StaticArray<StableNode> kregs = entry.kregs;
      assert(index < kregs.size());
      return RichNode(kregs[index]);
    } default: assert(false); return RichNode(nullptr);
  }
}

/* ========== Nodes counters ========== */

inline
void parseStaticArray(StaticArray<UnstableNode>& array,
  std::function<void(UnstableNode& unstableNode)> lambda) {
  for (size_t i = 0; i < array.size(); i++) {
    lambda(array[i]);
  }
}

inline
void updateNodesCountsFromNode(VM vm, Introspection::NodesCounts& counts,
  Node* node) {
  switch (node->type()->getStructuralBehavior()) {
    case sbVariable: counts.variableNodesCount++; break;
    case sbValue: counts.valueNodesCount++; break;
    case sbStructural: counts.structuralNodesCount++; break;
    case sbTokenEq: counts.tokenNodesCount++; break;
    default: assert(false);
  }
}

template<class T>
inline
void updateNodesCountsFromNodes(VM vm, Introspection::NodesCounts& counts,
  StaticArray<T> array) {
  counts.nodesCount += array.size();

  for (typename StaticArray<T>::iterator iter = array.begin();
    iter != array.end(); iter++) {
    T* node = static_cast<T*>(*iter);
    updateNodesCountsFromNode(vm, counts, node);
  }
}

inline
void updateNodesCountsFromStaticArray(VM vm, Introspection::NodesCounts& counts,
  StaticArray<StableNode> array) {
  counts.stableNodesCount += array.size();
  updateNodesCountsFromNodes<StableNode>(vm, counts, array);
}

inline
void updateNodesCountsFromStaticArray(VM vm, Introspection::NodesCounts& counts,
  StaticArray<UnstableNode> array) {
  counts.unstableNodesCount += array.size();
  updateNodesCountsFromNodes<UnstableNode>(vm, counts, array);
}

void Introspection::getNodesCounts(VM vm, Runnable* runnable,
  Introspection::NodesCounts& counts) {
  
  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    StaticArray<UnstableNode>& xregs = thread->xregs._array;
    counts.xNodesCount += xregs.size();
    updateNodesCountsFromStaticArray(vm, counts, xregs);

    ThreadStack& stack = thread->stack;
    for (ThreadStack::iterator entry = stack.begin();
      entry != stack.end(); ++entry) {
      counts.stackDepth++;

      StaticArray<UnstableNode>& yregs = entry->yregs;
      StaticArray<StableNode>& gregs = entry->gregs;
      StaticArray<StableNode>& kregs = entry->kregs;

      counts.yNodesCount += yregs.size();
      counts.gNodesCount += gregs.size();
      counts.kNodesCount += kregs.size();

      updateNodesCountsFromStaticArray(vm, counts, yregs);
      updateNodesCountsFromStaticArray(vm, counts, gregs);
      updateNodesCountsFromStaticArray(vm, counts, kregs);
    }
  }
}

/* ========== Nodes getters ========== */


/* ========== Nodes executers ========== */

template<class T>
inline
void doForEachNodeFromStaticArray(VM vm, Runnable* runnable, StaticArray<T> array,
  size_t from, size_t to, Introspection::NodeBoolLambda valid,
  Introspection::RunnableAndNodeLambda parse) {
  for (size_t i = from; i < to && i < array.size(); i++) {
    RichNode node = RichNode(array[i]);

    if (node.isNullNode())
      continue;

    if (valid(vm, node))
      parse(vm, runnable, node);
  }
}

void Introspection::doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t from, size_t to, NodeBoolLambda valid, RunnableAndNodeLambda parse) {
    
  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    assert(depth < thread->stack.size());
    StackEntry& entry = thread->stack[depth];

    switch (nodesRegister) {
      case xRegister: {
        assert(depth == 0);
        StaticArray<UnstableNode> xregs = thread->xregs._array;
        // assert(to <= xregs.size());
        doForEachNodeFromStaticArray(vm, runnable, xregs, from, to,
          valid, parse);
        break;
      } case yRegister: {
        StaticArray<UnstableNode> yregs = entry.yregs;
        // assert(to <= yregs.size());
        doForEachNodeFromStaticArray(vm, runnable, yregs, from, to,
          valid, parse);
        break;
      } case gRegister: {
        StaticArray<StableNode> gregs = entry.gregs;
        // assert(to <= gregs.size());
        doForEachNodeFromStaticArray(vm, runnable, gregs, from, to,
          valid, parse);
        break;
      } case kRegister: {
        StaticArray<StableNode> kregs = entry.kregs;
        // assert(to <= kregs.size());
        doForEachNodeFromStaticArray(vm, runnable, kregs, from, to,
          valid, parse);
        break;
      } default: assert(false);
    }
  }  
}

/* ========== Variables properties ========== */

template<typename T>
static inline
bool _isBoundVariable(VM vm, RichNode& node) {
  return Accessor<T>::get(node.value()).isNeeded(vm);
}

bool Introspection::isBoundVariable(VM vm, RichNode node) {
  if (!isVariableNode(vm, node)) return false;
  else if (node.is<OptVar>()) return false;
  else if (node.is<Variable>()) 
    return _isBoundVariable<Variable>(vm, node);
  else if (node.is<ReadOnly>()) return false;
  else if (node.is<ReadOnlyVariable>())
    return _isBoundVariable<ReadOnlyVariable>(vm, node);
  else if (node.is<FailedValue>()) return false;
  else {
    assert(false);
    return false;
  }
}

template<typename T>
static inline
bool _isNeededVariable(VM vm, RichNode& node) {
  return Accessor<T>::get(node.value()).isNeeded(vm);
}

bool Introspection::isNeededVariable(VM vm, RichNode node) {
  if (!isVariableNode(vm, node)) return false;
  else if (node.is<OptVar>())
    return _isNeededVariable<OptVar>(vm, node);
  else if (node.is<Variable>())
    return _isNeededVariable<Variable>(vm, node);
  else if (node.is<ReadOnly>())
    return _isNeededVariable<ReadOnly>(vm, node);
  else if (node.is<ReadOnlyVariable>())
    return _isNeededVariable<ReadOnlyVariable>(vm, node);
  else if (node.is<FailedValue>())
    return _isNeededVariable<FailedValue>(vm, node);
  else {
    assert(false);
    return false;
  }
}

template<typename T>
static inline
bool _isWaitedVariable(VM vm, RichNode& node) {
  return Accessor<T>::get(node.value()).isWaited(vm);
}

bool Introspection::isWaitedVariable(VM vm, RichNode node) {
  if (!isVariableNode(vm, node)) return false;
  else if (node.is<OptVar>()) return false;
  else if (node.is<Variable>()) 
    return _isWaitedVariable<Variable>(vm, node);
  else if (node.is<ReadOnly>()) return false;
  else if (node.is<ReadOnlyVariable>())
    return _isWaitedVariable<ReadOnlyVariable>(vm, node);
  else if (node.is<FailedValue>()) return false;
  else {
    assert(false);
    return false;
  }
}

/* ========== Variables counters ========== */

void Introspection::getVariablesCounts(VM vm, Runnable* runnable, VariablesCounts& counts) {
  doForEachNode(vm, runnable,
    [this](VM vm, RichNode node) { return this->isVariableNode(vm, node); },
    [this, &counts](VM vm, Runnable* runnable, RichNode node) {
      if (this->isBoundVariable(vm, node)) counts.boundVariablesCount++;
      else counts.unBoundVariablesCount++;
      if (this->isNeededVariable(vm, node)) counts.neededVariablesCount++;
      if (this->isWaitedVariable(vm, node)) counts.waitedVariablesCount++;
      counts.variablesCount++;
    }
  );
}

/* ========== Variables executers ========== */

void Introspection::doForEachVariable(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t from, size_t to, RunnableAndNodeLambda parse) {
  
  doForEachNode(vm, runnable,
    nodesRegister, depth, from, to,
    [this](VM vm, RichNode node) { return this->isVariableNode(vm, node); },
    parse
  );
}

bool Introspection::VariableState::has(const IdsVector& ids, Id id) {
  return std::find(ids.begin(), ids.end(), id) != ids.end();
}

void Introspection::VariableState::add(IdsVector& ids, Id id) {
  ids.push_back(id);
}

/* ========== Variables state ========== */

UnstableNode Introspection::buildVariable(VM vm, const VariableState& variable) {
  RichNode node = variable.node;

  if (node.isNullNode())
    return build(vm, "none");

  size_t id = SIZE_MAX, kindId = SIZE_MAX, generationId = SIZE_MAX;
  bool isBound = false, isNeeded = false;

  if (node.type() == Variable::type()) {
    Variable variable = Accessor<Variable>::get(node.value());
    id = variable.getId();
    kindId = variable.getKindId();
    generationId = variable.getGenerationId();
    isBound = variable.isBound(vm);
    isNeeded = variable.isNeeded(vm);
  } else if (node.type() == ReadOnlyVariable::type()) {
    ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
    id = variable.getId();
    kindId = variable.getKindId();
    generationId = variable.getGenerationId();
    isBound = variable.isBound(vm);
    isNeeded = variable.isNeeded(vm);
  } else return build(vm, "none");

  std::string type = node.type()->getName();
  std::string representation = Introspection::nodeToString(vm, node);

  return buildRecord(vm,
    buildArity(vm,
      "variable",
      "candidates",
      "generationId",
      "id",
      "isBound",
      "isNeeded",
      "kindId",
      "pendings",
      "type",
      "value"
    ),
    buildVector<Id>(vm, variable.candidates),
    build(vm, generationId),
    build(vm, id),
    build(vm, isBound),
    build(vm, isNeeded),
    build(vm, kindId),
    buildVector<Id>(vm, variable.pendings),
    build(vm, type.c_str()),
    build(vm, representation.c_str())
  );
}

template<>
inline
UnstableNode Introspection::build(VM vm, VariableState state) {
  return buildVariable(vm, state);
}

/* ========== Variables candidates extractors ========== */

void Introspection::pendingsToIdsVector(VM vm, VariableState& state, Pendings& pendings) {
  for (StableNode* nodePointer : pendings) {
    StableNode& node = *nodePointer;
    if (node.type() == ReifiedThread::type())
      state.addPending(Identifiable(node).getId(vm));
  }
}

void Introspection::getVariablePartially(VM vm, VariableState& state, RichNode node) {
  state.node = node;

  if (node.type() == Variable::type()) {
    Variable variable = Accessor<Variable>::get(node.value());
    Pendings& pendings = variable.pendings;
    pendingsToIdsVector(vm, state, pendings);
  } else if (node.type() == ReadOnlyVariable::type()) {
    ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
    Pendings& pendings = variable.pendings;
    pendingsToIdsVector(vm, state, pendings);
  } else assert(false);
}

Introspection::VariableState Introspection::getVariable(VM vm, Id id) {
  VariableState state = VariableState(RichNode(nullptr));
  doForEachVariable(vm,
    [this, &state, id](VM vm, Runnable* runnable, RichNode node) {
      bool found = false;
      if (node.is<Variable>()) {
        Variable variable = Accessor<Variable>::get(node.value());
        found = variable.getId() == id;
      } else if (node.is<ReadOnlyVariable>()) {
        ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
        found = variable.getId() == id;
      } else assert(false);

      if (found) {
        if (state.node.isNullNode())
          this->getVariablePartially(vm, state, node);
        state.addCandidate(runnable->getId());
      }
    }
  );
  return state;
}

Introspection::IdToVariableStateMap Introspection::getVariables(VM vm, Runnable* runnable) {
  IdToVariableStateMap map = getVariables(vm);
  
  for (auto iter = map.begin(); iter != map.end();) {
    Id id = runnable->getId();
    VariableState& state = iter->second;
    if (!state.hasPending(id) && !state.hasCandidate(id))
      iter = map.erase(iter);
    else ++iter;
  }

  return map;
}

Introspection::IdToVariableStateMap Introspection::getVariables(VM vm) {
  IdToVariableStateMap map;

  doForEachVariable(vm,
    [this, &map](VM vm, Runnable* runnable, RichNode node) {
      Id id;
      if (node.is<Variable>()) {
        Variable variable = Accessor<Variable>::get(node.value());
        id = variable.getId();
      } else if (node.is<ReadOnlyVariable>()) {
        ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
        id = variable.getId();
      } else assert(false);

      if (!map.contains(id)) {
        VariableState state = VariableState(node);
        this->getVariablePartially(vm, state, node);
        map.insert({id, state});
      }

      map[id].addCandidate(runnable->getId());
    }
  );

  return map;
}

/* ========== Reachability graph ========== */

void Introspection::computeReachabilityGraph(VM vm, ReachabilityGraph& graph, size_t variableId,
  Introspection::Pendings& pendings) {
  for (Pendings::iterator iter = pendings.begin(); iter != pendings.end(); ++iter) {
    RichNode node = RichNode(*static_cast<StableNode*>(*iter));
    if (node.is<ReifiedThread>()) {
      Runnable* runnable = getArgument<Runnable*>(vm, node);
      size_t threadId = runnable->getId();

      if (graph.variableToThreads.contains(variableId))
        graph.variableToThreads.insert({variableId, {threadId}});
      else
        graph.variableToThreads.at(variableId).push_back(threadId);
    }
  }
}

Introspection::ReachabilityGraph Introspection::computeReachabilityGraph(VM vm) {
  ReachabilityGraph graph;

  doForEachNode(vm,
    [](VM vm, Runnable* runnable) {
      return runnable->isRunnable() && runnable->isAlive();
    },
    [this](VM vm, RichNode node) {
      return this->isVariableNode(vm, node);
    },
    [this, &graph](VM vm, Runnable* runnable, RichNode node) {
      Id threadId = runnable->getId();
      Id variableId = SIZE_MAX;
      
      if (node.isNullNode()) return;
      else if (node.is<Variable>()) {
        Variable variable = node.getAs<Variable>();
        variableId = variable.getId();
        this->computeReachabilityGraph(vm, graph, variableId, variable.getPendings(vm));
      } else if (node.is<ReadOnlyVariable>()) {
        ReadOnlyVariable readOnlyVariable = node.getAs<ReadOnlyVariable>();
        variableId = readOnlyVariable.getId();
        this->computeReachabilityGraph(vm, graph, variableId, readOnlyVariable.getPendings(vm));
      } else return;
      
      if (graph.threadToVariables.contains(threadId))
        graph.threadToVariables.insert({threadId, {variableId}});
      else
        graph.threadToVariables.at(threadId).push_back(variableId);
    }
  );

  return graph;
}

/* ========== Structures counters ========== */

inline
Introspection::StructuresCounts Introspection::getStructuresCounts(VM vm) {
  StructuresCounts counts;

  doForEachThread(vm, allRunnables, [this, &counts](VM vm, Runnable* runnable) {
    StructuresCounts threadCounts = this->getStructuresCounts(vm, runnable);

    counts.consCount += threadCounts.consCount;
    counts.tuplesCount += threadCounts.tuplesCount;
    counts.aritiesCount += threadCounts.aritiesCount;
    counts.recordsCount += threadCounts.recordsCount;
  });

  return counts;
}

inline
Introspection::StructuresCounts Introspection::getStructuresCounts(VM vm, Runnable* runnable) {
  StructuresCounts counts;

  doForEachNode(vm, runnable,
    allNodes,
    [&counts](VM vm, Runnable* _, RichNode node) {
    if (node.is<Cons>())
      counts.consCount++;
    else if (node.is<Tuple>())
      counts.tuplesCount++;
    else if (node.is<Arity>())
      counts.aritiesCount++;
    else if (node.is<Record>())
      counts.recordsCount++;
  });

  return counts;
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_INTROSPECTION_H
