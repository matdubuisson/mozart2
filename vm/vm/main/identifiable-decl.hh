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

/**
 * @brief The point of this interface is to attribute automatically unique ids and group ids to Mozart objects
 * without making complicated modification on their definition.
 * 
 * The target objects are mainly runnables, variables and structures as abstractions and cons for streams.
 */

/**
 * @brief Attributes an unique id to a Mozart object
 * 
 * @tparam Identified is the class of the identified object
 * A dedicated static counter is created for this class
 */
template<class Identified>
class Identifiable {
private:
  // C++17 hack to automatically generating a static counter to the template without specifying it in advance
  inline static size_t _idsCounter = 0;

public:
  /** @brief Create a new identifiant with an unique id */
  Identifiable() : _id(_idsCounter++) {}

  /**
   * @brief Create an identifiant as a copy of another
   * 
   * @param other The identifiable from which inheriting the id
   */
  Identifiable(const Identifiable& other) : _id(other._id) {}

public:
  /**
   * @brief Copy the identity from an other type of identifiable
   * 
   * @tparam OtherIdentified The class defining the identiable template
   * @param other Another instance of a different identifable template
   */
  template<class OtherIdentified>
  void copyIdentity(const Identifiable<OtherIdentified>& other) {
    _id = other.getId();
  }

  /**
   * @brief Copy the identity from an other type of identifiable
   * 
   * @tparam OtherIdentified The class defining the identifiable template
   * @param other A pointer on another instance of a different identifiable template
   */
  template<class OtherIdentified>
  void copyIdentity(const Identifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    copyIdentity<OtherIdentified>(*other);
  }

public:
  /**
   * @brief Get the id
   * 
   * @return size_t 
   */
  size_t getId() const {
    return _id;
  }

  /**
   * @brief Set the id
   * 
   * @param id The new id to set to the identifiant
   * @remark Should be controlled only by the system or by the outside for debugging purposes
   */
  void setId(size_t id) {
    _id = id;
  }

public:
  /**
   * @brief Get the identity like a cast from the class source Identified to Identifiable<Identified>&
   * 
   * @return Identifiable<Identified>& 
   */
  Identifiable<Identified>& getIdentity() {
    return *this;
  }

protected:
  size_t _id;
};

/**
 * @brief An advanced identifiant adding kind id and generation id
 * A kind is a group : a stream id, a reference to a piece of code or another kind of group
 * 
 * @tparam Identified is the class of the identified object
 */
template<class Identified>
class AdvancedIdentifiable: public Identifiable<Identified> {
public:
  /** @brief Create an new advanced identity with an unique id */
  AdvancedIdentifiable(): Identifiable<Identified>(),
    _kindId(SIZE_MAX), _generationId(0) {}

  /**
   * @brief Construct a new Advanced Identifiable object
   * 
   * @param other The identity from which inheriting the new ids
   */
  AdvancedIdentifiable(const AdvancedIdentifiable& other): Identifiable<Identified>(other),
    _kindId(other._kindId), _generationId(other._generationId) {}

public:
  // Helps C++ to know which copyIdentity to use
  using Identifiable<Identified>::copyIdentity;

  /**
   * @brief Copy the identify from an other advanced identifiable template
   * 
   * @tparam OtherIdentified The class defining the identifiable template
   * @param other Another instance of a different identifiable template
   */
  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentifiable<OtherIdentified>& other) {
    Identifiable<Identified>::copyIdentity(other);

    _kindId = other.getKindId();
    _generationId = other.getGenerationId();
  }

  /**
   * @brief Copy the identify from an other advanced identifiable template
   * 
   * @tparam OtherIdentified The class defining the identifiable template
   * @param other A pointer on another instance of a different identifiable template
   */
  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    copyIdentity<OtherIdentified>(*other);
  }

  /**
   * @brief Follow the identify of an other advanced identifiable template
   * 
   * @tparam OtherIdentified The class defining the identifiable template
   * @param other Another instance of a different identifiable template
   */
  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentifiable<OtherIdentified>& other) {
    _kindId = other.getKindId();
    _generationId = other.getGenerationId() + 1;
  }

  /**
   * @brief Follow the identify of an other advanced identifiable template
   * 
   * @tparam OtherIdentified The class defining the identifiable template
   * @param other A pointer on another instance of a different identifiable template
   */
  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentifiable<OtherIdentified>* other) {
    assert(other != nullptr);
    followIdentity<OtherIdentified>(*other);
  }

public:
  /**
   * @brief Get the kind id
   * 
   * @return size_t
   * @remark Kind id == id if the current identity is a kind leader
   */
  size_t getKindId() const {
    return _kindId == SIZE_MAX ?
      Identifiable<Identified>::getId() : _kindId;
  }

  /**
   * @brief Get the generation id
   * 
   * @return size_t 
   */
  size_t getGenerationId() const {
    return _generationId;
  }

public:
  /**
   * @brief Tells if the current identity is a kind leader
   * 
   * @return true 
   * @return false 
   */
  bool isKindLeader() const {
    return _kindId == SIZE_MAX;
  }

public:
  /**
   * @brief Get the identity like a cast from the class source AdvancedIdentifiable to AdvancedIdentifiable<Identified>&
   * 
   * @return AdvancedIdentifiable<Identified>& 
   */
  AdvancedIdentifiable<Identified>& getAdvancedIdentity() {
    return *this;
  }

private:
  size_t _kindId, _generationId;
};

}

#endif