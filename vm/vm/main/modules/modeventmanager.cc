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

#include "../mozart.hh"
#include "modeventmanager.hh"

namespace mozart {

namespace builtins {

// About runnables

UnstableNode ModEventManager::buildThreadStateRecord(VM vm, Runnable* runnable) {
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

UnstableNode ModEventManager::buildRunnablesList(VM vm,
  RunnablesVector& runnables) {
  OzListBuilder builder(vm);

  for (auto iter = runnables.begin(); iter != runnables.end(); ++iter) {
    RunnableInfo info = *iter;
    builder.push_back(vm, buildThreadStateRecord(vm, info.runnable));
  }

  return builder.get(vm);
}

UnstableNode ModEventManager::buildRunnablesJournalRecord(VM vm, VirtualMachineEventManager& journal) {
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

// About variables

template<typename V>
void ModEventManager::fillInfo(VM vm, V* variable, size_t& id, std::string& type, bool& isNeeded, bool& isBound, bool& isWaited) {
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
UnstableNode ModEventManager::buildVariableStateRecord(VM vm, V* variable) {
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
UnstableNode ModEventManager::buildVariableStateRecord(VM vm, V* variable, RichNode self, RichNode src) {
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

UnstableNode ModEventManager::buildWaiterStateRecord(VM vm, RichNode waiter) {
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
UnstableNode ModEventManager::buildVariableStateRecord(VM vm, V* variable, RichNode waiter) {
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
UnstableNode ModEventManager::buildVariablesList(VM vm,
  VariablesVector<V>& variables) {
  OzListBuilder builder(vm);

  for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
    VariableInfo<V> info = *iter;
    builder.push_back(vm, buildVariableStateRecord(vm, info.variable));
  }

  return builder.get(vm);
}

template<typename V>
UnstableNode ModEventManager::buildBoundVariablesList(VM vm,
  BoundVariablesVector<V>& variables) {
  OzListBuilder builder(vm);

  for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
    BoundVariableInfo<V> boundVariable = *iter;
    builder.push_back(vm, buildVariableStateRecord(vm,
      boundVariable.variable, boundVariable.self, boundVariable.src));
  }

  return builder.get(vm);
}

template<typename V>
UnstableNode ModEventManager::buildWaitedVariablesList(VM vm,
  WaitedVariablesVector<V>& variables) {
  OzListBuilder builder(vm);

  for (auto iter = variables.begin(); iter != variables.end(); ++iter) {
    WaitedVariableInfo<V> waitedVariable = *iter;
    builder.push_back(vm, buildVariableStateRecord(vm,
      waitedVariable.variable, waitedVariable.waiter));
  }

  return builder.get(vm);
}

template<typename V>
UnstableNode ModEventManager::buildVariablesSubJournalRecord(VM vm, VirtualMachineEventManager& journal) {
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

UnstableNode ModEventManager::buildVariablesJournalRecord(VM vm, VirtualMachineEventManager& journal) {
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

// About structures

template<class S>
UnstableNode ModEventManager::buildStructuresList(VM vm, StructuresVector<S>& vector) {
  OzListBuilder builder(vm);

  for (auto iter = vector.begin(); iter != vector.end(); ++iter) {
    StructureInfo<S> info = *iter;
    // Runnable* author = info.author;
    S* structure = info.structure;

    std::string type;

    if (std::is_same_v<S, Abstraction>) {
      type = "abstraction";
    } else if (std::is_same_v<S, Cons>) {
      type = "cons";
    } else if (std::is_same_v<S, Tuple>) {
      type = "tuple";
    } else if (std::is_same_v<S, Record>) {
      type = "record";
    } else assert(false);

    builder.push_back(vm,
      buildRecord(vm,
        buildArity(vm,
          "structure",
          "id",
          "type"
        ),
        build(vm, structure->getId()),
        build(vm, type.c_str())
      )
    );
  }

  return builder.get(vm);
}

template<class S>
UnstableNode ModEventManager::buildStructuresSubJournalRecord(VM vm, VirtualMachineEventManager& journal) {
  std::string recordName;
  
  if (std::is_same_v<S, Abstraction>) {
    recordName = "abstractionsJournal";
  } else if (std::is_same_v<S, Cons>) {
    recordName = "consJournal";
  } else if (std::is_same_v<S, Tuple>) {
    recordName = "tuplesJournal";
  } else if (std::is_same_v<S, Record>) {
    recordName = "recordsJournal";
  } else assert(false);

  return buildRecord(vm,
    buildArity(vm,
      recordName.c_str(),
      "collected",
      "created"
    ),
    buildStructuresList(vm, journal.getStructures<S>(
      VirtualMachineEventManager::StructureAnnounce::Collected)),
    buildStructuresList(vm, journal.getStructures<S>(
      VirtualMachineEventManager::StructureAnnounce::Created))
  );
}

UnstableNode ModEventManager::buildStructuresJournalRecord(VM vm, VirtualMachineEventManager& journal) {
  return buildRecord(vm,
    buildArity(vm,
      "structuresJournal",
      "abstractions",
      "cons",
      "records",
      "tuples"
    ),
    buildStructuresSubJournalRecord<Abstraction>(vm, journal),
    buildStructuresSubJournalRecord<Cons>(vm, journal),
    buildStructuresSubJournalRecord<Record>(vm, journal),
    buildStructuresSubJournalRecord<Tuple>(vm, journal)
  );
}

// Builtin calls

void ModEventManager::GetJournal::call(VM vm, Out result) {
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

void ModEventManager::Track::call(VM vm, In event, In announce, In announcerId, In idsList) {
  VirtualMachineEventManager& eventManager = vm->getEventManager();
  
  // using Event = VirtualMachineEventManager::Event;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;
  using StructureAnnounce = VirtualMachineEventManager::StructureAnnounce;
  using RunnableTracking = VirtualMachineEventManager::RunnableTracking;
  using VariableTracking = VirtualMachineEventManager::VariableTracking;
  using StructureTracking = VirtualMachineEventManager::StructureTracking;
  using IdsVector = VirtualMachineEventManager::IdsVector;

  using namespace patternmatching;

  size_t announcerThreadId = 0;
  if (matches(vm, announcerId, "none"))
    announcerThreadId = SIZE_MAX;
  else
    announcerThreadId = getArgument<size_t>(vm, announcerId);

  IdsVector ids;
  ozListForEach(vm, idsList, [&ids](size_t id) {
    ids.push_back(id);
  }, "List of integer ids");

  if (matches(vm, event, "runnable")) {
    if (matches(vm, announce, "inserted")) {
      eventManager.track(vm, 
        RunnableTracking(vm, announcerThreadId, ids, RunnableAnnounce::Inserted));
    } else if (matches(vm, announce, "removed")) {
      eventManager.track(vm, 
        RunnableTracking(vm, announcerThreadId, ids, RunnableAnnounce::Removed));
    } else if (matches(vm, announce, "updated")) {
      eventManager.track(vm, 
        RunnableTracking(vm, announcerThreadId, ids, RunnableAnnounce::Updated));
    } else if (matches(vm, announce, "collected")) {
      eventManager.track(vm, 
        RunnableTracking(vm, announcerThreadId, ids, RunnableAnnounce::Collected));
    } else {
      raiseTypeError(vm, "inserted, removed, updated or collected", announce);
    }
  } else if (matches(vm, event, "variable")) {
    if (matches(vm, announce, "created")) {
      eventManager.track(vm, 
        VariableTracking(vm, announcerThreadId, ids, VariableAnnounce::Created));
    } else if (matches(vm, announce, "collected")) {
      eventManager.track(vm, 
        VariableTracking(vm, announcerThreadId, ids, VariableAnnounce::Collected));
    } else if (matches(vm, announce, "needed")) {
      eventManager.track(vm, 
        VariableTracking(vm, announcerThreadId, ids, VariableAnnounce::Needed));
    } else if (matches(vm, announce, "waited")) {
      eventManager.track(vm, 
        VariableTracking(vm, announcerThreadId, ids, VariableAnnounce::Waited));
    } else if (matches(vm, announce, "bound")) {
      eventManager.track(vm, 
        VariableTracking(vm, announcerThreadId, ids, VariableAnnounce::Bound));
    } else {
      raiseTypeError(vm, "created, collected, needed, waited or bound", announce);
    }
  } else if (matches(vm, event, "structure")) {
    if (matches(vm, announce, "created")) {
      eventManager.track(vm, 
        StructureTracking(vm, announcerThreadId, ids, StructureAnnounce::Created));
    } else if (matches(vm, announce, "collected")) {
      eventManager.track(vm, 
        StructureTracking(vm, announcerThreadId, ids, StructureAnnounce::Collected));
    } else {
      raiseTypeError(vm, "created or collected", announce);
    }
  } else {
    raiseTypeError(vm, "runnable, variable or structure", event);
  }
}

}

}
