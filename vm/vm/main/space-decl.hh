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

#ifndef MOZART_SPACE_DECL_H
#define MOZART_SPACE_DECL_H

#include "core-forward-decl.hh"

#include "store-decl.hh"
#include "exceptions-decl.hh"
#include "vmallocatedlist-decl.hh"

namespace mozart {

struct ScriptEntry {
public:
  UnstableNode left;
  UnstableNode right;
};

class SpaceScript : public VMAllocatedList<ScriptEntry> {
private:
  typedef VMAllocatedList<ScriptEntry> Super;
public:
  /**
   * Appends a new script entry at the end of the script
   * @param vm The virtual machine
   * @returns A reference on the new script entry
   */
  inline
  ScriptEntry& append(VM vm);
};

struct TrailEntry {
public:
  TrailEntry(StableNode* node, const Node& saved) : node(node), saved(saved) {}

  StableNode* node;
  Node saved;
};

class SpaceTrail : public VMAllocatedList<TrailEntry> {
};

/**
 * Base class for distributors
 */
class Distributor {
public:
  virtual nativeint getAlternatives() = 0;

  virtual nativeint commit(VM vm, Space* space, nativeint value) = 0;

  virtual Distributor* replicate(GR gr) = 0;
};

/**
 * Computation space
 */
class Space {
public:
  /**
   * A space can have three different statuses:
   * - ssReference: 
   * - ssNormal: 
   * - ssFailed: 
   */
  enum Status {
    ssReference, ssNormal, ssFailed
  };
public:
  /**
   * Constructs the top-level space
   * @param vm The virtual machine
   * @note The top-level space has for parent nullptr
   */
  inline
  Space(VM vm);

  /**
   * Constructs a subspace with parent the provided space
   * @param vm The virtual machine
   * @param parent The parent space
   */
  inline
  Space(VM vm, Space* parent);

  /**
   * Constructs a new space based on the provided space and using the provided
   * graph replicator
   * @param gr The graph replicator
   * @param from The space to copy
   * @note The provided status cannot have the status ssReference
   */
  inline
  Space(GR gr, Space* from);
private:
  /**
   * Initiates the space
   * @param vm The virtual machine
   * @param isTopLevel If the space is top-level or not
   * @param parent The parent space
   * @note If the space is on top-level then the parent should be nullptr
   */
  inline
  void constructor(VM vm, bool isTopLevel, Space* parent);

public:
  /**
   * @returns The parent space
   * @note Could be null if the space is on top-level
   */
  Space* getParent() {
    if (_isTopLevel)
      return nullptr;
    else
      return _parent;
  }

// Status

public:
  /** @returns If the space is on top-level */
  bool isTopLevel() {
    return _isTopLevel;
  }

  /** @returns The status of the space */
  Status status() {
    return _status;
  }

  /** @returns If the space has failed as status */
  bool isFailed() {
    return _status == ssFailed;
  }

  /**
   * @returns If the space is alive
   * @note A space is alive if all its ancestors are alive too
   */
  inline
  bool isAlive();

  /** @returns A stable node pointer on the root variable of the space */
  StableNode* getRootVar() {
    return &_rootVar;
  }

  /** @returns An unstable node pointer on the status variable of the space */
  UnstableNode* getStatusVar() {
    return &_statusVar;
  }

// Admissibility

  /**
   * Tells if the space is admissible in other words equivalently to know if the
   * space is not an ancestor of the provided current space :
   * A space A is admissible for an operation while the current space is B iff
   * A is not an ancestor of B
   * @param currentSpace The current space
   * @returns If the space is admissible for the current space
   */
  inline
  bool isAdmissible(Space* currentSpace);

  /**
   * Tells if the space is admissible in other words equivalently to know if the
   * space is not an ancestor of the current space of the provided virtual machine :
   * A space A is admissible for an operation while the current space is B iff
   * A is not an ancestor of B
   * @param vm The virtual machine
   * @returns If the space is admissible for the VM's current space
   */
  inline
  bool isAdmissible(VM vm);

  /**
   * Tells if the space is admissible in other words equivalently to know if the
   * space is not an ancestor of the current space of its virtual machine :
   * A space A is admissible for an operation while the current space is B iff
   * A is not an ancestor of B
   * @returns If the space is admissible for the space's VM's current space
   */
  inline
  bool isAdmissible();

// Relations between spaces

public:
  /**
   * Tells if the provided space is an ancestor of the current space
   * @param potentialAncestor The potential ancestor space
   * @returns If the provided space is an ancestor of the current space
   */
  inline
  bool isAncestor(Space* potentialAncestor);

// Speculative bindings

public:
  /**
   * TO-COMPLETE : no what is speculative binding
   * @param node The node to make a backup for
   */
  inline
  void makeBackupForSpeculativeBinding(StableNode* node);

// Operations

public:
  /**
   * Makes the space failed and sets VM's current space to the parent space
   * @param vm The virtual machine
   * @note The space must not be on top-level
   */
  inline
  void fail(VM vm);

  /**
   * Merges both spaces (provided and this) into the provided one and makes this space
   * as a transparent reference to the merged space
   * @param vm The virtual machine
   * @param destSpace The space to merge with
   * @returns If the merge succeeded
   * @note VM's current space must be the provided space
   */
  inline
  bool merge(VM vm, Space* destSpace);

  /**
   * Commits a value to the space distributor with the provided virtual machine
   * @param vm The virtual machine
   * @param value The value to commit
   * @returns The result of the commit operation
   */
  inline
  nativeint commit(VM vm, nativeint value);

  /**
   * Clones the space with the provided virtual machine
   * @param vm The virtual machine
   * @returns A clone of the space
   */
  inline
  Space* clone(VM vm);

  /**
   * Kills the space
   * @param vm The virtual machine
   * @note The space must not be on top-level and its parent must be VM's current space
   */
  inline
  void kill(VM vm);

// Distributor

public:
  /** @returns If the space has a distributor */
  bool hasDistributor() {
    return _distributor != nullptr;
  }

  /** @returns A pointer to the space's distributor */
  Distributor* getDistributor() {
    return _distributor;
  }

  /**
   * Sets the distributor of the space
   * @param distributor A pointer of the distributor to set
   */
  void setDistributor(Distributor* distributor) {
    _distributor = distributor;
  }

// Status variable

private:
  /**
   * Clears the status variable if still transient
   * @param vm The virtual machine
   */
  inline
  void clearStatusVar(VM vm);

  /**
   * Binds the status variable to a value
   * @param vm The virtual machine
   * @param value The value to bind to the status variable
   * @note The status variable must be transient
   */
  inline
  void bindStatusVar(VM vm, RichNode value);

  /**
   * Binds the status variable to a value
   * @param vm The virtual machine
   * @param value The value to bind to the status variable
   * @note The status variable must be transient
   */
  inline
  void bindStatusVar(VM vm, UnstableNode&& value);

  /**
   * Generates a succeeded value for the space
   * @param vm The virtual machine
   * @param isEntailed If the succeeded value has to be entailed or not
   * @returns A succeeded value for the space
   * @note TO-COMPLETE : what is the purpose ?
   */
  inline
  UnstableNode genSucceeded(VM vm, bool isEntailed);

// Garbage collection and cloning

public:
  /** @returns If the space should be cloned */
  bool shouldBeCloned() {
    return !hasMark();
  }

  /** Sets its mark and its parent mark if there is one */
  inline
  void setShouldNotBeCloned();

  /** Insets its mark and its parent mark if there is one */
  inline
  void unsetShouldNotBeCloned();

  /**
   * Allocates a new space with the provided garbage collector
   * @param gc The garbage collector
   * @returns A new space
   */
  inline
  virtual Space* gCollect(GC gc);

  /**
   * Allocates a new space with the provided space cloner
   * @param sc The space cloner
   * @returns A new space
   */
  inline
  virtual Space* sClone(SC sc);

  /**
   * Allocates a new space with the provided garbage collector
   * if the current replicate is null else it returns the current replicate
   * @param gc The garbage collector
   * @returns A new space
   */
  inline
  Space* gCollectOuter(GC gc);

  /**
   * Allocates a new space with the provided space cloner
   * if the current replicate is null else it returns the current replicate
   * @param sc The space cloner
   * @returns A new space
   */
  inline
  Space* sCloneOuter(SC sc);

  /** Restores after graph replication in other words it drops the current replicate */
  inline
  void restoreAfterGR();

// Stability detection

public:
  /** Notifies the space that a thread has been created */
  inline
  void notifyThreadCreated();

  /**
   * Notifies the space that a thread has been terminated
   * @note The number of runnable threads must be higher than 0
   */
  inline
  void notifyThreadTerminated();

  /** Notifies the space that a thread has been resumed */
  inline
  void notifyThreadResumed();

  /** Notifies the space that a thread has been suspended */

  inline
  void notifyThreadSuspended();
public:
  /** @returns If the space is stable */
  inline
  bool isStable();

  /** @returns If the space is blocked */
  inline
  bool isBlocked();
private:
  /**
   * Increments the thread counter of n
   * @note The space must not be failed and the provided n must be positive
   */
  inline
  void incThreadCount(int n = 1);

  /**
   * Decrements the thread counter of 1
   * @note The space must not be failed and the thread counter must be higher than 0
   */
  inline
  void decThreadCount();

  /**
   * @returns The thread counter
   * @note The space must not be failed and the thread counter must be positive 0
   */
  inline
  int getThreadCount();

  /** Increments the runnable thread counter of 1 */
  inline
  void incRunnableThreadCount();

  /** Decrements the runnable thread counter of 1 */
  inline
  void decRunnableThreadCount();

  /** @returns If the runnable thread counter is higher than 0 */
  inline
  bool hasRunnableThreads();

  /**
   * Checks the stability of the space
   * @note The space must not be on top-level and the space status must be normal
   */
  inline
  void checkStability();

// Reference

private:
  /**
   * Sets the reference of the space and the space becomes a reference as its
   * status is set to reference
   */
  void setReference(Space* ref) {
    _status = ssReference;
    _reference = ref;
  }

// Installation and deinstallation

public:
  // TO-COMPLETE : check if comments for installation methods are 100% correct

  /**
   * Try to install the current space
   * @returns If the installation succeeded
   * @note Upon failure, the highest space that could be installed is installed
   */
  inline
  bool install();
private:
  /**
   * De-installs all spaces between common ancestor and the provided space then
   * installs all spaces from current space to the ancestor
   * @param from The space to look with
   * @returns If the installation succeeded
   * @note The common nearest ancestor between both current and provided spaces
   */
  inline
  bool doInstall(Space* from);

  /**
   * Finds the common ancestor between the current space and the provided one
   * @param other The other space to compare with
   * @returns The space ancestor
   */
  inline
  Space* findCommonAncestor(Space* other);

  /**
   * Runs deinstallThis on all spaces between the ancestor and the current space
   */
  inline
  void deinstallTo(Space* ancestor);

  /**
   * Runs installThis on all spaces between ancestor and the current space (included)
   * @returns If the installation succeeded
   */
  inline
  bool installFrom(Space* ancestor);

  /**
   * Restores all saved nodes from the trail entries and sets script
   * entries accordingly
   */
  inline
  void deinstallThis();

  /** Restores all saved nodes from the trail entries */
  inline
  void deinstallThisFailed();

  /**
   * Tries to unify all script entry of the space
   * @param isMerge unused
   * @returns If all the unification succeeded
   */
  inline
  bool installThis(bool isMerge = false);

  /**
   * If the variable is transient suspends the provided thread (runnable) on the
   * provided variable and it might initializes the propagated thread as a dummy
   * thread if it is nullptr
   */
  inline
  void createPropagateThreadOnceAndSuspendItOnVar(
    VM vm, Runnable*& propagateThread, RichNode variable);

// The mark

private:
  /** @returns If the space is marked */
  bool hasMark() {
    return _mark;
  }

  /** Sets the mark */
  void setMark() {
    _mark = true;
  }

  /** Resets the mark */
  void unsetMark() {
    _mark = false;
  }

// Fields

private:
  friend struct SpaceRef;

  VM vm;

  union {
    SpaceRef _parent;  // status != ssReference && !isTopLevel
    Space* _reference; // status == ssReference
  };

  Space* _replicate;

  bool _isTopLevel;
  Status _status;

  bool _mark;

  VMAllocatedList<Runnable*> suspensionList;

  StableNode _rootVar;
  UnstableNode _statusVar;

  Distributor* _distributor;

  SpaceTrail trail;
  SpaceScript script;

  /*
   * Maintaining a counter of threads
   * Invariants:
   *
   *   threadCount ==
   *     Number of threads th such that th->getSpace() == this,
   *     which have been created but are not yet terminated.
   *     Note that threads that are suspended (even lost, i.e.,
   *     garbage-collectable), or that have failed are part of this count.
   *
   *   cascadedRunnableThreadCount == A + B
   *     where A ==
   *       Number of threads th such that th->getSpace() == this &&
   *       th->isRunnable().
   *     and B ==
   *       Number of spaces sp such that sp->getParent() == this &&
   *       sp->cascadedRunnableThreadCount > 0.
   *
   * These invariants are maintained by the notifyThread...() methods.
   */

  int threadCount;
  int cascadedRunnableThreadCount;
};

}

#endif // MOZART_SPACE_DECL_H
