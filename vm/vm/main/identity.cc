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

////////////////////
// OptVarIdentity //
////////////////////

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const Identity& other) {
  self.become(vm, Variable::build(vm));
  Identifiable(self).copyIdentity(vm, other);
}

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const Identity* other) {
  self.become(vm, Variable::build(vm));
  Identifiable(self).copyIdentity(vm, other);
}

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other) {
  self.become(vm, Variable::build(vm));
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other) {
  self.become(vm, Variable::build(vm));
  AdvancedIdentifiable(self).copyIdentity(vm, other);
}

void OptVarIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity& other,
  bool newGeneration) {
  self.become(vm, Variable::build(vm));
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

void OptVarIdentity::followIdentity(RichNode self, VM vm, const AdvancedIdentity* other,
  bool newGeneration) {
  self.become(vm, Variable::build(vm));
  AdvancedIdentifiable(self).followIdentity(vm, other, newGeneration);
}

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const OptVarIdentity& other) {
  assert(false);
}

void OptVarIdentity::copyIdentity(RichNode self, VM vm, const OptVarIdentity* other) {
  assert(false);
}

void OptVarIdentity::followIdentity(RichNode self, VM vm, const OptVarIdentity& other,
  bool newGeneration) {
  assert(false);
}

void OptVarIdentity::followIdentity(RichNode self, VM vm, const OptVarIdentity* other,
  bool newGeneration) {
  assert(false);
}

bool OptVarIdentity::is(RichNode self, VM vm) {
  self.become(vm, Variable::build(vm));
  return Identifiable(self).is(vm);
}

size_t OptVarIdentity::getId(RichNode self, VM vm) {
  self.become(vm, Variable::build(vm));
  return Identifiable(self).getId(vm);
}

void OptVarIdentity::setId(RichNode self, VM vm, size_t id) {
  self.become(vm, Variable::build(vm));
  Identifiable(self).setId(vm, id);
}

size_t OptVarIdentity::getKindId(RichNode self, VM vm) {
  self.become(vm, Variable::build(vm));
  return AdvancedIdentifiable(self).getKindId(vm);
}

size_t OptVarIdentity::getGenerationId(RichNode self, VM vm) {
  self.become(vm, Variable::build(vm));
  return AdvancedIdentifiable(self).getGenerationId(vm);
}

bool OptVarIdentity::isKindLeader(RichNode self, VM vm) {
  self.become(vm, Variable::build(vm));
  return AdvancedIdentifiable(self).isKindLeader(vm);
}

}