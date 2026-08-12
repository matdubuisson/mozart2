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

#ifndef MOZART_VMEVENTMANAGER_DECL_H
#define MOZART_VMEVENTMANAGER_DECL_H

#include <cstdlib>

#include "core-forward-decl.hh"
#include "identifiable-decl.hh"

#include "runnable-decl.hh"
#include "store-decl.hh"

namespace mozart {

////////////////////////////////
// VirtualMachineEventManager //
////////////////////////////////

class VirtualMachineEventManager {
public:
  VirtualMachineEventManager() : trackingTriggered(false) {}

private:
  static inline
  Runnable* getCurrentThread(VM vm);

public:
  enum class RunnableAnnounce {
    Inserted,
    Removed,
    Updated,
    Collected
  };

  struct RunnableInfo {
    explicit RunnableInfo(VM vm, Runnable* runnable) : runnable(runnable) {
      author = getCurrentThread(vm);
    }
    
    Runnable *author;
    Runnable* runnable;
  };

  using RunnablesVector = std::vector<RunnableInfo>;
public:
  void announceRunnable(VM vm, Runnable* runnable, RunnableAnnounce announce) {
    RunnableInfo info = RunnableInfo(vm, runnable);

    detectTriggeredRunnableTracking(vm, info, announce);

    switch (announce) {
      case RunnableAnnounce::Inserted: insertedRunnables.push_back(info); break;
      case RunnableAnnounce::Removed: removedRunnables.push_back(info); break;
      case RunnableAnnounce::Updated: updatedRunnables.push_back(info); break;
      case RunnableAnnounce::Collected: collectedRunnables.push_back(info); break;
      default: assert(false);
    }
  }

  RunnablesVector& getRunnables(RunnableAnnounce announce) {
    switch (announce) {
      case RunnableAnnounce::Inserted: return insertedRunnables;
      case RunnableAnnounce::Removed: return removedRunnables;
      case RunnableAnnounce::Updated: return updatedRunnables;
      case RunnableAnnounce::Collected: return collectedRunnables;
      default: assert(false); return insertedRunnables;
    }
  }

  bool empty(RunnableAnnounce announce) {
    return getRunnables(announce).empty();
  }
public:
  enum class VariableAnnounce {
    Created,
    Collected,
    Needed,
    Waited,
    Bound
  };

  template<typename V>
  struct VariableInfo {
    explicit VariableInfo(VM vm, V* v) : variable(v) {
      author = getCurrentThread(vm);
    }

    Runnable* author;
    V* variable;
  };

  template<typename V>
  using VariablesVector = std::vector<VariableInfo<V>>;

  template<typename V>
  struct BoundVariableInfo : public VariableInfo<V> {
    explicit BoundVariableInfo(VM vm, V* v, RichNode self, RichNode src) :
      VariableInfo<V>(vm, v), self(self), src(src) {}

    RichNode self, src;
  };

  template<typename V>
  using BoundVariablesVector = std::vector<BoundVariableInfo<V>>;

  template<typename V>
  struct WaitedVariableInfo : public VariableInfo<V>{
    explicit WaitedVariableInfo(VM vm, V* v, RichNode w) :
      VariableInfo<V>(vm, v), waiter(w) {}

    RichNode waiter;
  };

  template<typename V>
  using WaitedVariablesVector = std::vector<WaitedVariableInfo<V>>;

  template<typename V>
  struct VariablesVectors {
    VariablesVector<V> createds;
    VariablesVector<V> collecteds;
    VariablesVector<V> neededs;
    BoundVariablesVector<V> bounds;
    WaitedVariablesVector<V> waiteds;

    void clear() {
      createds.clear();
      collecteds.clear();
      neededs.clear();
      bounds.clear();
      waiteds.clear();
    }
  };

private:
  template<typename V>
  void announceVariable(VM vm, VariablesVectors<V>& vector, V* variable, VariableAnnounce announce) {
    VariableInfo<V> info = VariableInfo(vm, variable);

    detectTriggeredVariableTracking<V, VariableInfo<V>>(vm, info, announce);

    switch (announce) {
      case VariableAnnounce::Created:
        vector.createds.push_back(info);
        break;
      case VariableAnnounce::Collected:
        vector.collecteds.push_back(info);
        break;
      case VariableAnnounce::Needed:
        vector.neededs.push_back(info);
        break;
      default: assert(false);
    }
  }

  template<typename V>
  void announceBoundVariable(VM vm, VariablesVectors<V>& vector, V* variable, RichNode self, RichNode src) {
    BoundVariableInfo<V> info = BoundVariableInfo(vm, variable, self, src);
    detectTriggeredVariableTracking<V, BoundVariableInfo<V>>(vm, info, VariableAnnounce::Bound);
    vector.bounds.push_back(info);
  }

  template<typename V>
  void announceWaitedVariable(VM vm, VariablesVectors<V>& vector, V* variable, RichNode waiter) {
    WaitedVariableInfo<V> info = WaitedVariableInfo(vm, variable, waiter);
    detectTriggeredVariableTracking<V, WaitedVariableInfo<V>>(vm, info, VariableAnnounce::Waited);
    vector.waiteds.push_back(info);
  }

public:
  template<class V>
  void announceVariableBase(VM vm, VariableBase<V>* variable, VariableAnnounce announce) {
    if constexpr (std::is_same_v<V, Variable>) {
      announceVariable(vm, aggregatedVariables,
        static_cast<Variable*>(variable), announce);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceVariable(vm, aggregatedReadOnlyVariables,
        static_cast<ReadOnlyVariable*>(variable), announce);
    } else assert(false);
  }

  template<class V>
  void announceBoundVariableBase(VM vm, VariableBase<V>* variable, RichNode self, RichNode src) {
    if constexpr (std::is_same_v<V, Variable>) {
      announceBoundVariable(vm, aggregatedVariables,
        static_cast<Variable*>(variable), self, src);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceBoundVariable(vm, aggregatedReadOnlyVariables,
        static_cast<ReadOnlyVariable*>(variable), self, src);
    } else assert(false);
  }

  template<class V>
  void announceWaitedVariableBase(VM vm, VariableBase<V>* variable, RichNode waiter) {
    if constexpr (std::is_same_v<V, Variable>) {
      announceWaitedVariable(vm, aggregatedVariables,
        static_cast<Variable*>(variable), waiter);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceWaitedVariable(vm, aggregatedReadOnlyVariables,
        static_cast<ReadOnlyVariable*>(variable), waiter);
    } else assert(false);
  }

  template<class V>
  void announceVariable(VM vm, V* variable, VariableAnnounce announce) {
    if constexpr (std::is_same_v<V, OptVar>) {
      announceVariable(vm, aggregatedOptVariables, variable, announce);
    } else if constexpr (std::is_same_v<V, Variable>) {
      announceVariable(vm, aggregatedVariables, variable, announce);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceVariable(vm, aggregatedReadOnlyVariables, variable, announce);
    } else assert(false);
  }

  template<class V>
  void announceBoundVariable(VM vm, V* variable, RichNode self, RichNode src) {
    if constexpr (std::is_same_v<V, OptVar>) {
      announceBoundVariable(vm, aggregatedOptVariables, variable, self, src);
    } else if constexpr (std::is_same_v<V, Variable>) {
      announceBoundVariable(vm, aggregatedVariables, variable, self, src);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceBoundVariable(vm, aggregatedReadOnlyVariables, variable, self, src);
    } else assert(false);
  }

  template<class V>
  void announceWaitedVariable(VM vm, V* variable, RichNode waiter) {
    if constexpr (std::is_same_v<V, OptVar>) {
      announceWaitedVariable(vm, aggregatedOptVariables, variable, waiter);
    } else if constexpr (std::is_same_v<V, Variable>) {
      announceWaitedVariable(vm, aggregatedVariables, variable, waiter);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      announceWaitedVariable(vm, aggregatedReadOnlyVariables, variable, waiter);
    } else assert(false);
  }

private:
  template<typename V>
  VariablesVector<V>& getVariables(VariablesVectors<V>& vector, VariableAnnounce announce) {
    switch (announce) {
      case VariableAnnounce::Created: return vector.createds;
      case VariableAnnounce::Collected: return vector.collecteds;
      case VariableAnnounce::Needed: return vector.neededs;
      default: assert(false); return vector.createds;
    }
  }

  template<typename V>
  BoundVariablesVector<V>& getBoundVariables(VariablesVectors<V>& vector) {
    return vector.bounds;
  }

  template<typename V>
  WaitedVariablesVector<V>& getWaitedVariables(VariablesVectors<V>& vector) {
    return vector.waiteds;
  }

public:
  template<typename V>
  VariablesVector<V>& getVariables(VariableAnnounce announce) {
    if constexpr (std::is_same_v<V, OptVar>) {
      return getVariables(aggregatedOptVariables, announce);
    } else if constexpr (std::is_same_v<V, Variable>) {
      return getVariables(aggregatedVariables, announce);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      return getVariables(aggregatedReadOnlyVariables, announce);
    } else assert(false);
  }

  template<typename V>
  BoundVariablesVector<V>& getBoundVariables() {
    if constexpr (std::is_same_v<V, OptVar>) {
      return getBoundVariables(aggregatedOptVariables);
    } else if constexpr (std::is_same_v<V, Variable>) {
      return getBoundVariables(aggregatedVariables);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      return getBoundVariables(aggregatedReadOnlyVariables);
    } else assert(false);
  }

  template<typename V>
  WaitedVariablesVector<V>& getWaitedVariables() {
    if constexpr (std::is_same_v<V, OptVar>) {
      return getWaitedVariables(aggregatedOptVariables);
    } else if constexpr (std::is_same_v<V, Variable>) {
      return getWaitedVariables(aggregatedVariables);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      return getWaitedVariables(aggregatedReadOnlyVariables);
    } else assert(false);
  }

  template<typename V>
  bool empty(VariableAnnounce announce) {
    if (announce == VariableAnnounce::Bound)
      return getBoundVariables<V>().empty();
    else if (announce == VariableAnnounce::Waited)
      return getWaitedVariables<V>().empty();
    else
      return getVariables<V>(announce).empty();
  }

  bool empty(VariableAnnounce announce) {
    return empty<OptVar>(announce)
      && empty<Variable>(announce)
      && empty<ReadOnlyVariable>(announce);
  }

public:
  enum class StructureAnnounce {
    Created,
    Collected
  };

  template<class S>
  struct StructureInfo {
    explicit StructureInfo(VM vm, S* structure) : structure(structure) {
      author = getCurrentThread(vm);
    }

    Runnable* author;
    S* structure;
  };

  template<class S>
  using StructuresVector = std::vector<StructureInfo<S>>;

  template<class S>
  struct StructuresVectors {
    StructuresVector<S> createds;
    StructuresVector<S> collecteds;

    void clear() {
      createds.clear();
      collecteds.clear();
    }
  };

private:
  template<class S>
  void announceStructure(VM vm, StructuresVectors<S>& vectors,
    StructureInfo<S> info, StructureAnnounce announce) {
    switch (announce) {
      case StructureAnnounce::Created:
        vectors.createds.push_back(info);
        break;
      case StructureAnnounce::Collected:
        vectors.collecteds.push_back(info);
        break;
      default: assert(false);
    }
  }

public:
  template<class S>
  void announceStructure(VM vm, S* structure, StructureAnnounce announce) { 
    StructureInfo<S> info = StructureInfo<S>(vm, structure);
    detectTriggeredStructureTracking<S>(vm, info, announce);

    // std::cout << "New struct: " << structure->getId() << std::endl;

    if constexpr (std::is_same_v<S, Abstraction>) {
      announceStructure<S>(vm, abstractionStructuresVectors, info, announce);
    } else if constexpr (std::is_same_v<S, Cons>) {
      announceStructure<S>(vm, consStructuresVectors, info, announce);
    } else if constexpr (std::is_same_v<S, Tuple>) {
      announceStructure<S>(vm, tupleStructuresVectors, info, announce);
    } else if constexpr (std::is_same_v<S, Record>) {
      announceStructure<S>(vm, recordStructuresVectors, info, announce);
    } else assert(false);
  }

  template<class S>
  StructuresVector<S>& getStructures(StructuresVectors<S>& vectors, StructureAnnounce announce) {
    switch (announce) {
      case StructureAnnounce::Created: return vectors.createds;
      case StructureAnnounce::Collected: return vectors.collecteds;
      default: assert(false); return vectors.createds;
    }
  }

  template<class S>
  StructuresVector<S>& getStructures(StructureAnnounce announce) {
    if constexpr (std::is_same_v<S, Abstraction>) {
      return getStructures<Abstraction>(abstractionStructuresVectors, announce);
    } else if constexpr (std::is_same_v<S, Cons>) {
      return getStructures<Cons>(consStructuresVectors, announce);
    } else if constexpr (std::is_same_v<S, Tuple>) {
      return getStructures<Tuple>(tupleStructuresVectors, announce);
    } else if constexpr (std::is_same_v<S, Record>) {
      return getStructures<Record>(recordStructuresVectors, announce);
    } else {
      assert(false);
      return getStructures<S>(StructureAnnounce::Created); // Anti-warning and never executed
    }
  }

  template<class S>
  bool empty(StructureAnnounce announce) {
    return getStructures<S>(announce).empty();
  }

  bool empty(StructureAnnounce announce) {
    return empty<Abstraction>(announce) && empty<Cons>(announce)
      && empty<Tuple>(announce) && empty<Record>(announce);
  }

public:
  enum class Event {
    Runnable,
    Variable,
    Structure
  };

  void clear() {
    trackingTriggered = false;

    insertedRunnables.clear();
    removedRunnables.clear();
    updatedRunnables.clear();
    collectedRunnables.clear();

    aggregatedOptVariables.clear();
    aggregatedVariables.clear();
    aggregatedReadOnlyVariables.clear();

    abstractionStructuresVectors.clear();
    consStructuresVectors.clear();
    tupleStructuresVectors.clear();
    recordStructuresVectors.clear();
  }

public:
  using IdsVector = std::vector<size_t>;

  struct Tracking {
  protected:
    Tracking(VM vm, Event event, size_t announcerThreadId, IdsVector idsVector) :
      event(event), announcerThreadId(announcerThreadId), idsVector(idsVector) {}
  public:
    Event event;
    size_t announcerThreadId;
    IdsVector idsVector;
  };

  using TrackingVector = std::vector<Tracking>;

  struct RunnableTracking : public Tracking {
    explicit RunnableTracking(VM vm, size_t announcerThreadId, IdsVector idsVector,
      RunnableAnnounce announce) :
      Tracking(vm, Event::Runnable, announcerThreadId, idsVector),
      announce(announce) {}

    RunnableAnnounce announce;
  };

  using RunnableTrackingVector = std::vector<RunnableTracking>;

  enum class VariableType {
    All,
    OptVar,
    Variable,
    ReadOnlyVariable
  };

  struct VariableTracking : public Tracking {
    explicit VariableTracking(VM vm, size_t announcerThreadId, IdsVector idsVector,
      VariableAnnounce announce) :
      Tracking(vm, Event::Variable, announcerThreadId, idsVector),
      announce(announce), type(VariableType::All) {}

    VariableAnnounce announce;
    VariableType type;
  };

  using VariableTrackingVector = std::vector<VariableTracking>;

  enum class StructureType {
    All,
    Abstraction,
    Cons,
    Tuple,
    Record
  };

  struct StructureTracking : public Tracking {
    explicit StructureTracking(VM vm, size_t announcerThreadId, IdsVector idsVector,
      StructureAnnounce announce) :
      Tracking(vm, Event::Structure, announcerThreadId, idsVector),
      announce(announce), type(StructureType::All) {}

    StructureAnnounce announce;
    StructureType type;
  };

  using StructureTrackingVector = std::vector<StructureTracking>;

public:
  RunnableTrackingVector& getRunnableTrackingVector() {
    return runnableTrackingVector;
  }

  VariableTrackingVector& getVariableTrackingVector() {
    return variableTrackingVector;
  }

  StructureTrackingVector& getStructureTrackingVector() {
    return structureTrackingVector;
  }

private:
  inline
  bool contains(IdsVector& idsVector, size_t id);

  template<class S>
  inline
  size_t getStructureId(StructureInfo<S> structure);

public:
  void track(VM vm, RunnableTracking tracking) {
    runnableTrackingVector.push_back(tracking);
  }

  void track(VM vm, VariableTracking tracking) {
    variableTrackingVector.push_back(tracking);
  }

  void track(VM vm, StructureTracking tracking) {
    structureTrackingVector.push_back(tracking);
  }

private:
  inline
  bool matchTracking(VM vm, RunnableTracking& tracking,
    RunnableInfo info, RunnableAnnounce announce);

  template<class V>
  inline
  bool matchTracking(VM vm, VariableTracking& tracking,
    VariableInfo<V> info, VariableAnnounce announce);

  template<class V>
  inline
  bool matchTracking(VM vm, VariableTracking& tracking,
    BoundVariableInfo<V> info, VariableAnnounce announce);

  template<class V>
  inline
  bool matchTracking(VM vm, VariableTracking& tracking,
    WaitedVariableInfo<V> info, VariableAnnounce announce);

  template<class S>
  inline
  bool matchTracking(VM vm, StructureTracking tracking,
    StructureInfo<S> info, StructureAnnounce announce);

private:
  inline
  void detectTriggeredRunnableTracking(VM vm, RunnableInfo info, RunnableAnnounce announce) {
    for (auto iter = runnableTrackingVector.begin();
      iter != runnableTrackingVector.end(); ++iter) {
      RunnableTracking tracking = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  inline
  void detectTriggeredRunnableTracking(VM vm, RunnableTracking& tracking,
    RunnablesVector& vector, RunnableAnnounce announce) {
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      RunnableInfo info = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  inline
  void detectTriggeredRunnableTracking(VM vm, RunnableTracking& tracking) {
    detectTriggeredRunnableTracking(vm, tracking,
      insertedRunnables, RunnableAnnounce::Inserted);
    detectTriggeredRunnableTracking(vm, tracking,
      removedRunnables, RunnableAnnounce::Removed);
    detectTriggeredRunnableTracking(vm, tracking,
      updatedRunnables, RunnableAnnounce::Updated);
    detectTriggeredRunnableTracking(vm, tracking,
      collectedRunnables, RunnableAnnounce::Collected);
  }

private:
  template<class V, class VInfo>
  inline
  void detectTriggeredVariableTracking(VM vm, VInfo info, VariableAnnounce announce) {
    for (auto iter = variableTrackingVector.begin();
      iter != variableTrackingVector.end(); ++iter) {
      VariableTracking tracking = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    VariablesVector<V>& vector, VariableAnnounce announce) {
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      VariableInfo<V> info = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    WaitedVariablesVector<V>& vector, VariableAnnounce announce) {
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      WaitedVariableInfo<V> info = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    BoundVariablesVector<V>& vector, VariableAnnounce announce) {
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      BoundVariableInfo<V> info = *iter;
      if (matchTracking(vm, tracking, info, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    VariablesVectors<V>& vectors) {
    detectTriggeredVariableTracking<V>(vm, tracking, vectors.createds, VariableAnnounce::Created);
    detectTriggeredVariableTracking<V>(vm, tracking, vectors.collecteds, VariableAnnounce::Collected);
    detectTriggeredVariableTracking<V>(vm, tracking, vectors.neededs, VariableAnnounce::Needed);
    detectTriggeredVariableTracking<V>(vm, tracking, vectors.waiteds, VariableAnnounce::Waited);
    detectTriggeredVariableTracking<V>(vm, tracking, vectors.bounds, VariableAnnounce::Bound);
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking) {
    if constexpr (std::is_same_v<V, OptVar>) {
      detectTriggeredVariableTracking(vm, tracking, aggregatedOptVariables);
    } else if constexpr (std::is_same_v<V, Variable>) {
      detectTriggeredVariableTracking(vm, tracking, aggregatedVariables);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      detectTriggeredVariableTracking(vm, tracking, aggregatedReadOnlyVariables);
    } else assert(false);
  }


  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking) {
    switch (tracking.type) {
      case VariableType::OptVar:
        detectTriggeredVariableTracking<OptVar>(vm, tracking);
        break;
      case VariableType::Variable:
        detectTriggeredVariableTracking<Variable>(vm, tracking);
        break;
      case VariableType::ReadOnlyVariable:
        detectTriggeredVariableTracking<ReadOnlyVariable>(vm, tracking);
        break;
      case VariableType::All:
        detectTriggeredVariableTracking<OptVar>(vm, tracking);
        detectTriggeredVariableTracking<Variable>(vm, tracking);
        detectTriggeredVariableTracking<ReadOnlyVariable>(vm, tracking);
        break;
      default: assert(false);
    }
  }

private:
  template<class S>
  inline
  void detectTriggeredStructureTracking(VM vm, StructureInfo<S> structure,
    StructureAnnounce announce) {
    for (auto iter = structureTrackingVector.begin();
      iter != structureTrackingVector.end(); ++iter) {
      StructureTracking tracking = *iter;
      if (matchTracking<S>(vm, tracking, structure, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class S>
  inline
  void detectTriggeredStructureTracking(VM vm, StructureTracking& tracking,
    StructuresVector<S>& vector, StructureAnnounce announce) {
    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      StructureInfo<S> structure = *iter;
      if (matchTracking<S>(vm, tracking, structure, announce)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class S>
  inline
  void detectTriggeredStructureTracking(VM vm, StructuresVectors<S>& vectors,
    StructureTracking& tracking) {
    detectTriggeredStructureTracking<S>(vm, tracking,
      vectors.createds, StructureAnnounce::Created);
    detectTriggeredStructureTracking<S>(vm, tracking,
      vectors.collecteds, StructureAnnounce::Collected);
  }

  template<class S>
  inline
  void detectTriggeredStructureTracking(VM vm, StructureTracking& tracking) {
    if constexpr (std::is_same_v<S, Abstraction>) {
      detectTriggeredStructureTracking<S>(vm, abstractionStructuresVectors, tracking);
    } else if constexpr (std::is_same_v<S, Cons>) {
      detectTriggeredStructureTracking<S>(vm, abstractionStructuresVectors, tracking);
    } else if constexpr (std::is_same_v<S, Tuple>) {
      detectTriggeredStructureTracking<S>(vm, abstractionStructuresVectors, tracking);
    } else if constexpr (std::is_same_v<S, Record>) {
      detectTriggeredStructureTracking<S>(vm, abstractionStructuresVectors, tracking);
    } else assert(false);
  }

public:
  bool isTrackingTriggered() {
    return trackingTriggered;
  }

private:
  RunnablesVector insertedRunnables;
  RunnablesVector removedRunnables;
  RunnablesVector updatedRunnables;
  RunnablesVector collectedRunnables;

  VariablesVectors<OptVar> aggregatedOptVariables;
  VariablesVectors<Variable> aggregatedVariables;
  VariablesVectors<ReadOnlyVariable> aggregatedReadOnlyVariables;

  StructuresVectors<Abstraction> abstractionStructuresVectors;
  StructuresVectors<Cons> consStructuresVectors;
  StructuresVectors<Tuple> tupleStructuresVectors;
  StructuresVectors<Record> recordStructuresVectors;

  bool trackingTriggered;
  RunnableTrackingVector runnableTrackingVector;
  VariableTrackingVector variableTrackingVector;
  StructureTrackingVector structureTrackingVector;
};

}

#endif // MOZART_EVENTMANAGER_DECL_H
