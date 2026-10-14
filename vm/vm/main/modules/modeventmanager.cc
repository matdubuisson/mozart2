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

void ModEventManager::Track::call(VM vm, In event, In announce, In id, In idsList) {
  VirtualMachineEventManager& eventManager = vm->getEventManager();

  using Id = VirtualMachineEventManager::Id;
  using IdsVector = VirtualMachineEventManager::IdsVector;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  using RunnableTracking = VirtualMachineEventManager::RunnableTracking;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;
  using VariableTracking = VirtualMachineEventManager::VariableTracking;

  using namespace patternmatching;

  Id authorId = 0;
  if (matches(vm, id, "none"))
    authorId = SIZE_MAX;
  else
    authorId = getArgument<size_t>(vm, id);

  IdsVector ids;
  ozListForEach(vm, idsList, [&ids](Id id) {
    ids.push_back(id);
  }, "List of integer ids");

  if (matches(vm, event, "runnable")) {
    if (matches(vm, announce, "created")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Created));
    } else if (matches(vm, announce, "updated")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Updated));
    } else if (matches(vm, announce, "collected")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Collected));
    } else if (matches(vm, announce, "scheduled")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Scheduled));
    } else if (matches(vm, announce, "unscheduled")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Unscheduled));
    } else if (matches(vm, announce, "rescheduled")) {
      eventManager.track(
        RunnableTracking(vm, authorId, ids, RunnableAnnounce::Rescheduled));
    } else {
      raiseTypeError(vm,
        "created, updated, collected, scheduled, unscheduled or rescheduled", announce);
    }
  } else if (matches(vm, event, "variable")) {
    if (matches(vm, announce, "created")) {
      eventManager.track(
        VariableTracking(vm, authorId, ids, VariableAnnounce::VCreated));
    } else if (matches(vm, announce, "collected")) {
      eventManager.track(
        VariableTracking(vm, authorId, ids, VariableAnnounce::VCollected));
    } else if (matches(vm, announce, "needed")) {
      eventManager.track(
        VariableTracking(vm, authorId, ids, VariableAnnounce::Needed));
    } else if (matches(vm, announce, "waited")) {
      eventManager.track(
        VariableTracking(vm, authorId, ids, VariableAnnounce::Waited));
    } else if (matches(vm, announce, "bound")) {
      eventManager.track(
        VariableTracking(vm, authorId, ids, VariableAnnounce::Bound));
    } else {
      raiseTypeError(vm,
        "created, collected, needed, waited or bound", announce);
    }
  } else {
    raiseTypeError(vm, "runnable or variable", event);
  }
}

}

}
