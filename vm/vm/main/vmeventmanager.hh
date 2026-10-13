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

Runnable* VirtualMachineEventManager::getCurrentThread(VM vm) {
  return vm->getCurrentThread();
}

bool VirtualMachineEventManager::contains(IdsVector& idsVector, Id id) {
  return id != SIZE_MAX && count(idsVector.begin(), idsVector.end(), id) > 0;
}

/** ================ Runnable events ================ */

UnstableNode VirtualMachineEventManager::buildRunnableEvent(VM vm,
  const RunnableEvent& event, RunnableAnnounce announce) {

  if (announce == Collected) {
    return buildRecord(vm,
      buildArity(vm,
        "event",
        "id"
      ),
      build(vm, event.id)
    );
  } else {
    return buildRecord(vm,
      buildArity(vm,
        "event",
        "authorId",
        "victimId"
      ),
      event.author == nullptr ?
        build(vm, "none") : build(vm, event.author->getId()),
      build(vm, event.runnable->getId())
    );
  }
}

UnstableNode VirtualMachineEventManager::buildRunnableEventsVector(VM vm,
  const RunnableEventsVector& vector, RunnableAnnounce announce) {
  OzListBuilder builder(vm);
  for (const RunnableEvent event : vector) {
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

bool VirtualMachineEventManager::matchTracking(VM vm,
  RunnableTracking& tracking, RunnableEvent info) {

  if (tracking.authorId != SIZE_MAX
    && info.author != nullptr
    && tracking.authorId != info.author->getId())
    return false;
  else if (tracking.victimIds.empty())
    return true;
  else
    return contains(tracking.victimIds, info.runnable->getId());
}

/** ================ Variable events ================ */

/** ================ Variable tracks ================ */

template<class V>
bool VirtualMachineEventManager::matchTracking(VM vm, VariableTracking& tracking,
  VariableInfo<V> info, VariableAnnounce announce) {

  IdsVector& ids = tracking.idsVector;

  if (tracking.announce != announce)
    return false;
  else if (tracking.announcerThreadId != SIZE_MAX
    && info.author != nullptr
    && tracking.announcerThreadId != info.author->getId())
    return false;
  else if (ids.empty())
    return true;
  else if (contains(ids, info.variable->getId()))
    return true;
  else if (contains(ids, info.variable->getKindId()))
    return true;
  else return false;
}

template<class V>
bool VirtualMachineEventManager::matchTracking(VM vm, VariableTracking& tracking,
  BoundVariableInfo<V> info, VariableAnnounce announce) {

  if (matchTracking<V>(vm, tracking,
    static_cast<VariableInfo<V>>(info), announce))
    return true;
  else return false;
}

template<class V>
bool VirtualMachineEventManager::matchTracking(VM vm, VariableTracking& tracking,
  WaitedVariableInfo<V> info, VariableAnnounce announce) {

  if (matchTracking<V>(vm, tracking,
    static_cast<VariableInfo<V>>(info), announce))
    return true;
  else return false;
}

template<class S>
bool VirtualMachineEventManager::matchTracking(VM vm, StructureTracking tracking,
  StructureInfo<S> info, StructureAnnounce announce) {

  if (tracking.announce != announce)
    return false;
  else if (tracking.announcerThreadId != SIZE_MAX
    && info.author != nullptr
    && tracking.announcerThreadId != info.author->getId())
    return false;
  else if (tracking.idsVector.empty())
    return true;
  else
    return contains(tracking.idsVector, info.structure->getId())
      || contains(tracking.idsVector, info.structure->getKindId());
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_VMEVENTMANAGER_H
