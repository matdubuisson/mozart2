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

#ifndef MOZART_IDENTITY_DECL_H
#define MOZART_IDENTITY_DECL_H

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
class Identity {
private:
  // C++20 automatically generating a static counter to the template without specifying it in advance
  inline static size_t _idsCounter = 0;

public:
  /** @brief Create a new identifiant with an unique id */
  Identity() : _id(_idsCounter++) {}

  /**
   * @brief Create an identifiant as a copy of another
   * 
   * @param other The identity from which inheriting the id
   */
  Identity(const Identity& other) : _id(other._id) {}

public:
  /**
   * @brief Copy the identity from an other type of identity
   * 
   * @tparam OtherIdentified The class defining the identiable template
   * @param other Another instance of a different identifable template
   */
  template<class OtherIdentified>
  void copyIdentity(const Identity<OtherIdentified>& other) {
    _id = other.getId();
  }

  /**
   * @brief Copy the identity from an other type of identity
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  template<class OtherIdentified>
  void copyIdentity(const Identity<OtherIdentified>* other) {
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
   * @brief Get the identity like a cast from the class source Identified to Identity<Identified>&
   * 
   * @return Identity<Identified>& 
   */
  Identity<Identified>& getIdentity() {
    return *this;
  }

protected:
  size_t _id;
};

class AdvancedIdentityTransmitter;

/**
 * @brief An advanced identifiant adding kind id and generation id
 * A kind is a group : a stream id, a reference to a piece of code or another kind of group
 * 
 * @tparam Identified is the class of the identified object
 */
template<class Identified>
class AdvancedIdentity: public Identity<Identified> {
public:
  /** @brief Create an new advanced identity with an unique id */
  AdvancedIdentity(): Identity<Identified>(),
    _kindId(SIZE_MAX), _generationId(0) {}

  /**
   * @brief Construct a new Advanced Identity object
   * 
   * @param other The identity from which inheriting the new ids
   */
  AdvancedIdentity(const AdvancedIdentity& other): Identity<Identified>(other),
    _kindId(other._kindId), _generationId(other._generationId) {}

public:
  // Helps C++ to know which copyIdentity to use
  using Identity<Identified>::copyIdentity;

  /**
   * @brief Copy the identify from an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other Another instance of a different identity template
   */
  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentity<OtherIdentified>& other) {
    Identity<Identified>::copyIdentity(other);

    _kindId = other.getKindId();
    _generationId = other.getGenerationId();
  }

  /**
   * @brief Copy the identify from an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  template<class OtherIdentified>
  void copyIdentity(const AdvancedIdentity<OtherIdentified>* other) {
    assert(other != nullptr);
    copyIdentity<OtherIdentified>(*other);
  }

  /**
   * @brief Follow the identify of an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other Another instance of a different identity template
   */
  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentity<OtherIdentified>& other, bool newGeneration = true) {
    _kindId = other.getKindId();

    if (newGeneration)
      _generationId = other.getGenerationId() + 1;
    else
      _generationId = other.getGenerationId();
  }

  /**
   * @brief Follow the identify of an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  template<class OtherIdentified>
  void followIdentity(const AdvancedIdentity<OtherIdentified>* other, bool newGeneration = true) {
    assert(other != nullptr);
    followIdentity<OtherIdentified>(*other, newGeneration);
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
      Identity<Identified>::getId() : _kindId;
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
   * @brief Get the identity like a cast from the class source AdvancedIdentity to AdvancedIdentity<Identified>&
   * 
   * @return AdvancedIdentity<Identified>& 
   */
  AdvancedIdentity<Identified>& getAdvancedIdentity() {
    return *this;
  }

private:
  friend class AdvancedIdentityTransmitter;

  size_t _kindId, _generationId;
};

// class AdvancedIdentityTransmitter {
// public:
//   AdvancedIdentityTransmitter() {}

// public:
//   /**
//    * @brief Follow the identify of an other advanced identity template
//    * 
//    * @tparam OtherIdentified The class defining the identity template
//    * @param other Another instance of a different identity template
//    */
//   template<class OtherIdentified>
//   void followIdentity(const AdvancedIdentity<OtherIdentified>& other) {
//     _kindId = other._kindId;
//     _generationId = other._generationId;
//   }

//   /**
//    * @brief Follow the identify of an other advanced identity template
//    * 
//    * @tparam OtherIdentified The class defining the identity template
//    * @param other A pointer on another instance of a different identity template
//    */
//   template<class OtherIdentified>
//   void followIdentity(const AdvancedIdentity<OtherIdentified>* other) {
//     assert(other != nullptr);
//     followIdentity<OtherIdentified>(*other);
//   }

//     /**
//    * @brief Follow the identify of an other advanced identity template
//    * 
//    * @tparam OtherIdentified The class defining the identity template
//    * @param other Another instance of a different identity template
//    */
//   template<class OtherIdentified>
//   void transmitIdentity(const AdvancedIdentity<OtherIdentified>& other) {
//     other._kindId = _kindId;
//     other._generationId = _generationId + 1;
//   }

//   /**
//    * @brief Follow the identify of an other advanced identity template
//    * 
//    * @tparam OtherIdentified The class defining the identity template
//    * @param other A pointer on another instance of a different identity template
//    */
//   template<class OtherIdentified>
//   void transmitIdentity(const AdvancedIdentity<OtherIdentified>* other) {
//     assert(other != nullptr);
//     transmitIdentity<OtherIdentified>(*other);
//   }

// private:
//   size_t _kindId, _generationId;
// };

}

#endif