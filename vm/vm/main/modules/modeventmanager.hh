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

#ifndef MOZART_MODJOURNAL_H
#define MOZART_MODJOURNAL_H

#include "../mozartcore.hh"
#include "../introspection.hh"
#include "../emulate.hh"

#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModEventManager: public Module {
public:
  ModEventManager(): Module("EventManager") {}

  static inline
  std::string nodeToString(VM vm, RichNode node) {
    auto& config = vm->getPropertyRegistry().config;
    std::basic_stringstream<char> buffer;
    buffer << repr(vm, node, config.printDepth, config.printWidth);
    return buffer.str();
  }

private:
  using RunnablesVector = VirtualMachineEventManager::RunnablesVector;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  // using RunnableToRecordLambda = std::function<UnstableNode(VM, Runnable*)>;

  static inline
  UnstableNode buildThreadStateRecord(VM vm, Runnable* runnable) {
    UnstableNode id = build(vm, runnable->getId());
    UnstableNode kindId = build(vm, runnable->getKindId());
    UnstableNode generationId = build(vm, runnable->getGenerationId());
    UnstableNode isRunnable = build(vm, runnable->isRunnable());
    UnstableNode isTerminated = build(vm, runnable->isTerminated());
    UnstableNode isDead = build(vm, runnable->isDead());
    UnstableNode isPreempted = build(vm, runnable->isPreempted());
    UnstableNode isPreemptible = build(vm, runnable->isPreemptible());

    UnstableNode type;
    UnstableNode priority;

    if (Thread* thread = dynamic_cast<Thread*>(runnable)) {
      type = build(vm, "thread");
      switch (thread->getPriority()) {
        case tpLow: priority = build(vm, "low"); break;
        case tpMiddle: priority = build(vm, "medium"); break;
        case tpHi: priority = build(vm, "high"); break;
        case tpSystem: priority = build(vm, "system"); break;
        default: assert(false);
      }
    } else {
      type = build(vm, "runnable");
      priority = build(vm, "none");
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
        "terminated",
        "type"
      ),
      isDead,
      generationId,
      id,
      kindId,
      isPreempted,
      isPreemptible,
      priority,
      isRunnable,
      isTerminated,
      type
    );
  }

  static inline
  UnstableNode buildRunnablesList(VM vm,
    RunnablesVector& runnables) {
    OzListBuilder builder(vm);

    for (auto iter = runnables.begin(); iter != runnables.end(); ++iter) {
      Runnable* runnable = static_cast<Runnable*>(*iter);
      builder.push_back(vm, buildThreadStateRecord(vm, runnable));
    }

    return builder.get(vm);
  }

  static inline
  UnstableNode buildRunnablesJournalRecord(VM vm, VirtualMachineEventManager& journal) {
    return buildRecord(vm,
      buildArity(vm,
        "runnablesJournal",
        "collected",
        "inserted",
        "removed",
        "updated"
      ),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Collected)),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Inserted)),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Removed)),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Updated))
    );
  }

private:
  template<typename V>
  using VariablesVector = VirtualMachineEventManager::VariablesVector<V>;
  template<typename V>
  using BoundVariable = VirtualMachineEventManager::BoundVariable<V>;
  template<typename V>
  using BoundVariablesVector = VirtualMachineEventManager::BoundVariablesVector<V>;
  template<typename V>
  using WaitedVariable = VirtualMachineEventManager::WaitedVariable<V>;
  template<typename V>
  using WaitedVariablesVector = VirtualMachineEventManager::WaitedVariablesVector<V>;
  template<typename V>
  using VariablesVectors = VirtualMachineEventManager::VariablesVectors<V>;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;

  // template<typename V>
  // using VariableToRecordLambda = std::function<UnstableNode(VM, V*)>;
  // template<typename V>
  // using WaitedVariableToRecordLambda = std::function<UnstableNode(VM, WaitedVariable<V>*)>;

  template<typename V>
  static inline
  void fillInfo(VM vm, V* variable, size_t& id, std::string& type, bool& isNeeded, bool& isBound, bool& isWaited) {
    if constexpr (std::is_same_v<V, OptVar>) {
      OptVar* v = static_cast<OptVar*>(variable);
      id = v->getId();
      type = "optVariable";
      isNeeded = isBound = isWaited = false;
    } else if constexpr (std::is_same_v<V, Variable>) {
      Variable* v = static_cast<Variable*>(variable);
      id = v->getId();
      type = "variable";
      isNeeded = v->isNeeded(vm);
      isBound = v->isBound(vm);
      isWaited = v->isWaited(vm);
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      ReadOnlyVariable* v = static_cast<ReadOnlyVariable*>(variable);
      id = v->getId();
      type = "readOnlyVariable";
      isNeeded = v->isNeeded(vm);
      isBound = v->isBound(vm);
      isWaited = v->isWaited(vm);
    } else assert(false);
  }

  template<typename V>
  static inline
  UnstableNode buildVariableStateRecord(VM vm, V* variable) {
    assert(variable != nullptr);

    size_t id;
    std::string type;
    bool isNeeded, isBound, isWaited;
    fillInfo<V>(vm, variable, id, type, isNeeded, isBound, isWaited);

    return buildRecord(vm,
      buildArity(vm,
        "variable",
        "id",
        "isBound",
        "isNeeded",
        "isWaited",
        "type"
      ),
      build(vm, id),
      build(vm, isBound),
      build(vm, isNeeded),
      build(vm, isWaited),
      build(vm, type.c_str())
    );
  }

  template<typename V>
  static inline
  UnstableNode buildVariableStateRecord(VM vm, V* variable, RichNode self, RichNode src) {
    assert(variable != nullptr);

    size_t id;
    std::string type;
    bool isNeeded, isBound, isWaited;
    fillInfo<V>(vm, variable, id, type, isNeeded, isBound, isWaited);

    return buildRecord(vm,
      buildArity(vm,
        "variable",
        "destinationNodeId",
        "id",
        "isBound",
        "isNeeded",
        "isWaited",
        "sourceNodeId",
        "type"
      ),
      build(vm, SIZE_MAX),
      build(vm, id),
      build(vm, isBound),
      build(vm, isNeeded),
      build(vm, isWaited),
      build(vm, src.getId()),
      build(vm, type.c_str())
    );
  }

  static inline
  UnstableNode buildWaiterStateRecord(VM vm, RichNode waiter) {
    if (waiter.isNullNode()) {
      return build(vm, "none");
    } else if (waiter.is<ReifiedThread>()) {
      Runnable* runnable = getArgument<Runnable*>(vm, waiter);
      return buildThreadStateRecord(vm, runnable);
    } else if (waiter.is<OptVar>()) {
      OptVar v = Accessor<OptVar>::get(waiter.value());
      return buildVariableStateRecord<OptVar>(vm, &v);
    } else if (waiter.is<Variable>()) {
      Variable v = Accessor<Variable>::get(waiter.value());
      return buildVariableStateRecord<Variable>(vm, &v);
    } else if (waiter.is<ReadOnlyVariable>()) {
      ReadOnlyVariable v = Accessor<ReadOnlyVariable>::get(waiter.value());
      return buildVariableStateRecord<ReadOnlyVariable>(vm, &v);
    } else {
      return build(vm, nodeToString(vm, waiter).c_str());
    }
  }


  template<typename V>
  static inline
  UnstableNode buildVariableStateRecord(VM vm, V* variable, RichNode waiter) {
    assert(variable != nullptr);

    size_t id;
    std::string type;
    bool isNeeded, isBound, isWaited;
    fillInfo<V>(vm, variable, id, type, isNeeded, isBound, isWaited);

    return buildRecord(vm,
      buildArity(vm,
        "variable",
        "id",
        "isBound",
        "isNeeded",
        "isWaited",
        "type",
        "waiter"
      ),
      build(vm, id),
      build(vm, isBound),
      build(vm, isNeeded),
      build(vm, isWaited),
      build(vm, type.c_str()),
      buildWaiterStateRecord(vm, waiter)
    );
  }

  template<typename V>
  static inline
  UnstableNode buildVariablesList(VM vm,
    VariablesVector<V>& variables) {
    OzListBuilder builder(vm);

    for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
      V* variable = static_cast<V*>(*iter);
      builder.push_back(vm, buildVariableStateRecord(vm, variable));
    }

    return builder.get(vm);
  }

  template<typename V>
  static inline
  UnstableNode buildBoundVariablesList(VM vm,
    BoundVariablesVector<V>& variables) {
    OzListBuilder builder(vm);

    for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
      BoundVariable<V> boundVariable = static_cast<BoundVariable<V>>(*iter);
      builder.push_back(vm, buildVariableStateRecord(vm,
        boundVariable.variable, boundVariable.self, boundVariable.src));
    }

    return builder.get(vm);
  }

  template<typename V>
  static inline
  UnstableNode buildWaitedVariablesList(VM vm,
    WaitedVariablesVector<V>& variables) {
    OzListBuilder builder(vm);

    for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
      WaitedVariable<V> waitedVariable = static_cast<WaitedVariable<V>>(*iter);
      builder.push_back(vm, buildVariableStateRecord(vm,
        waitedVariable.variable, waitedVariable.waiter));
    }

    return builder.get(vm);
  }

  template<typename V>
  static inline
  UnstableNode buildVariablesSubJournalRecord(VM vm, VirtualMachineEventManager& journal) {
    std::string recordName;
    if constexpr (std::is_same_v<V, OptVar>) {
      recordName = "optVariablesJournal";
    } else if constexpr (std::is_same_v<V, Variable>) {
      recordName = "variablesJournal";
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      recordName = "readOnlyVariablesJournal";
    } else assert(false);

    return buildRecord(vm,
      buildArity(vm,
        recordName.c_str(),
        "bound",
        "collected",
        "created",
        "needed",
        "waited"
      ),
      buildBoundVariablesList(vm, journal.getBoundVariables<V>()),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Collected)),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Created)),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Needed)),
      buildWaitedVariablesList(vm, journal.getWaitedVariables<V>())
    );
  }

  static inline
  UnstableNode buildVariablesJournalRecord(VM vm, VirtualMachineEventManager& journal) {
    return buildRecord(vm,
      buildArity(vm,
        "variablesJournal",
        "optVariables",
        "readOnlyVariables",
        "variables"
      ),
      buildVariablesSubJournalRecord<OptVar>(vm, journal),
      buildVariablesSubJournalRecord<ReadOnlyVariable>(vm, journal),
      buildVariablesSubJournalRecord<Variable>(vm, journal)
    );
  }

  static inline
  UnstableNode buildStructuresList(VM vm, VirtualMachineEventManager::StructuresVector& vector) {
    OzListBuilder builder(vm);

    for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
      VirtualMachineEventManager::Structure structure = *iter;
      Runnable* runnable = structure.runnable;
      RichNode node = structure.node;

      std::string type;
      size_t id = SIZE_MAX;
      size_t hash = SIZE_MAX;

      if (node.is<Abstraction>()) {
        type = "abstraction";
      } else if (node.is<Cons>()) {
        type = "cons";
        TypedRichNode<Cons> typedNode = node.as<Cons>();
        id = typedNode.getId();
      } else if (node.is<Tuple>()) {
        type = "tuple";
      } else if (node.is<Record>()) {
        type = "record";
      } else assert(false);

      builder.push_back(vm,
        buildRecord(vm,
          buildArity(vm,
            "structure",
            "hash",
            "id",
            "repr",
            "type"
          ),
          hash,
          id,
          nodeToString(vm, node).c_str(),
          type.c_str()
        )
      );
    }

    return builder.get(vm);
  }

  static inline
  UnstableNode buildStructuresJournalRecord(VM vm, VirtualMachineEventManager& journal) {
    return buildRecord(vm,
      buildArity(vm,
        "structuresJournal",
        "collected",
        "created"
      ),
      buildStructuresList(vm, journal.getStructures(
        VirtualMachineEventManager::StructureAnnounce::Collected)),
      buildStructuresList(vm, journal.getStructures(
        VirtualMachineEventManager::StructureAnnounce::Created))
    );
  }

public:
  class GetJournal: public Builtin<GetJournal> {
  public:
    GetJournal(): Builtin("getJournal") {}

    static void call(VM vm, Out result) {
      VirtualMachineEventManager& journal = vm->getEventManager();
      
      result = buildRecord(vm,
        buildArity(vm,
          "journal",
          "runnables",
          "structures",
          "variables"
        ),
        buildRunnablesJournalRecord(vm, journal),
        buildStructuresJournalRecord(vm, journal),
        buildVariablesJournalRecord(vm, journal)
      );
    }
  };

  // class GetAggregatedJournal: public Builtin<GetAggregatedJournal> {
  // public:
  //   GetAggregatedJournal(): Builtin("getAggregatedJournal") {}

  //   static void call(VM vm, Out result) {
  //     VirtualMachineEventManager& journal = vm->getEventManager();
      
  //     result = buildRecord(vm,
  //       buildArity(vm,
  //         "journal",
  //         "runnables",
  //         "structures",
  //         "variables"
  //       ),
  //       buildRunnablesJournalRecord(vm, journal),
  //       buildStructuresJournalRecord(vm, journal),
  //       buildVariablesJournalRecord(vm, journal)
  //     );
  //   }
  // };


public:
  class AddTracking: public Builtin<AddTracking> {
  public:
    AddTracking(): Builtin("addTracking") {}

    static void call(VM vm, In event, In announce/*, In identifier*/) {
      VirtualMachineEventManager& eventManager = vm->getEventManager();
      
      using Event = VirtualMachineEventManager::Event;
      using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
      using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;
      using StructureAnnounce = VirtualMachineEventManager::StructureAnnounce;


      using namespace patternmatching;

      // size_t id;
      // if (matches(vm, identifier, "none")) {
      //   id = SIZE_MAX;
      // } else {
      //   id = getArgument<size_t>(vm, identifier);
      // }

      if (matches(vm, event, "runnable")) {
        if (matches(vm, announce, "inserted")) {
          eventManager.addTracking(Event::Runnable, RunnableAnnounce::Inserted);
        } else if (matches(vm, announce, "removed")) {
          eventManager.addTracking(Event::Runnable, RunnableAnnounce::Removed);
        } else if (matches(vm, announce, "updated")) {
          eventManager.addTracking(Event::Runnable, RunnableAnnounce::Updated);
        } else if (matches(vm, announce, "collected")) {
          eventManager.addTracking(Event::Runnable, RunnableAnnounce::Collected);
        } else {
          raiseTypeError(vm, "inserted, removed, updated or collected", announce);
        }
      } else if (matches(vm, event, "variable")) {
        if (matches(vm, announce, "created")) {
          eventManager.addTracking(Event::Variable, VariableAnnounce::Created);
        } else if (matches(vm, announce, "collected")) {
          eventManager.addTracking(Event::Variable, VariableAnnounce::Collected);
        } else if (matches(vm, announce, "needed")) {
          eventManager.addTracking(Event::Variable, VariableAnnounce::Needed);
        } else if (matches(vm, announce, "waited")) {
          eventManager.addTracking(Event::Variable, VariableAnnounce::Waited);
        } else if (matches(vm, announce, "bound")) {
          eventManager.addTracking(Event::Variable, VariableAnnounce::Bound);
        } else {
          raiseTypeError(vm, "created, collected, needed, waited or bound", announce);
        }
      } else if (matches(vm, event, "structure")) {
        if (matches(vm, announce, "created")) {
          eventManager.addTracking(Event::Structure, StructureAnnounce::Created);
        } else if (matches(vm, announce, "collected")) {
          eventManager.addTracking(Event::Structure, StructureAnnounce::Collected);
        } else {
          raiseTypeError(vm, "created or collected", announce);
        }
      } else {
        raiseTypeError(vm, "runnable, variable or structure", event);
      }
    }
  };

  class TrackingTriggered: public Builtin<TrackingTriggered> {
  public:
    TrackingTriggered(): Builtin("trackingTriggered") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getEventManager().trackingTriggered());
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTIONJOURNAL_H