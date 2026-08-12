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

bool VirtualMachineEventManager::contains(IdsVector& idsVector, size_t id) {
  return id != SIZE_MAX && count(idsVector.begin(), idsVector.end(), id) > 0;
}

template<class S>
size_t VirtualMachineEventManager::getStructureId(StructureInfo<S> structure) {
  if (structure.node.template is<S>())
    return structure.node.template as<S>().getId();
  else return SIZE_MAX;
}

bool VirtualMachineEventManager::matchTracking(VM vm, RunnableTracking& tracking,
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
bool VirtualMachineEventManager::matchTracking(VM vm, VariableTracking& tracking,
  VariableInfo<V> info, VariableAnnounce announce) {

  IdsVector& ids = tracking.idsVector;

  if (tracking.announce != announce)
    return false;
  else if (tracking.announcerThreadId != SIZE_MAX
    && tracking.announcerThreadId != info.author->getId())
    return false;
  else if (ids.empty())
    return true;
  else if (contains(ids, info.variable->getId()))
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
inline
bool VirtualMachineEventManager::matchTracking(VM vm, StructureTracking tracking,
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

}

#endif // MOZART_GENERATOR

#endif // MOZART_VMEVENTMANAGER_H
