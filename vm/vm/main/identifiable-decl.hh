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

#ifndef MOZART_IDENTIFIABLE_DECL_H
#define MOZART_IDENTIFIABLE_DECL_H

#include <cstdint>
#include <cstddef>
#include <cassert>

namespace mozart {

template<class Identified>
class Identifiable {
private:
  inline static size_t _idsCounter = 0;

public:
  Identifiable() : _id(_idsCounter++) {}

  Identifiable(const Identifiable& other) : _id(other._id) {}

public:
  template<class OtherIdentified>
  void copyIdentity(const Identifiable<OtherIdentified>& other) {
    _id = other.getId();
  }

  template<class OtherIdentified>
  void copyIdentity(const Identifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    copyIdentity<OtherIdentified>(*other);
  }

public:
  size_t getId() const {
    return _id;
  }

  void setId(size_t id) {
    _id = id;
  }

public:
  Identifiable<Identified>& getIdentity() {
    return *this;
  }

protected:
  size_t _id;
};

template<class Identified>
class AdvancedIdentifiable: public Identifiable<Identified> {
public:
  AdvancedIdentifiable(): Identifiable<Identified>(),
    _kindId(SIZE_MAX), _generationId(0) {}
  
  AdvancedIdentifiable(const AdvancedIdentifiable& other): Identifiable<Identified>(other),
    _kindId(other._kindId), _generationId(other._generationId) {}

public:
  using Identifiable<Identified>::copyIdentity;

  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentifiable<OtherIdentified>& other) {
    Identifiable<Identified>::copyIdentity(other);

    _kindId = other.getKindId();
    _generationId = other.getGenerationId();
  }

  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    copyIdentity<OtherIdentified>(*other);
  }

  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentifiable<OtherIdentified>& other) {
    _kindId = other.getKindId();
    _generationId = other.getGenerationId() + 1;
  }

  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    followIdentity<OtherIdentified>(*other);
  }

public:
  size_t getKindId() const {
    return _kindId == SIZE_MAX ?
      Identifiable<Identified>::getId() : _kindId;
  }

  size_t getGenerationId() const {
    return _generationId;
  }

public:
  bool isKindLeader() const {
    return _kindId == SIZE_MAX;
  }

public:
  AdvancedIdentifiable<Identified>& getAdvancedIdentity() {
    return *this;
  }

private:
  size_t _kindId, _generationId;
};

}

#endif