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

void ModEventManager::Track::call(VM vm, In type, In category, In authorId, In victimIds) {
  VirtualMachineEventManager& eventManager = vm->getEventManager();

  using Id = VirtualMachineEventManager::Id;
  using IdsVector = VirtualMachineEventManager::IdsVector;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  using RunnableTracking = VirtualMachineEventManager::RunnableTracking;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;
  using VariableTracking = VirtualMachineEventManager::VariableTracking;

  using namespace patternmatching;

  Id id = 0;
  if (matches(vm, authorId, "none"))
    id = SIZE_MAX;
  else
    id = getArgument<size_t>(vm, authorId);

  IdsVector ids;
  ozListForEach(vm, victimIds, [&ids](Id id) {
    ids.push_back(id);
  }, "List of integer ids");

  if (matches(vm, type, "runnable")) {
    if (matches(vm, category, "created")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Created));
    } else if (matches(vm, category, "updated")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Updated));
    } else if (matches(vm, category, "collected")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Collected));
    } else if (matches(vm, category, "scheduled")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Scheduled));
    } else if (matches(vm, category, "unscheduled")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Unscheduled));
    } else if (matches(vm, category, "rescheduled")) {
      eventManager.track(
        RunnableTracking(id, ids, RunnableAnnounce::Rescheduled));
    } else {
      raiseTypeError(vm,
        "created, updated, collected, scheduled, unscheduled or rescheduled", category);
    }
  } else if (matches(vm, type, "variable")) {
    if (matches(vm, category, "created")) {
      eventManager.track(
        VariableTracking(id, ids, VariableAnnounce::VCreated));
    } else if (matches(vm, category, "collected")) {
      eventManager.track(
        VariableTracking(id, ids, VariableAnnounce::VCollected));
    } else if (matches(vm, category, "needed")) {
      eventManager.track(
        VariableTracking(id, ids, VariableAnnounce::Needed));
    } else if (matches(vm, category, "waited")) {
      eventManager.track(
        VariableTracking(id, ids, VariableAnnounce::Waited));
    } else if (matches(vm, category, "bound")) {
      eventManager.track(
        VariableTracking(id, ids, VariableAnnounce::Bound));
    } else {
      raiseTypeError(vm,
        "created, collected, needed, waited or bound", category);
    }
  } else {
    raiseTypeError(vm, "runnable or variable", type);
  }
}

void ModEventManager::Untrack::call(VM vm, In type, In category, In ruleIndex) {
  VirtualMachineEventManager& eventManager = vm->getEventManager();

  using Id = VirtualMachineEventManager::Id;
  using IdsVector = VirtualMachineEventManager::IdsVector;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  using RunnableTracking = VirtualMachineEventManager::RunnableTracking;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;
  using VariableTracking = VirtualMachineEventManager::VariableTracking;

  using namespace patternmatching;

  Id index = getArgument<Id>(vm, ruleIndex);

  if (matches(vm, type, "runnable")) {
    if (matches(vm, category, "created")) {
      eventManager.untrack(RunnableAnnounce::Created, index);
    } else if (matches(vm, category, "updated")) {
      eventManager.untrack(RunnableAnnounce::Updated, index);
    } else if (matches(vm, category, "collected")) {
      eventManager.untrack(RunnableAnnounce::Collected, index);
    } else if (matches(vm, category, "scheduled")) {
      eventManager.untrack(RunnableAnnounce::Scheduled, index);
    } else if (matches(vm, category, "unscheduled")) {
      eventManager.untrack(RunnableAnnounce::Unscheduled, index);
    } else if (matches(vm, category, "rescheduled")) {
      eventManager.untrack(RunnableAnnounce::Rescheduled, index);
    } else {
      raiseTypeError(vm,
        "created, updated, collected, scheduled, unscheduled or rescheduled", category);
    }
  } else if (matches(vm, type, "variable")) {
    if (matches(vm, category, "created")) {
      eventManager.untrack(VariableAnnounce::VCreated, index);
    } else if (matches(vm, category, "collected")) {
      eventManager.untrack(VariableAnnounce::VCollected, index);
    } else if (matches(vm, category, "needed")) {
      eventManager.untrack(VariableAnnounce::Needed, index);
    } else if (matches(vm, category, "waited")) {
      eventManager.untrack(VariableAnnounce::Waited, index);
    } else if (matches(vm, category, "bound")) {
      eventManager.untrack(VariableAnnounce::Bound, index);
    } else {
      raiseTypeError(vm,
        "created, collected, needed, waited or bound", category);
    }
  } else {
    raiseTypeError(vm, "runnable or variable", type);
  }
}

}

}
