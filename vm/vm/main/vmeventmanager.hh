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

#ifndef MOZART_VMEVENTMANAGER_H
#define MOZART_VMEVENTMANAGER_H

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

////////////////////////////////
// VirtualMachineEventManager //
////////////////////////////////

UnstableNode VirtualMachineEventManager::buildIdsVector(VM vm, const IdsVector& vector) {
  OzListBuilder builder(vm);
  for (const Id id : vector) {
    builder.push_back(vm, build(vm, id));
  }
  return builder.get(vm);
}

Runnable* VirtualMachineEventManager::getCurrentThread() {
  return vm->getCurrentThread();
}

bool VirtualMachineEventManager::contains(const IdsVector& idsVector, Id id) {
  return id != SIZE_MAX && count(idsVector.begin(), idsVector.end(), id) > 0;
}

/** ================ Runnable events ================ */

UnstableNode VirtualMachineEventManager::buildRunnableEvent(VM vm,
  const RunnableEvent& event, RunnableAnnounce announce) {

  bool isCollected = announce == Collected;

  return buildRecord(vm,
    buildArity(vm,
      "event",
      "authorId",
      "victimId"
    ),
    event.author == nullptr || isCollected ?
      build(vm, "none") : build(vm, event.author->getId()),
    isCollected ? build(vm, event.id) : build(vm, event.runnable->getId())
  );
}

UnstableNode VirtualMachineEventManager::buildRunnableEventsVector(VM vm,
  const RunnableEventsVector& vector, RunnableAnnounce announce) {
  OzListBuilder builder(vm);
  for (const RunnableEvent& event : vector) {
    builder.push_back(vm, buildRunnableEvent(vm, event, announce));
  }
  return builder.get(vm);
}

UnstableNode VirtualMachineEventManager::buildRunnableEventsVectors(VM vm,
  const RunnableEventsVectors& vectors) {
  return buildRecord(vm,
    buildArity(vm,
      "events",
      "collected",
      "created",
      "rescheduled",
      "scheduled",
      "unscheduled",
      "updated"
    ),
    buildRunnableEventsVector(vm, vectors.collected, Collected),
    buildRunnableEventsVector(vm, vectors.created, Created),
    buildRunnableEventsVector(vm, vectors.rescheduled, Rescheduled),
    buildRunnableEventsVector(vm, vectors.scheduled, Scheduled),
    buildRunnableEventsVector(vm, vectors.unscheduled, Unscheduled),
    buildRunnableEventsVector(vm, vectors.updated, Updated)
  );
}

/** ================ Runnable tracks ================ */

UnstableNode VirtualMachineEventManager::buildRunnableTrack(VM vm,
  const RunnableTracking& track, Id index) {
  return buildRecord(vm,
    buildArity(vm,
      "track",
      "authorId",
      "index",
      "victimIds"
    ),
    track.authorId == SIZE_MAX ? build(vm, "none") : build(vm, track.authorId),
    build(vm, index),
    buildIdsVector(vm, track.victimIds)
  );
}

UnstableNode VirtualMachineEventManager::buildRunnableTracks(VM vm,
  const RunnableTrackingVector& vector) {
  OzListBuilder created(vm);
  OzListBuilder collected(vm);
  OzListBuilder updated(vm);
  OzListBuilder scheduled(vm);
  OzListBuilder unscheduled(vm);
  OzListBuilder rescheduled(vm);

  Id index = 0;
  for (const RunnableTracking& track : vector) {
    switch (track.announce) {
      case Created:
        created.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
      case Collected:
        collected.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
      case Updated:
        updated.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
      case Scheduled:
        scheduled.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
      case Unscheduled:
        unscheduled.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
      case Rescheduled:
        rescheduled.push_back(vm, buildRunnableTrack(vm, track, index));
        break;
    }
    index++;
  }
  
  return buildRecord(vm,
    buildArity(vm,
      "events",
      "collected",
      "created",
      "rescheduled",
      "scheduled",
      "unscheduled",
      "updated"
    ),
    collected.get(vm),
    created.get(vm),
    rescheduled.get(vm),
    scheduled.get(vm),
    unscheduled.get(vm),
    updated.get(vm)
  );
}

bool VirtualMachineEventManager::matchTracking(
  const RunnableTracking& tracking, const RunnableEvent& event) {

  if (tracking.authorId != SIZE_MAX
    && event.author != nullptr
    && tracking.authorId != event.author->getId())
    return false;
  else if (tracking.victimIds.empty())
    return true;
  else
    return contains(tracking.victimIds, event.runnable->getId());
}

/** ================ Variable events ================ */

template<class V>
UnstableNode VirtualMachineEventManager::buildVariableEvent(VM vm,
  const VariableEvent<V>& event, VariableAnnounce announce) {

  bool isCollected = announce == VCollected;

  return buildRecord(vm,
    buildArity(vm,
      "event",
      "authorId",
      "victimId"
    ),
    event.author == nullptr || isCollected ? build(vm, "none") :
      build(vm, event.author->getId()),
    isCollected ? build(vm, event.id) : build(vm, event.variable->getId())
  );
}

template<class V>
UnstableNode VirtualMachineEventManager::buildVariableEventsVector(VM vm,
  const VariableEventsVector<V>& vector, VariableAnnounce announce) {
  OzListBuilder builder(vm);
  for (const VariableEvent<V>& event : vector) {
    builder.push_back(vm, buildVariableEvent<V>(vm, event, announce));
  }
  return builder.get(vm);
}

template<class V>
UnstableNode VirtualMachineEventManager::buildVariableEventsVectors(VM vm,
  const VariableEventsVectors<V>& vectors) {
  return buildRecord(vm,
    buildArity(vm,
      "events",
      "bound",
      "collected",
      "created",
      "needed",
      "waited"
    ),
    buildVariableEventsVector<V>(vm, vectors.bound, Bound),
    buildVariableEventsVector<V>(vm, vectors.collected, VCollected),
    buildVariableEventsVector<V>(vm, vectors.created, VCreated),
    buildVariableEventsVector<V>(vm, vectors.needed, Needed),
    buildVariableEventsVector<V>(vm, vectors.waited, Waited)
  );
}

/** ================ Variable tracks ================ */

UnstableNode VirtualMachineEventManager::buildVariableTrack(VM vm,
  const VariableTracking& track, Id index) {
  return buildRecord(vm,
    buildArity(vm,
      "track",
      "authorId",
      "index",
      "victimIds"
    ),
    track.authorId == SIZE_MAX ? build(vm, "none") : build(vm, track.authorId),
    build(vm, index),
    buildIdsVector(vm, track.victimIds)
  );
}

UnstableNode VirtualMachineEventManager::buildVariableTracks(VM vm,
  const VariableTrackingVector& vector) {
  OzListBuilder created(vm);
  OzListBuilder collected(vm);
  OzListBuilder needed(vm);
  OzListBuilder waited(vm);
  OzListBuilder bound(vm);
  
  Id index = 0;
  for (const VariableTracking& track : vector) {
    switch (track.announce) {
      case VCreated:
        created.push_back(vm, buildVariableTrack(vm, track, index));
        break;
      case VCollected:
        collected.push_back(vm, buildVariableTrack(vm, track, index));
        break;
      case Needed:
        needed.push_back(vm, buildVariableTrack(vm, track, index));
        break;
      case Waited:
        waited.push_back(vm, buildVariableTrack(vm, track, index));
        break;
      case Bound:
        bound.push_back(vm, buildVariableTrack(vm, track, index));
        break;
    }
    index++;
  }

  return buildRecord(vm,
    buildArity(vm,
      "events",
      "bound",
      "collected",
      "created",
      "needed",
      "waited"
    ),
    bound.get(vm),
    collected.get(vm),
    created.get(vm),
    needed.get(vm),
    waited.get(vm)
  );
}

template<class V>
bool VirtualMachineEventManager::matchTracking(
  const VariableTracking& tracking, const VariableEvent<V>& event) {

  const IdsVector& ids = tracking.victimIds;

  if (tracking.authorId != SIZE_MAX
    && event.author != nullptr
    && tracking.authorId != event.author->getId())
    return false;
  else if (ids.empty())
    return true;
  else if (contains(ids, event.variable->getId()))
    return true;
  else if (contains(ids, event.variable->getKindId()))
    return true;
  else return false;
}

/** ================ Other ================ */

UnstableNode VirtualMachineEventManager::buildEventsJournal(VM vm, const VirtualMachineEventManager& eventManager) {
  return buildRecord(vm,
    buildArity(vm,
      "journal",
      "readOnlyVariable",
      "runnable",
      "variable"
    ),
    buildVariableEventsVectors<ReadOnlyVariable>(vm, eventManager.readOnlyVariableEventsVectors),
    buildRunnableEventsVectors(vm, eventManager.runnableEventsVectors),
    buildVariableEventsVectors<Variable>(vm, eventManager.variableEventsVectors)
  );
}

UnstableNode VirtualMachineEventManager::buildTracks(VM vm, const VirtualMachineEventManager& eventManager) {
  return buildRecord(vm,
    buildArity(vm,
      "tracks",
      "runnable",
      "variable"
    ),
    buildRunnableTracks(vm, eventManager.runnableTrackingVector),
    buildVariableTracks(vm, eventManager.variableTrackingVector)
  );
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_VMEVENTMANAGER_H
