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
#include "emulate.hh"

namespace mozart {

inline
std::string Introspection::OperationArgument::toRepr(VM vm, RichNode value) {
  auto& config = vm->getPropertyRegistry().config;
  std::basic_stringstream<char> buffer;
  buffer << repr(vm, value, config.printDepth, config.printWidth);
  return buffer.str();
}

/* ========== VM state ========== */

inline
size_t Introspection::getSchedulesCounter(VM vm) {
  return vm->_schedulesCounter;
}

inline
size_t Introspection::getOperationsCounter(VM vm) {
  return vm->_operationsCounter;
}

inline
Runnable* Introspection::getNextScheduledThread(VM vm, bool includeSystemThreads) {
  return vm->getThreadPool().getNext(includeSystemThreads);
}

/* ========== Threads getters ========== */

inline
Runnable* Introspection::getThread(VM vm, size_t id) {
  using iterator = RunnableList::iterator;

  RunnableList& list = vm->aliveThreads;
  for (iterator iter = list.begin(); iter != list.end(); ++iter) {
    Runnable* runnable = *iter;
    if (runnable->getId() == id)
      return runnable;
  }
  return nullptr;
}

inline
RunnableList& Introspection::getThreads(VM vm) {
  return vm->aliveThreads;
}

/* ========== Threads executers ========== */

inline
void Introspection::doForEachThread(VM vm, Introspection::RunnableLambda parse) {
  using iterator = RunnableList::iterator;

  RunnableList& list = getThreads(vm);
  for (iterator iter = list.begin(); iter != list.end(); ++iter) {
    Runnable* runnable = *iter;
    parse(runnable);
  }
}

/* ========== Threads counters ========== */

inline
Introspection::ThreadsCounts Introspection::getThreadsCounts(VM vm) {
  ThreadsCounts counts;
  doForEachThread(vm, [vm, &counts](Runnable* runnable) {
    if (runnable->isRunnable() && !runnable->isTerminated() && !runnable->isDead())
      counts.activeThreadsCount++;
    else
      counts.passiveThreadsCount++;
    counts.threadsCount++;
  });
  return counts;
}

/* ========== Registers stats ========== */

inline
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
    } default: assert(false);
  }
}

/* ========== Nodes properties ========== */

inline
Type Introspection::getNodeType(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.type;
}

inline
MemWord Introspection::getNodeValue(VM vm, Node* node) {
  assert(node != nullptr);
  return node->data.value;
}

inline
bool Introspection::isVariableNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbVariable;
}

inline
bool Introspection::isStructuralNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbStructural;
}

inline
bool Introspection::isValueNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbValue;
}

inline
bool Introspection::isTokenNode(VM vm, RichNode node) {
  if (node.type().info() == nullptr) return false;
  else return node.type()->getStructuralBehavior() == sbTokenEq;
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

inline
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

inline
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
    } default: assert(false);
  }
}

/* ========== Nodes executers ========== */

template<class T>
inline
void doForEachNodeFromStaticArray(VM vm, Runnable* runnable, StaticArray<T> array,
  size_t from, size_t to, Introspection::RunnableBoolLambda validRunnable,
  Introspection::NodeBoolLambda validNode, Introspection::RunnableAndNodeLambda parse) {
  for (size_t i = from; i < to && i < array.size(); i++) {
    RichNode node = RichNode(array[i]);

    if (node.isNullNode())
      continue;

    if (validRunnable(runnable) && validNode(node))
      parse(runnable, node);
  }
}

inline
void Introspection::doForEachNode(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t from, size_t to, RunnableBoolLambda validRunnable,
  NodeBoolLambda validNode, RunnableAndNodeLambda parse) {

  if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
    assert(depth < thread->stack.size());
    StackEntry& entry = thread->stack[depth];

    switch (nodesRegister) {
      case xRegister: {
        assert(depth == 0);
        StaticArray<UnstableNode> xregs = thread->xregs._array;
        // assert(to <= xregs.size());
        doForEachNodeFromStaticArray(vm, runnable, xregs, from, to,
          validRunnable, validNode, parse);
        break;
      } case yRegister: {
        StaticArray<UnstableNode> yregs = entry.yregs;
        // assert(to <= yregs.size());
        doForEachNodeFromStaticArray(vm, runnable, yregs, from, to,
          validRunnable, validNode, parse);
        break;
      } case gRegister: {
        StaticArray<StableNode> gregs = entry.gregs;
        // assert(to <= gregs.size());
        doForEachNodeFromStaticArray(vm, runnable, gregs, from, to,
          validRunnable, validNode, parse);
        break;
      } case kRegister: {
        StaticArray<StableNode> kregs = entry.kregs;
        // assert(to <= kregs.size());
        doForEachNodeFromStaticArray(vm, runnable, kregs, from, to,
          validRunnable, validNode, parse);
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

inline
bool Introspection::isBoundVariable(VM vm, RichNode node) {
  if (!isVariableNode(vm, node)) return false;
  else if (node.is<OptVar>()) return false;
  else if (node.is<Variable>()) 
    return _isBoundVariable<Variable>(vm, node);
  else if (node.is<ReadOnly>()) return false;
  else if (node.is<ReadOnlyVariable>())
    return _isBoundVariable<ReadOnlyVariable>(vm, node);
  else if (node.is<FailedValue>()) return false;
  else assert(false);
}

template<typename T>
static inline
bool _isNeededVariable(VM vm, RichNode& node) {
  return Accessor<T>::get(node.value()).isNeeded(vm);
}

inline
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
  else assert(false);
}

template<typename T>
static inline
bool _isWaitedVariable(VM vm, RichNode& node) {
  return Accessor<T>::get(node.value()).isWaited(vm);
}

inline
bool Introspection::isWaitedVariable(VM vm, RichNode node) {
  if (!isVariableNode(vm, node)) return false;
  else if (node.is<OptVar>()) return false;
  else if (node.is<Variable>()) 
    return _isWaitedVariable<Variable>(vm, node);
  else if (node.is<ReadOnly>()) return false;
  else if (node.is<ReadOnlyVariable>())
    return _isWaitedVariable<ReadOnlyVariable>(vm, node);
  else if (node.is<FailedValue>()) return false;
  else assert(false);
}

/* ========== Variables counters ========== */
inline
void Introspection::getVariablesCounts(VM vm, Runnable* runnable, VariablesCounts& counts) {
  doForEachNode(vm, runnable,
    [](Runnable* _) { return true; },
    [vm, this](RichNode node) { return this->isVariableNode(vm, node); },
    [vm, this, &counts](Runnable* runnable, RichNode node) {
      if (this->isBoundVariable(vm, node)) counts.boundVariablesCount++;
      else counts.unBoundVariablesCount++;
      if (this->isNeededVariable(vm, node)) counts.neededVariablesCount++;
      if (this->isWaitedVariable(vm, node)) counts.waitedVariablesCount++;
      counts.variablesCount++;
    }
  );
}

/* ========== Variables executers ========== */
inline
void Introspection::doForEachVariable(VM vm, Runnable* runnable, NodesRegister nodesRegister,
  size_t depth, size_t from, size_t to, RunnableAndNodeLambda parse) {
  
  doForEachNode(vm, runnable, nodesRegister, depth, from, to,
    [](Runnable* runnable) { return true; },
    [vm, this](RichNode node) { return this->isVariableNode(vm, node); },
    parse
  );
}

inline
bool Introspection::VariableCandidates::has(size_t candidateThreadId) {
  return std::find(candidates.begin(), candidates.end(), candidateThreadId) != candidates.end();
}

inline
void Introspection::VariableCandidates::add(size_t candidateThreadId) {
  candidates.push_back(candidateThreadId);
}

static inline
void updateVariableCandidatesMap(Runnable* runnable, RichNode node, size_t variableId,
  Introspection::VariableCandidatesMap& map) {
  size_t candidateThreadId = runnable->getId();
  bool isNotKey = map.find(variableId) == map.end();

  if (isNotKey) {
    Introspection::VariableCandidates variableCandidates(node);
    variableCandidates.add(candidateThreadId);
    map.insert({variableId, variableCandidates});
  } else {
    map.at(variableId).add(candidateThreadId);
  }
}

static inline
void updateVariableCandidatesMap(Runnable* runnable, RichNode node,
  Introspection::VariableCandidatesMap& map) {
  if (node.is<Variable>()) {
    Variable variable = Accessor<Variable>::get(node.value());
    updateVariableCandidatesMap(runnable, node, variable.getId(), map);
  } else if (node.is<ReadOnlyVariable>()) {
    ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
    updateVariableCandidatesMap(runnable, node, variable.getId(), map);
  } else {
    assert(false);
  }
}

inline
Introspection::VariableCandidates Introspection::getVariable(VM vm, size_t variableId) {
  VariableCandidates variable(RichNode(nullptr));
  doForEachVariable(vm, [vm, &variable, variableId](Runnable* runnable, RichNode node) {
    bool found = false;
    if (node.is<Variable>()) {
      Variable variable = Accessor<Variable>::get(node.value());
      found = variable.getId() == variableId;
    } else if (node.is<ReadOnlyVariable>()) {
      ReadOnlyVariable variable = Accessor<ReadOnlyVariable>::get(node.value());
      found = variable.getId() == variableId;
    } else {
      assert(false);
    }

    if (found) {
      variable.setNode(node);
      variable.add(runnable->getId());
    }
  });
  return variable;
}

inline
Introspection::VariableCandidatesMap Introspection::getVariableCandidatesMap(VM vm, Runnable* runnable) {
  size_t candidateThreadId = runnable->getId();
  VariableCandidatesMap map;
  doForEachVariable(vm, [vm, candidateThreadId, &map](Runnable* runnable, RichNode node) {
    if (runnable->getId() == candidateThreadId)
      updateVariableCandidatesMap(runnable, node, map);
  });
  return map;
}

inline
Introspection::VariableCandidatesMap Introspection::getVariableCandidatesMap(VM vm) {
  VariableCandidatesMap map;
  doForEachVariable(vm, [vm, &map](Runnable* runnable, RichNode node) {
    updateVariableCandidatesMap(runnable, node, map);
  });
  return map;
}

// inline
// Introspection::StructuresCounts Introspection::getStructuresCounts(VM vm) {
//   StructuresCounts counts;

//   RunnableList& list = vm->aliveThreads;
//   for (iterator iter = list.begin(); iter != list.end(); iter++) {
//     Runnable *runnable = *iter;
//     StructuresCounts threadCounts = getStructuresCounts(vm, runnable);

//     counts.consCount += threadCounts.consCount;
//     counts.tuplesCount += threadCounts.tuplesCount;
//     counts.aritiesCount += threadCounts.aritiesCount;
//     counts.recordsCount += threadCounts.recordsCount;
//   }

//   return counts;
// }

// inline
// Introspection::StructuresCounts Introspection::getStructuresCounts(VM vm, Runnable* runnable) {
//   StructuresCounts counts;

//   doForEachNode(vm, runnable, [vm, &counts](Runnable* _, RichNode node) {
//     if (node.is<Cons>())
//       counts.consCount++;
//     if (node.is<Tuple>())
//       counts.tuplesCount++;
//     if (node.is<Arity>())
//       counts.aritiesCount++;
//     if (node.is<Record>())
//       counts.recordsCount++;
//   });
// }

}

#endif // MOZART_INTROSPECTION_H
