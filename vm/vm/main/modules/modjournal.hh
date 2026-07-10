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

private:
  using RunnablesVector = VirtualMachineJournal::RunnablesVector;
  using RunnableAnnounce = VirtualMachineJournal::RunnableAnnounce;
  using RunnableToRecordLambda = std::function<UnstableNode(VM, Runnable*)>;

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
    RunnablesVector& runnables,
    RunnableToRecordLambda lambda) {
    OzListBuilder builder(vm);

    for (auto iter = runnables.begin(); iter != runnables.end(); ++iter) {
      Runnable* runnable = static_cast<Runnable*>(*iter);
      builder.push_back(vm, lambda(vm, runnable));
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
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Collected),
        buildThreadStateRecord),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Inserted),
        buildThreadStateRecord),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Removed),
        buildThreadStateRecord),
      buildRunnablesList(vm, journal.getRunnables(RunnableAnnounce::Updated),
        buildThreadStateRecord)
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
          "runnables"
        ),
        buildRunnablesJournalRecord(vm, journal)
      );
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTIONJOURNAL_H