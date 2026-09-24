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
#include <iostream>

#include "mozartcore-decl.hh"

namespace mozart {

/**
 * @brief The point of this interface is to attribute automatically unique ids and group ids to Mozart objects
 * without making complicated modification on their definition.
 * 
 * The target objects are mainly runnables, variables and structures as abstractions and cons for streams.
 */

//////////////
// Identity //
//////////////

/**
 * @brief Attributes an unique id to a Mozart object
 * 
 * @tparam Identified is the class of the identified object
 * A dedicated static counter is created for this class
 */
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
   * @brief Get the identity like a cast from the class source Identified to Identity&
   * 
   * @return Identity& 
   */
  Identity& getIdentity() {
    return *this;
  }

  inline
  Identity& getIdentity(RichNode self, VM vm);

public:
  bool is() {
    return true;
  }

  inline
  bool is(RichNode self, VM vm);

  /**
   * @brief Get the id
   * 
   * @return size_t 
   */
  size_t getId() const {
    return _id;
  }
  
  inline
  size_t getId(RichNode self, VM vm);

  /**
   * @brief Set the id
   * 
   * @param id The new id to set to the identifiant
   * @remark Should be controlled only by the system or by the outside for debugging purposes
   */
  void setId(size_t id) {
    _id = id;
  }

  inline
  void setId(RichNode self, VM vm, size_t id);

public:
  /**
   * @brief Copy the identity from an other type of identity
   * 
   * @tparam OtherIdentified The class defining the identiable template
   * @param other Another instance of a different identifable template
   */
  void copyIdentity(const Identity& other) {
    _id = other.getId();
  }

  inline
  void copyIdentity(RichNode self, VM vm, const Identity& other);

  /**
   * @brief Copy the identity from an other type of identity
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  void copyIdentity(const Identity* other) {
    assert(other != nullptr);
    copyIdentity(*other);
  }

  inline
  void copyIdentity(RichNode self, VM vm, const Identity* other);

protected:
  size_t _id;
};

//////////////////////
// AdvancedIdentity //
//////////////////////

/**
 * @brief An advanced identifiant adding kind id and generation id
 * A kind is a group : a stream id, a reference to a piece of code or another kind of group
 * 
 * @tparam Identified is the class of the identified object
 */
class AdvancedIdentity: public Identity {
public:
  /** @brief Create an new advanced identity with an unique id */
  AdvancedIdentity(): Identity(),
    _kindId(SIZE_MAX), _generationId(0) {}

  /**
   * @brief Construct a new Advanced Identity object
   * 
   * @param other The identity from which inheriting the new ids
   */
  AdvancedIdentity(const AdvancedIdentity& other): Identity(other),
    _kindId(other._kindId), _generationId(other._generationId) {}

public:
  /**
   * @brief Get the identity like a cast from the class source AdvancedIdentity to AdvancedIdentity&
   * 
   * @return AdvancedIdentity& 
   */
  AdvancedIdentity& getAdvancedIdentity() {
    return *this;
  }

  inline
  AdvancedIdentity& getAdvancedIdentity(RichNode self, VM vm);

public:
  /**
   * @brief Get the kind id
   * 
   * @return size_t
   * @remark Kind id == id if the current identity is a kind leader
   */
  size_t getKindId() const {
    return _kindId == SIZE_MAX ?
      Identity::getId() : _kindId;
  }

  inline
  size_t getKindId(RichNode self, VM vm);

  /**
   * @brief Get the generation id
   * 
   * @return size_t 
   */
  size_t getGenerationId() const {
    return _generationId;
  }

  inline
  size_t getGenerationId(RichNode self, VM vm);

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

  inline
  bool isKindLeader(RichNode self, VM vm);

public:
  // Helps C++ to know which copyIdentity to use
  using Identity::copyIdentity;

  /**
   * @brief Copy the identify from an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other Another instance of a different identity template
   */
  void copyIdentity(const AdvancedIdentity& other) {
    Identity::copyIdentity(other);

    _kindId = other.getKindId();
    _generationId = other.getGenerationId();
  }
  
  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other);

  /**
   * @brief Copy the identify from an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  void copyIdentity(const AdvancedIdentity* other) {
    assert(other != nullptr);
    copyIdentity(*other);
  }

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other);

  /**
   * @brief Follow the identify of an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other Another instance of a different identity template
   */
  void followIdentity(const AdvancedIdentity& other, bool newGeneration) {
    assert(_kindId == SIZE_MAX);
    _kindId = other.getKindId();

    if (newGeneration)
      _generationId = other.getGenerationId() + 1;
    else
      _generationId = other.getGenerationId();
  }

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration);

  /**
   * @brief Follow the identify of an other advanced identity template
   * 
   * @tparam OtherIdentified The class defining the identity template
   * @param other A pointer on another instance of a different identity template
   */
  void followIdentity(const AdvancedIdentity* other, bool newGeneration) {
    assert(other != nullptr);
    followIdentity(*other, newGeneration);
  }
  
  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration);

private:
  size_t _kindId, _generationId;
};

////////////////////////
// ReferencedIdentity //
////////////////////////

class ReferencedIdentity {
public:
  ReferencedIdentity() {}

protected:
  inline
  virtual Identity& getIdentity() = 0;

public:
  inline
  Identity& getIdentity(RichNode self, VM vm);

public:
  inline
  bool is(RichNode self, VM vm);
  
  inline
  size_t getId(RichNode self, VM vm);

  inline
  void setId(RichNode self, VM vm, size_t id);

public:
  inline
  void copyIdentity(RichNode self, VM vm, const Identity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const Identity* other);
};

////////////////////////////////
// ReferencedAdvancedIdentity //
////////////////////////////////

class ReferencedAdvancedIdentity: public ReferencedIdentity {
public:
  ReferencedAdvancedIdentity() {}

protected:
  inline
  virtual AdvancedIdentity& getAdvancedIdentity() = 0;

public:
  inline
  AdvancedIdentity& getAdvancedIdentity(RichNode self, VM vm);

public:
  inline
  size_t getKindId(RichNode self, VM vm);

  inline
  size_t getGenerationId(RichNode self, VM vm);

public:
  inline
  bool isKindLeader(RichNode self, VM vm);

public:
  // Helps C++ to know which copyIdentity to use
  using ReferencedIdentity::copyIdentity;

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration);
};

////////////////////////////
// NodeReferencedIdentity //
////////////////////////////

class NodeReferencedIdentity {
public:
  NodeReferencedIdentity() {}

protected:
  inline
  virtual StableNode& dereference(RichNode self, VM vm) = 0;

public:
  inline
  Identity& getIdentity(RichNode self, VM vm);

public:
  inline
  bool is(RichNode self, VM vm);
  
  inline
  size_t getId(RichNode self, VM vm);

  inline
  void setId(RichNode self, VM vm, size_t id);

public:
  inline
  void copyIdentity(RichNode self, VM vm, const Identity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const Identity* other);
};

////////////////////////////////////
// NodeReferencedAdvancedIdentity //
////////////////////////////////////

class NodeReferencedAdvancedIdentity: public NodeReferencedIdentity {
public:
  NodeReferencedAdvancedIdentity() {}

public:
  inline
  AdvancedIdentity& getAdvancedIdentity(RichNode self, VM vm);

public:
  inline
  size_t getKindId(RichNode self, VM vm);

  inline
  size_t getGenerationId(RichNode self, VM vm);

public:
  inline
  bool isKindLeader(RichNode self, VM vm);

public:
  // Helps C++ to know which copyIdentity to use
  using NodeReferencedIdentity::copyIdentity;

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration);
};

///////////////////////
// DelegatedIdentity //
///////////////////////

/**
 * @brief Warning this interface should not have any attributes else it will break OptVar optimizations !!
 * 
 * This interface has for only purpose to interface identity methods to OptVar without making them heavier.
 * Any operation applied to a such variable will transform it into a normal variable then will perform
 * the requested operation. It allows transparency for the identity of variables vs opt variables,
 * to convert as less as possible opt variables to variables and converting an opt variable to variable
 * only when needed.
 */

class DelegatedIdentity {
protected:
  inline
  virtual void becomeIdentifiable(RichNode self, VM vm) = 0;

public:
  inline
  Identity& getIdentity(RichNode self, VM vm);

public:
  inline
  bool is(RichNode self, VM vm);
  
  inline
  size_t getId(RichNode self, VM vm);

  inline
  void setId(RichNode self, VM vm, size_t id);

public:
  inline
  void copyIdentity(RichNode self, VM vm, const Identity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const Identity* other);

};

///////////////////////////////
// DelegatedAdvancedIdentity //
///////////////////////////////

class DelegatedAdvancedIdentity : public DelegatedIdentity {
protected:
  inline
  virtual void becomeAdvancedIdentifiable(RichNode self, VM vm) = 0;

public:
  inline
  AdvancedIdentity& getAdvancedIdentity(RichNode self, VM vm);

public:
  inline
  size_t getKindId(RichNode self, VM vm);

  inline
  size_t getGenerationId(RichNode self, VM vm);

public:
  inline
  bool isKindLeader(RichNode self, VM vm);

public:
  using DelegatedIdentity::copyIdentity;

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity& other);

  inline
  void copyIdentity(RichNode self, VM vm, const AdvancedIdentity* other);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity& other, bool newGeneration);

  inline
  void followIdentity(RichNode self, VM vm, const AdvancedIdentity* other, bool newGeneration);
};

}

#endif