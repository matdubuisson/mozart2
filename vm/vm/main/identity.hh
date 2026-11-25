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

#ifndef MOZART_IDENTITY_H
#define MOZART_IDENTITY_H

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

//////////////
// Identity //
//////////////

Identity& Identity::getIdentity(RichNode self, VM vm) {
  return getIdentity();
}

bool Identity::is(RichNode self, VM vm) {
  return true;
}

size_t Identity::getId(RichNode self, VM vm) {
  return _id;
}

void Identity::setId(RichNode self, VM vm, size_t id) {
  setId(id);
}

void Identity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  copyIdentity(other);
}

void Identity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  copyIdentity(other);
}

//////////////////////
// AdvancedIdentity //
//////////////////////

AdvancedIdentity& AdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  return getAdvancedIdentity();
}

size_t AdvancedIdentity::getKindId(RichNode self, VM vm) {
  return _kindId;
}

size_t AdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  return _generationId;
}

bool AdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  return _kindId == SIZE_MAX;
}

void AdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  copyIdentity(other);
}

void AdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  copyIdentity(other);
}

void AdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration) {
  followIdentity(other, newGeneration);
}

void AdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration) {
  followIdentity(other, newGeneration);
}

////////////////////////
// ReferencedIdentity //
////////////////////////

Identity& ReferencedIdentity::getIdentity(RichNode self, VM vm) {
  return getIdentity();
}

bool ReferencedIdentity::is(RichNode self, VM vm) {
  return getIdentity(self, vm).is(self, vm);
}

size_t ReferencedIdentity::getId(RichNode self, VM vm) {
  return getIdentity(self, vm).getId(self, vm);
}

void ReferencedIdentity::setId(RichNode self, VM vm, size_t id) {
  getIdentity(self, vm).setId(self, vm, id);
}

void ReferencedIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  getIdentity(self, vm).copyIdentity(self, vm, other);
}

void ReferencedIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  getIdentity(self, vm).copyIdentity(self, vm, other);
}

////////////////////////////////
// ReferencedAdvancedIdentity //
////////////////////////////////

AdvancedIdentity& ReferencedAdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  return getAdvancedIdentity();
}

size_t ReferencedAdvancedIdentity::getKindId(RichNode self, VM vm) {
  return getAdvancedIdentity(self, vm).getKindId(self, vm);
}

size_t ReferencedAdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  return getAdvancedIdentity(self, vm).getGenerationId(self, vm);
}

bool ReferencedAdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  return getAdvancedIdentity(self, vm).isKindLeader(self, vm);
}

void ReferencedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  getAdvancedIdentity(self, vm).copyIdentity(self, vm, other);
}

void ReferencedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  getAdvancedIdentity(self, vm).copyIdentity(self, vm, other);
}

void ReferencedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration) {
  getAdvancedIdentity(self, vm).followIdentity(self, vm, other, newGeneration);
}

void ReferencedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration) {
  getAdvancedIdentity(self, vm).followIdentity(self, vm, other, newGeneration);
}

////////////////////////////
// NodeReferencedIdentity //
////////////////////////////

Identity& NodeReferencedIdentity::getIdentity(RichNode self, VM vm) {
  return Identifiable(dereference(self, vm)).getIdentity(vm);
}

bool NodeReferencedIdentity::is(RichNode self, VM vm) {
  return Identifiable(dereference(self, vm)).is(vm);
}

size_t NodeReferencedIdentity::getId(RichNode self, VM vm) {
  return Identifiable(dereference(self, vm)).getId(vm);
}

void NodeReferencedIdentity::setId(RichNode self, VM vm, size_t id) {
  Identifiable(dereference(self, vm)).setId(vm, id);
}

void NodeReferencedIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  Identifiable(dereference(self, vm)).copyIdentity(vm, other);
}

void NodeReferencedIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  Identifiable(dereference(self, vm)).copyIdentity(vm, other);
}

////////////////////////////////////
// NodeReferencedAdvancedIdentity //
////////////////////////////////////

AdvancedIdentity& NodeReferencedAdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  return AdvancedIdentifiable(dereference(self, vm)).getAdvancedIdentity(vm);
}

bool NodeReferencedAdvancedIdentity::is(RichNode self, VM vm) {
  return AdvancedIdentifiable(dereference(self, vm)).is(vm);
}

size_t NodeReferencedAdvancedIdentity::getKindId(RichNode self, VM vm) {
  return AdvancedIdentifiable(dereference(self, vm)).getKindId(vm);
}

size_t NodeReferencedAdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  return AdvancedIdentifiable(dereference(self, vm)).getGenerationId(vm);
}

bool NodeReferencedAdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  return AdvancedIdentifiable(dereference(self, vm)).isKindLeader(vm);
}

void NodeReferencedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  AdvancedIdentifiable(dereference(self, vm)).copyIdentity(vm, other);
}

void NodeReferencedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  AdvancedIdentifiable(dereference(self, vm)).copyIdentity(vm, other);
}

void NodeReferencedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration) {
  AdvancedIdentifiable(dereference(self, vm)).followIdentity(vm, other, newGeneration);
}

void NodeReferencedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration) {
  AdvancedIdentifiable(dereference(self, vm)).followIdentity(vm, other, newGeneration);
}

///////////////////////
// DelegatedIdentity //
///////////////////////

Identity& DelegatedIdentity::getIdentity(RichNode self, VM vm) {
  becomeIdentifiable(self, vm);
  return Identifiable(self).getIdentity(vm);
}

bool DelegatedIdentity::is(RichNode self, VM vm) {
  becomeIdentifiable(self, vm);
  return Identifiable(self).is(vm);
}

size_t DelegatedIdentity::getId(RichNode self, VM vm) {
  becomeIdentifiable(self, vm);
  return Identifiable(self).getId(vm);
}

void DelegatedIdentity::setId(RichNode self, VM vm, size_t id) {
  becomeIdentifiable(self, vm);
  Identifiable(self).setId(vm, id);
}

void DelegatedIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  becomeIdentifiable(self, vm);
  Identifiable(self).copyIdentity(vm, other);
}

void DelegatedIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  becomeIdentifiable(self, vm);
  Identifiable(self).copyIdentity(vm, other);
}

///////////////////////////////
// DelegatedAdvancedIdentity //
///////////////////////////////

AdvancedIdentity& DelegatedAdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).getAdvancedIdentity(vm);
}

size_t DelegatedAdvancedIdentity::getKindId(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).getKindId(vm);
}

size_t DelegatedAdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).getGenerationId(vm);
}

bool DelegatedAdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).isKindLeader(vm);
}

void DelegatedAdvancedIdentity::copyIdentity(RichNode self, VM vm,
  const AdvancedIdentity& other) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void DelegatedAdvancedIdentity::copyIdentity(RichNode self, VM vm,
  const AdvancedIdentity* other) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void DelegatedAdvancedIdentity::followIdentity(RichNode self, VM vm,
  const AdvancedIdentity& other, bool newGeneration) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

void DelegatedAdvancedIdentity::followIdentity(RichNode self, VM vm,
  const AdvancedIdentity* other, bool newGeneration) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_IDENTITY_H