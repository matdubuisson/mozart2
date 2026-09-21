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

#include "identity-decl.hh"
#include "mozartcore.hh"

namespace mozart {

//////////////
// Identity //
//////////////

Identity& Identity::getIdentity(RichNode self, VM vm) {
  return getIdentity();
}

void Identity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  copyIdentity(other);
}

void Identity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  copyIdentity(other);
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

////////////////////
// CopiedIdentity //
////////////////////

Identity& CopiedIdentity::getIdentity(RichNode self, VM vm) {
  return getIdentity();
}

void CopiedIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  getIdentity().copyIdentity(self, vm, other);
}

void CopiedIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  getIdentity().copyIdentity(self, vm, other);
}

bool CopiedIdentity::is(RichNode self, VM vm) {
  return getIdentity().is(self, vm);
}

size_t CopiedIdentity::getId(RichNode self, VM vm) {
  return getIdentity().getId(self, vm);
}

void CopiedIdentity::setId(RichNode self, VM vm, size_t id) {
  getIdentity().setId(self, vm, id);
}

//////////////////////
// AdvancedIdentity //
//////////////////////

AdvancedIdentity& AdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  return getAdvancedIdentity();
}

void AdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  copyIdentity(other);
}

void AdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  copyIdentity(other);
}

void AdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration) {
  if (_kindId == 11111111 || _kindId == 22222222 || _kindId == 33333333) {
    std::cout << "Who? " << self.type()->getName().c_str() << std::endl;
  }
  followIdentity(other, newGeneration);
}

void AdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration) {
  if (_kindId == 11111111 || _kindId == 22222222 || _kindId == 33333333) {
    std::cout << "Who? " << self.type()->getName().c_str() << std::endl;
  }
  followIdentity(other, newGeneration);
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

////////////////////////////
// CopiedAdvancedIdentity //
////////////////////////////

AdvancedIdentity& CopiedAdvancedIdentity::getAdvancedIdentity(RichNode self, VM vm) {
  return getAdvancedIdentity();
}

void CopiedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  getAdvancedIdentity().copyIdentity(self, vm, other);
}

void CopiedAdvancedIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  getAdvancedIdentity().copyIdentity(self, vm, other);
}

void CopiedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration) {
  getAdvancedIdentity().followIdentity(self, vm, other, newGeneration);
}

void CopiedAdvancedIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration) {
  getAdvancedIdentity().followIdentity(self, vm, other, newGeneration);
}

size_t CopiedAdvancedIdentity::getKindId(RichNode self, VM vm) {
  return getAdvancedIdentity().getKindId(self, vm);
}

size_t CopiedAdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  return getAdvancedIdentity().getGenerationId(self, vm);
}

bool CopiedAdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  return getAdvancedIdentity().isKindLeader(self, vm);
}

}

#endif