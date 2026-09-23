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

#include "mozart.hh"

namespace mozart {

// In order to resolve a cycling dependency problem :
//   - coreinterfaces-decl.hh needs identity-decl.hh
//   - identity-decl.hh needs coreinterfaces-decl.hh
// These implementations are put here into a concrete C file.

/////////////////////////
// TransparentIdentity //
/////////////////////////

bool TransparentIdentity::is(RichNode self, VM vm) {
  becomeIdentifiable(self, vm);
  return Identifiable(self).is(vm);
}

size_t TransparentIdentity::getId(RichNode self, VM vm) {
  becomeIdentifiable(self, vm);
  return Identifiable(self).getId(vm);
}

void TransparentIdentity::setId(RichNode self, VM vm, size_t id) {
  becomeIdentifiable(self, vm);
  Identifiable(self).setId(vm, id);
}

void TransparentIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  becomeIdentifiable(self, vm);
  Identifiable(self).copyIdentity(vm, other);
}

void TransparentIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  becomeIdentifiable(self, vm);
  Identifiable(self).copyIdentity(vm, other);
}

/////////////////////////////////
// TransparentAdvancedIdentity //
/////////////////////////////////

size_t TransparentAdvancedIdentity::getKindId(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).getKindId(vm);
}

size_t TransparentAdvancedIdentity::getGenerationId(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).getGenerationId(vm);
}

bool TransparentAdvancedIdentity::isKindLeader(RichNode self, VM vm) {
  becomeAdvancedIdentifiable(self, vm);
  return AdvancedIdentifiable(self).isKindLeader(vm);
}

void TransparentAdvancedIdentity::copyIdentity(RichNode self, VM vm,
  const AdvancedIdentity& other) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void TransparentAdvancedIdentity::copyIdentity(RichNode self, VM vm,
  const AdvancedIdentity* other) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void TransparentAdvancedIdentity::followIdentity(RichNode self, VM vm,
  const AdvancedIdentity& other, bool newGeneration) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

void TransparentAdvancedIdentity::followIdentity(RichNode self, VM vm,
  const AdvancedIdentity* other, bool newGeneration) {
  becomeAdvancedIdentifiable(self, vm);
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

}