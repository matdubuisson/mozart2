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

class ModJournal: public Module {
public:
  ModJournal(): Module("Journal") {}

  static inline
  std::string nodeToString(VM vm, RichNode node) {
    auto& config = vm->getPropertyRegistry().config;
    std::basic_stringstream<char> buffer;
    buffer << repr(vm, node, config.printDepth, config.printWidth);
    return buffer.str();
  }

private:
  using RunnablesVector = VirtualMachineJournal::RunnablesVector;
  using RunnableAnnounce = VirtualMachineJournal::RunnableAnnounce;
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
  UnstableNode buildRunnablesJournalRecord(VM vm, VirtualMachineJournal& journal) {
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
  using VariablesVector = VirtualMachineJournal::VariablesVector<V>;
  template<typename V>
  using WaitedVariable = VirtualMachineJournal::WaitedVariable<V>;
  template<typename V>
  using WaitedVariablesVector = VirtualMachineJournal::WaitedVariablesVector<V>;
  template<typename V>
  using VariablesVectors = VirtualMachineJournal::VariablesVectors<V>;
  using VariableAnnounce = VirtualMachineJournal::VariableAnnounce;

  // template<typename V>
  // using VariableToRecordLambda = std::function<UnstableNode(VM, V*)>;
  // template<typename V>
  // using WaitedVariableToRecordLambda = std::function<UnstableNode(VM, WaitedVariable<V>*)>;

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
  UnstableNode buildVariableStateRecord(VM vm, V* variable, RichNode waiter = RichNode(nullptr)) {
    assert(variable != nullptr);

    size_t id;
    std::string type;
    bool isNeeded, isBound, isWaited;

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
  UnstableNode buildVariablesSubJournalRecord(VM vm, VirtualMachineJournal& journal) {
    std::string recordName;
    if constexpr (std::is_same_v<V, OptVar>) {
      recordName = "optVariablesJournal";
    } else if constexpr (std::is_same_v<V, Variable>) {
      recordName = "variablesJournal";
    } else if constexpr (std::is_same_v<V, ReadOnlyVariable>) {
      recordName = "readOnlyVariables";
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
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Bound)),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Collected)),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Created)),
      buildVariablesList(vm, journal.getVariables<V>(VariableAnnounce::Needed)),
      buildWaitedVariablesList(vm, journal.getWaitedVariables<V>())
    );
  }

  static inline
  UnstableNode buildVariablesJournalRecord(VM vm, VirtualMachineJournal& journal) {
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

public:
  class GetJournal: public Builtin<GetJournal> {
  public:
    GetJournal(): Builtin("getJournal") {}

    static void call(VM vm, Out result) {
      VirtualMachineJournal& journal = vm->getJournal();
      
      result = buildRecord(vm,
        buildArity(vm,
          "journal",
          "runnables",
          "variables"
        ),
        buildRunnablesJournalRecord(vm, journal),
        buildVariablesJournalRecord(vm, journal)
      );
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTIONJOURNAL_H