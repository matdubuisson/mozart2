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

#ifndef MOZART_VM_DECL_H
#define MOZART_VM_DECL_H

#include <cstdlib>
#include <forward_list>
#include <atomic>

#include "core-forward-decl.hh"
#include "identifiable-decl.hh"

#include "memmanager.hh"

#include "store-decl.hh"
#include "threadpool-decl.hh"
#include "gcollect-decl.hh"
#include "sclone-decl.hh"
#include "space-decl.hh"
#include "uuid-decl.hh"
#include "vmallocatedlist-decl.hh"

#include "atomtable.hh"
#include "bigintimplem-decl.hh"
#include "coreatoms-decl.hh"
#include "properties-decl.hh"

#include "introspection-decl.hh"

namespace mozart {

///////////////////
// BuiltinModule //
///////////////////

class BuiltinModule {
public:
  /**
   * @param vm The virtual machine
   * @param name The name of the built-in module to invoke
   */
  inline
  BuiltinModule(VM vm, const char* name);

  virtual ~BuiltinModule() {}

  /** Gets the name of the built-in module as an atom */
  atom_t getName() {
    return _name;
  }

  /**
   * Provides access to a built-in module
   * @returns The built-in module as a stable node
   */
  StableNode& getModule() {
    return *_module;
  }
protected:
  /**
   * Inits the built-in module into a protected node
   * @param vm The virtual machine
   * @param module A pointer on a node containing the module (generally unstable)
   */
  template <typename T>
  inline
  void initModule(VM vm, T&& module);
private:
  atom_t _name;
  ProtectedNode _module;
};

////////////////////
// VirtualMachine //
////////////////////

class VirtualMachineEnvironment {
public:
  VirtualMachineEnvironment(): _useDynamicPreemption(false) {}

  VirtualMachineEnvironment(bool useDynamicPreemption):
    _useDynamicPreemption(useDynamicPreemption) {}

  VirtualMachineEnvironment(const VirtualMachineEnvironment&) = delete;

  bool useDynamicPreemption() {
    return _useDynamicPreemption;
  }

  virtual bool testDynamicPreemption() {
    return false;
  }

  virtual bool testDynamicExitRun() {
    return false;
  }

// The following methods assume a single VM per process
public:
  // Must only be used for initialization
  MemoryManager& getSecondMemoryManagerRef() {
    return _secondMemoryManager;
  }

  virtual void withSecondMemoryManager(const std::function<void(MemoryManager&)>& doGC) {
    doGC(_secondMemoryManager);
  }

  virtual void killVM(VM vm, nativeint exitCode, const std::string& reason) {
    std::exit(exitCode);
  }

// Miscellaneous
public:
  virtual UUID genUUID(VM vm) = 0;

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, nativeint value);

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, double value);

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, const std::string& value);

  inline
  virtual void sendOnVMPort(VM from, VMIdentifier to, RichNode value);

  virtual void gCollect(GC gc) {
  }

protected:
  MemoryManager _secondMemoryManager;
private:
  bool _useDynamicPreemption;
};

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

    detectTriggeredVariableTracking(vm, info, announce);

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
    detectTriggeredVariableTracking(vm, info, VariableAnnounce::Bound);
    vector.bounds.push_back(info);
  }

  template<typename V>
  void announceWaitedVariable(VM vm, VariablesVectors<V>& vector, V* variable, RichNode waiter) {
    WaitedVariableInfo<V> info = WaitedVariableInfo(vm, variable, waiter);
    detectTriggeredVariableTracking(vm, info, VariableAnnounce::Waited);
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
  bool contains(IdsVector& idsVector, size_t id) {
    return id != SIZE_MAX && count(idsVector.begin(), idsVector.end(), id) > 0;
  }

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
    RunnableInfo info, RunnableAnnounce announce) {

    if (tracking.announce != announce)
      return false;
    else if (tracking.announcerThreadId != SIZE_MAX
      && tracking.announcerThreadId != info.author->getId())
      return false;
    else if (tracking.idsVector.empty())
      return true;
    else
      return contains(tracking.idsVector, info.runnable->getId());
  }

  template<class V>
  inline
  bool matchTracking(VM vm, VariableTracking& tracking,
    VariableInfo<V> info, VariableAnnounce announce) {

    if (tracking.announce != announce)
      return false;
    else if (tracking.announcerThreadId != SIZE_MAX
      && tracking.announcerThreadId != info.author->getId())
      return false;
    else if (tracking.idsVector.empty())
      return true;
    else
      return contains(tracking.idsVector, info.variable->getId());
  }

  template<class S>
  inline
  bool matchTracking(VM vm, StructureTracking tracking,
    StructureInfo<S> info, StructureAnnounce announce) {

    if (tracking.announce != announce)
      return false;
    else if (tracking.announcerThreadId != SIZE_MAX
      && tracking.announcerThreadId != info.author->getId())
      return false;
    else if (tracking.idsVector.empty())
      return true;
    else
      return contains(tracking.idsVector, info.structure->getId())
        || contains(tracking.idsVector, info.structure->getKindId());
  }
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
  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableInfo<V> info, VariableAnnounce announce) {
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
      WaitedVariableInfo info = *iter;
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
      BoundVariableInfo info = *iter;
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

class VirtualMachine {
public:
  enum ExecutionMode {
    Normal = 0,
    LimitedSchedules,
    LimitedOperations,
    OperationByOperation, // Without system threads by definition: they are responsible to advance the VM
    FollowingJournal
  };

  /**
   * Exit codes allows the virtual machine to process runnable entities differently when they exit :
   * - recNeverInvokeAgain: ....
   * - recInvokeAgainNow: ....
   * - recInvokeAgainLater: ....
   */
   enum RunExitCode {
    recNeverInvokeAgain, recInvokeAgainNow, recInvokeAgainLater
  };

  typedef std::pair<RunExitCode, std::int64_t> run_return_type;

  using StreamsQueue = std::queue<RichNode>;
private:
  struct AlarmRecord {
    AlarmRecord(std::int64_t expiration, StableNode* wakeable):
      expiration(expiration), wakeable(wakeable) {}

    std::int64_t expiration;
    StableNode* wakeable;
  };
public:
  /**
   * Inits the virtual machine, basically it means several parts to initiate :
   * 0) Garbage collector
   * 1) Space cloner
   * 2) Memory manager
   * 3) Top level space
   * 4) Built-in modules dictionary
   * 5) VM configuration TO-COMPLETE
   * @param environment The VM environment defines the memory manager and the preemption mode
   * @param options The VM options define the heap allocation boundaries
   */
  inline
  VirtualMachine(VirtualMachineEnvironment& environment,
                 VirtualMachineOptions options);

  /** The virtual machine cannot be copied */
  VirtualMachine(const VirtualMachine& src) = delete;

  /** Deconstructor = doCleanup() */
  inline
  ~VirtualMachine();

  /**
   * Allocates memory using VM's memory manager
   * @param size The size of the new memory block
   */
  void* malloc(size_t size) {
    return memoryManager.malloc(size);
  }

   /**
   * Frees memory using VM's memory manager
   * @param ptr The address of the memory block previously allocated
   * @param size The size of the new memory block
   */
  void free(void* ptr, size_t size) {
    memoryManager.free(ptr, size);
  }

  /**
   * Allocates a static array able to contain N elements of type T
   * T is generally UnstableNode, RichNode, nativeint or UnstableField
   * @param size The number of elements of size sizeof(T)
   * @note So the total allocated memory is size * sizeof(T)
   */
  template <class T>
  StaticArray<T> newStaticArray(size_t size) {
    void* memory = malloc(size * sizeof(T));
    return StaticArray<T>(static_cast<T*>(memory), size);
  }

  /**
   * Deallocates a pre-allocated static array
   * @param size The number of elements of size * sizeof(T)
   * @note So the total allocated memory is size * sizeof(T)
   */
  template <class T>
  void deleteStaticArray(StaticArray<T> array, size_t size) {
    void* memory = static_cast<void*>((T*) array);
    free(memory, size * sizeof(T));
  }

  /**
   * Pushs a new cleaning task on the clean-up list
   * @param handler Simple void procedure taking the VM as argument
   */
  void onCleanup(const VMCleanupProc& handler) {
    onCleanup(new (this) VMCleanupListNode, handler);
  }

  /**
   * Pushs a new cleaning task on the clean-up list
   * @param node Cleanup node containing a clean-up task (handler) and the next cleanup node
   * @param handler Simple void procedure taking the VM as argument
   */
  void onCleanup(VMCleanupListNode* node, const VMCleanupProc& handler) {
    node->handler = handler;
    node->next = _cleanupList;
    _cleanupList = node;
  }

  void scheduleThread(bool isSystem = false);

  void scheduleSystemThreads();

  /**
   * Runs the virtual machine
   * @returns The virtual machine's exit code
   * @note This is a main procedure that is basically the scheduler orchestrating threads and internal entities as the garbage collector for example
   */
  run_return_type run();

  /**
   * Tests if the threads have to be preempted (for example running the garbage collector)
   * @returns true if the threads should be preempted
   */
  inline
  bool testPreemption();

  /** Gets the thread-pool of the VM */
  ThreadPool& getThreadPool() { return threadPool; }

  /** Get the memory manager of the VM */
  MemoryManager& getMemoryManager() {
    return memoryManager;
  }

  /** Gets the heap size of the VM from property registry */
  size_t getHeapSize() {
    return getPropertyRegistry().config.heapSize;
  }

  /**
   * Adjusts the heap size according to property registry and using the garbage collector
   * @note
   *   It uses a doubling size approach for increasing or decreasing the heap size
   *   The wished new heap size is based on the garbage collector's threshold
   *   and is bound by a minimal and maximal heap size from the property registry
   *   Once the size is modified, the garbage collector is requested to modify the heap according to the new size
   */
  inline
  void adjustHeapSize();

  /** Gets global exception mechanism */
  GlobalExceptionMechanism& getGlobalExceptionMechanism() {
    return exceptionMechanism;
  }

  /** Gets the top level space */
  Space* getTopLevelSpace() {
    return _topLevelSpace;
  }

  /** Gets the current space */
  Space* getCurrentSpace() {
    return _currentSpace;
  }

  /** Gets the current thread (runnable) */
  Runnable* getCurrentThread() {
    return _currentThread;
  }

  /** Tells if the current space is on top level */
  bool isOnTopLevel() {
    return _isOnTopLevel;
  }

  /** Tells if a current thread containing an intermediate state is available */
  bool isIntermediateStateAvailable() {
    return _currentThread != nullptr;
  }

  /**
   * Gets the intermediate state of the current thread
   * @note A current thread has to be available isIntermediateStateAvailable() has to be true
   */
  IntermediateState& getIntermediateState() {
    assert(isIntermediateStateAvailable());
    return _currentThread->getIntermediateState();
  }

  /**
   * Sets a new current space
   * @param space The new space (it might be on top level)
   */
  inline
  void setCurrentSpace(Space* space);

  /**
   * Clones the space using VM's space cloner
   * @param space The space to clone
   * @returns The cloned space
   */
  inline
  Space* cloneSpace(Space* space);

  /** Gets the virtual machine environment as reference */
  VirtualMachineEnvironment& getEnvironment() {
    return environment;
  }

public:
  /**
   * Registers a new built-in modulo into the built-in modules dictionary owned by the VM
   * @param module A shared pointer on the module
   */
  inline
  void registerBuiltinModule(const std::shared_ptr<BuiltinModule>& module);

  /**
   * Finds the built-in module with a lookup in the built-in modules dictionary
   * @param name A referenced reference of an object of type T (generally UnstableNode)
   * @note If the module cannot be found a run-time error is raised
   */
  template <typename T>
  inline
  UnstableNode findBuiltinModule(T&& name);

  /**
   * Finds the built-in from a specified built-in module by performing a lookup
   * @param moduleName The built-in module name
   * @param builtinName The built-in name
   */
  template <typename T, typename U>
  inline
  UnstableNode findBuiltin(T&& moduleName, U&& builtinName);

public:
  /** Gets property registry */
  PropertyRegistry& getPropertyRegistry() {
    return _propertyRegistry;
  }

  /**
   * Generates a new UUID from the VM environment
   * @returns The generated UUID
   */
  inline
  UUID genUUID();

  /**
   * Gets atomically the reference time
   * @returns The reference time
   */
  std::int64_t getReferenceTime() {
    return _referenceTime.load(std::memory_order_acquire);
  }

  /**
   * Inserts a new alarm in the list sorted by expiration according to the reference time
   * @param delay The delay in milliseconds TO-COMPLETE : Checks it true
   * @param wakeable A reference on the alarm as a stable node
   */
  inline
  void setAlarm(std::int64_t delay, StableNode* wakeable);

  /** Gets the pickle types record */
  StableNode* getPickleTypesRecord() {
    return _pickleTypesRecord;
  }

public:
  /**
   * Converts a native int to a big int
   * @param value A native int
   * @returns The converted value to big int
   */
  inline
  std::shared_ptr<BigIntImplem> newBigIntImplem(nativeint value);

public:
  CoreAtoms coreatoms;

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const char* data) {
    return atomTable.get(this, data);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param length The length to read in data
   * @param data The atom name
   * @returns An atom
   */
  atom_t getAtom(size_t length, const char* data) {
    return atomTable.get(this, length, data);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const BaseLString<char>& data) {
    return atomTable.get(this, data.length, data.string);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const std::string& data) {
    return atomTable.get(this, data.length(), data.c_str());
  }

  /**
   * Gets an unique name from the atom table by its name
   * @param data The unique name
   * @returns An unique name
   */
  unique_name_t getUniqueName(const char* data) {
    return atomTable.getUniqueName(this, data);
  }

  /**
   * Gets an unique name from the atom table by its name
   * @param length The length to read in data
   * @param data The unique name
   * @returns An unique name
   */
  unique_name_t getUniqueName(size_t length, const char* data) {
    return atomTable.getUniqueName(this, length, data);
  }
public:
  Introspection& getIntrospection() {
    return introspection;
  }

  VirtualMachineEventManager& getEventManager() {
    return eventManager;
  }
public:
  /**
   * Protects a node from the garbage collector
   * @param node The node to protect
   * @returns A reference-counted ref to that node
   */
  template <typename T>
  inline
  ProtectedNode protect(T&& node);
public:
  // Influence from the external world


  /** Requests preemption of the current thread TO-COMPLETE : check if 100% correct */
  void requestPreempt() {
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  /** Requests virtual machine run termination */
  void requestExitRun() {
    // The order of these two operations *is* important
    _exitRunRequestedNot.clear(std::memory_order_release);
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  /** Requests VM's garbage collector to be run */
  void requestGC() {
    // The order of these two operations *is* important
    _gcRequestedNot.clear(std::memory_order_release);
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  void enableGC() {
    _gcEnabled = true;
  }

  void disableGC() {
    _gcEnabled = false;
  }

  bool isGCReady() {
    return _gcReady;
  }

  bool isGCDone() {
    return _gcDone;
  }

  /**
   * Sets the reference time
   * @param value The new time in milliseconds TO-COMPLETE : check unit
   */
  void setReferenceTime(std::int64_t value) {
    _referenceTime.store(value, std::memory_order_release);
  }
public:
  void setExecutionMode(ExecutionMode mode, uint64_t counter = SIZE_MAX) {
    _executionMode = mode;
    _executionCounter = counter;
  }

  void resetExecutionMode() {
    _executionMode = Normal;
    _executionCounter = SIZE_MAX;
  }

  void updateExecutionMode(size_t nOperations) {
    _schedulesCounter++;
    _operationsCounter += nOperations;

    if (_currentThread->getPriority() == tpSystem) {
      _systemSchedulesCounter++;
      _systemOperationsCounter += nOperations;
      return;
    }

    switch (_executionMode) {
      case LimitedSchedules: {
        if (_executionCounter > 0) _executionCounter--;
        break;
      }
      case LimitedOperations: {
        if (_executionCounter > nOperations) _executionCounter -= nOperations;
        else _executionCounter = 0;
        break;
      }
      case OperationByOperation:
      case FollowingJournal:
      case Normal:
      default: break;
    }

    if (_executionCounter == 0)
      resetExecutionMode();
  }

  size_t getMaxOperationsExecutionMode() {
    assert(_currentThread != nullptr);
    if (_currentThread->getPriority() == tpSystem)
      return SIZE_MAX;

    switch (_executionMode) {
      case LimitedOperations: return _executionCounter;
      case OperationByOperation: return 1;
      case LimitedSchedules:
      case FollowingJournal:
      case Normal:
      default: return SIZE_MAX;
    }
  }

  ExecutionMode getExecutionMode() {
    return _executionMode;
  }

  size_t getExecutionCounter() {
    return _executionCounter;
  }

  bool testLimitedOperationsExecutionMode() {
    return _executionMode == LimitedOperations;
  }

  bool testOperationByOperationExecutionMode() {
    return _executionMode == OperationByOperation;
  }

  bool testEarlyPreemptionRequested() {
    assert(_currentThread != nullptr);
    return _currentThread->isPreemptible()
      && _currentThread->getPriority() != tpSystem
      && eventManager.isTrackingTriggered();
  }

private:
  /** Checks if preemption is requested and clears the flag */
  bool testAndClearPreemptRequested() {
    return !_preemptRequestedNot.test_and_set(std::memory_order_acquire);
  }

  /** Checks if termination is requested and clears the flag */
  bool testAndClearExitRunRequested() {
    return !_exitRunRequestedNot.test_and_set(std::memory_order_acquire);
  }

  /** Checks if garbage collector is requested and clears the flag */
  bool testAndClearGCRequested() {
    return !_gcRequestedNot.test_and_set(std::memory_order_acquire);
  }
private:
  friend class GarbageCollector;
  friend class SpaceCloner;
  friend class Runnable;
  friend class GlobalNode;
  friend class Introspection;

  friend void* ::operator new (size_t size, mozart::VM vm);
  friend void* ::operator new[] (size_t size, mozart::VM vm);

  /**
   * Gets memory from the pre-allocated memory of VM's memory manager
   * @param size The amount of memory to alloc
   * @returns A pointer on the allocated block
   */
  void* getMemory(size_t size) {
    return memoryManager.getMemory(size);
  }

  /** Initializes the coreatomes from VM's atom table */
  inline
  void initialize();

  /**
   * Using a second memory manager from VM's environment
   * it runs the garbage collector
   */
  inline
  void doGC();

  /**
   * Updates the top level space if necessary
   * Prepares all alive threads to a garbage collection
   * @param gr The garbage collector
   */
  inline
  void beforeGR(GR gr);

  /**
   * Updates the top level space if necessary
   * Prepares all alive threads to return to normal after garbage collection
   * @param gr The garbage collector
   */
  inline
  void afterGR(GR gr);

  /**
   * To process with the garbage collection it does several steps :
   * 0) Swaps a the second memory manager and initializes it
   * 1) Reinitializes the VM (atom table, alive threads list and alarms)
   * 2) Copies the top level space
   * 3) Reinitializes the built-in modules
   * 4) Runs the garbage collector on property registry
   * 5) Runs the garbage collector on the thread pool
   * 6) Runs the garbage collector on the protected nodes
   * 7) Copies the alarms in the new alarms list
   * 8) Copies the types record
   * 9) Runs the garbage collector on the VM's environment
   * @param gc The garbage collector
   * @param secondMemoryManager The new memory manager
   */
  inline
  void startGC(GC gc, MemoryManager& secondMemoryManager);

  /**
   * Thanks to smart pointers, detects the protected nodes non-referenced anymore
   * and so deletes them
   * @param gc The garbage collector
   */
  inline
  void gcProtectedNodes(GC gc);

  /**
   * Gets the cleanup list and sets the internal reference to null
   * @returns A cleanup list
   */
  inline
  VMCleanupListNode* acquireCleanupList();

  /**
   * Runs each handler of the cleanup list one by one on the virtual machine
   * @param cleanupList A list of VMCleanupListNode representing the handlers
   */
  inline
  void doCleanup(VMCleanupListNode* cleanupList);

  /** Runs each handler of the cleanup list one by one on the virtual machine */
  void doCleanup() {
    doCleanup(acquireCleanupList());
  }

  ThreadPool threadPool;
  AtomTable atomTable;
  GlobalNode* rootGlobalNode;

  VirtualMachineEnvironment& environment;
  VirtualMachineEventManager eventManager;

  MemoryManager memoryManager;

  GlobalExceptionMechanism exceptionMechanism;

  Space* _topLevelSpace;
  Space* _currentSpace;
  Runnable* _currentThread;
  bool _isOnTopLevel;
  bool _gcEnabled, _gcReady, _gcDone;

  NodeDictionary* _builtinModules;
  PropertyRegistry _propertyRegistry;

  RunnableList threads;
  StreamsQueue streams;

  VMCleanupListNode* _cleanupList;

  GarbageCollector gc;
  SpaceCloner sc;

  Introspection introspection;

  VMAllocatedList<AlarmRecord> _alarms;
  StableNode* _pickleTypesRecord;
  std::forward_list<std::weak_ptr<StableNode*>> _protectedNodes;

  // Flags set externally for preemption etc.
  // TODO Use atomic data types
  bool _envUseDynamicPreemption;
  std::atomic_flag _preemptRequestedNot;
  std::atomic_flag _exitRunRequestedNot;
  std::atomic_flag _gcRequestedNot;
  std::atomic<std::int64_t> _referenceTime;

  /**
   * @brief Flow execution control
   * No atomic variables as only the POSIX thread running the scheduler should be able
   * (through a builtin module) to manipulate this state and for performance reasons as
   * these counters will be updated very very often.
   */
  ExecutionMode _executionMode;
  size_t _executionCounter;
  size_t _schedulesCounter;
  size_t _operationsCounter;
  size_t _systemSchedulesCounter;
  size_t _systemOperationsCounter;

  // During GC, we need a SpaceRef version of the top-level space
  SpaceRef _topLevelSpaceRef;
};

}

#endif // MOZART_VM_DECL_H
