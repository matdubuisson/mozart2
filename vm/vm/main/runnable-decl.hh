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
// THIS SOFTWARE IS PROVIdED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIdENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MOZART_RUNNABLE_DECL_H
#define MOZART_RUNNABLE_DECL_H

#include "core-forward-decl.hh"
#include "store-decl.hh"
#include "vmallocatedlist-decl.hh"

#include <cassert>

namespace mozart {

// This enum is often used for indexing in arrays
// tpCount reflects the number of valid ThreadPriority's
// And you probably don't want to add or remove existing ThreadPriority's
enum ThreadPriority {
  tpLow, tpMiddle, tpHi, tpSystem,
  tpCount
};

///////////////////////
// IntermediateState //
///////////////////////

// TO-COMPLETE : the comments for IntermediateState needs some refactoring
class IntermediateState {
private:
  typedef VMAllocatedList<UnstableNode> List;
public:
  typedef List::iterator CheckPoint;
public:
  /**
   * Instantiates a new intermediate state
   * @param vm The virtual machine
   */
  inline
  explicit IntermediateState(VM vm);

  /**
   * Instantiates a new intermediate state
   * Copies the list of unstable nodes of the provided intermediate state
   * @param vm The virtual machine
   * @param gr The graph replicator
   * @param from A reference on another intermediate state
   */
  inline
  IntermediateState(VM vm, GR gr, IntermediateState& from);

  /**
   * Gets the last element of the list of unstable nodes as check point
   * @param vm The virtual machine
   * @returns An iterable on the check point (unstable node)
   */
  CheckPoint makeCheckPoint(VM vm) {
    return _last;
  }

  /**
   * Removes all nodes after the checkpoint from the list
   * @param vm The virtual machine
   * @param checkPoint An iterator on the list
   */
  inline
  void reset(VM vm, CheckPoint checkPoint);

  /**
   * Removes all nodes after the checkpoint from the list
   * @param vm The virtual machine
   */
  inline
  void reset(VM vm);

  /**
   * Tries to fetch the next checkpoint in the list
   * @param vm The virtual machine
   * @param identity The identity name
   * @param args The native values
   * @returns True if the fetch succeeded (last checkpoint matched identity(args))
   *          else false or because the end of list is reached
   * @note A tuple is formed as identity(args)
   */
  template <typename... Args>
  inline
  bool fetch(VM vm, const char* identity, Args... args);

  /**
   * Stores an new tuple as a check point at the end of the list
   * @param vm The virtual machine
   * @param identity The identity name
   * @param args The native rvalues
   * @note A tuple is formed as identity(args)
   */
  template <typename... Args>
  inline
  void store(VM vm, const char* identity, Args&&... args);

  /**
   * Resets and stores the check point defined by a tuple
   * @param vm The virtual machine
   * @param checkpoint The checkpoint in the list
   * @param identity The identity name
   * @param args The native rvalues
   * @note A tuple is formed as identity(args)
   */
  template <typename... Args>
  inline
  void resetAndStore(VM vm, CheckPoint checkPoint, const char* identity,
                     Args&&... args);

  /**
   * Resets the cursor at the beginning of the list
   * @param vm The virtual machine
   */
  inline
  void rewind(VM vm);
private:
  List _list;
  List::iterator _last;
};

//////////////
// Runnable //
//////////////

class Runnable {
public:
  static size_t _everCreatedThreadsCount;

  struct Statistics {
    size_t runsCount = 0;
    size_t resumesCount = 0;
    size_t suspendsCount = 0;
    size_t suspendsOnVarCount = 0;
  };
public:
  /**
   * Initializes a thread defined as a VM, a space and a priority level
   * @param vm The virtual machine
   * @param space The space for the thread
   * @param priority The priority level
   */
  inline
  Runnable(VM vm, Space* space, ThreadPriority priority = tpMiddle);

  /**
   * Initializes a thread from an other thread
   * @param gr The graph replicator to copy the space of the parent thread
   * @param from A reference on the parent thread to copy
   */
  inline
  Runnable(GR gr, Runnable& from);

  inline
  ~Runnable();

public:
  /** Gets the space of the thread */
  Space* getSpace() {
    return _space;
  }

  /** @returns The thread id representing that the thread is the 'id' th
   * thread ever created
   */
  size_t getId() { return _id; }

  /** @returns The thread kind id representing from which code this thread
   * has been generated
   */
  size_t getKindId() { return _kindId; }

  /** @returns The thread generation id representing the nth thread generated
   * from the same code
   */
  size_t getGenerationId() { return _generationId; }

  /** @returns The priority of the thread */
  ThreadPriority getPriority() { return _priority; }

  Statistics getStatistics() { return _statistics; }

  /**
   * Sets the priority of the thread
   * @param priority The thread priority level
   * @note It might force the thread pool to reschedule the thread
   */
  inline
  void setPriority(ThreadPriority priority);

  /**
   * Runs the thread
   */
  inline
  size_t run(size_t maxInstructionsNumber = SIZE_MAX);

  virtual size_t doRun(size_t maxInstructionsNumber) = 0;

  /** Tells if the thread is runnable */
  inline
  bool isRunnable() { return _runnable; }

  /** Tells if the thread is terminated */
  inline
  bool isTerminated() { return _terminated; }

  /** Tells if the thread is dead */
  inline
  bool isDead() { return _dead; }

  inline
  bool isAlive() {
    return !_dead && !_terminated;
  }

  inline
  bool isPreempted() {
    return _preempted || _terminated || _dead || !_runnable;
  }

  /** Tells if the thread is preemptible */
  inline
  bool isPreemptible() { return _preemptible; }

  inline
  void setPreemptible(bool preemptible);

  /**
   * Resumes the thread
   * @param skipSchedule Decides if the thread has to be scheduled in the thread pool or not
   * @note The thread has to be alive, non-terminated and non-runnable
   */
  inline
  virtual void resume(bool skipSchedule = false);

  /**
   * Suspends the thread
   * @param skipUnschedule Decides if the thread has to be unscheduled in the thread pool or not
   * @note The thread has to be alive, non-terminated and runnable
   */
  inline
  virtual void suspend(bool skipUnschedule = false);

  /**
   * Suspends the thread on a dataflow variable
   * @param vm The virtual machine
   * @param variable A rich node representing the suspending variable
   * @param skipUnschedule Decides if the thread has to be unscheduled in the thread pool or not
   * @note The variable must be transient and not a failed value
   */
  inline
  void suspendOnVar(VM vm, RichNode variable, bool skipUnschedule = true);

  inline
  void preempt();

  /**
   * Kills/Disposes the thread
   * @note The thread must not be dead or terminated
   */
  inline
  virtual void kill();

  /** Should be never called */
  inline
  virtual void injectException(StableNode* exception) {
    /* We cannot come here because there's no way the Oz code has gained a
     * reference to a thread that is not an emulated thread.
     */
    assert(false);
  }

  /** Gets raise on block */
  bool getRaiseOnBlock() {
    return _raiseOnBlock;
  }

  /** Sets raise on block */
  void setRaiseOnBlock(bool value) {
    _raiseOnBlock = value;
  }

  /** Gets intermediate state */
  IntermediateState& getIntermediateState() {
    return _intermediateState;
  }

  /**
   * Prepares the thread for graph replication
   */
  virtual void beforeGR() {}

  /**
   * Prepares the thread to get back to normal after graph replication
   */
  virtual void afterGR() {}

  /**
   * Runs the garbage collector on the thread
   * @param gc The garbage collector
   * @returns A pointer on the replicated runnable
   */
  virtual Runnable* gCollect(GC gc) = 0;

  /**
   * Runs the space cloner on the thread
   * @param sc The space cloner
   * @returns A pointer on the replicated runnable
   */
  virtual Runnable* sClone(SC sc) = 0;

  /**
   * Runs the garbage collector on the thread
   * @param gc The garbage collector
   * @returns A pointer on the replicated runnable
   */
  inline
  Runnable* gCollectOuter(GC gc);

  /**
   * Runs the space cloner on the thread
   * @param sc The space cloner
   * @returns A pointer on the replicated runnable
   */
  inline
  Runnable* sCloneOuter(SC sc);

  /** Restores after graph replication */
  inline
  void restoreAfterGR();
protected:
  /**
   * Terminates the thread
   * @note The thread must be alive, non-terminated and runnable
   */
  inline
  virtual void terminate();

  /** Disposes/Kills the thread */
  inline
  virtual void dispose();
public:
  /** Dumps the thread */
  virtual void dump() {}
protected:
  VM vm;
  const size_t _id, _kindId, _generationId;
private:
  friend class RunnableList;
  friend class Introspection;

  SpaceRef _space;

  ThreadPriority _priority;

  Statistics _statistics;

  bool _runnable;
  bool _terminated;
  bool _dead;
  bool _preempted; // Is the thread preempted right now ?
  bool _preemptible; // Can be the thread preempted during its execution ? => For system threads

  bool _raiseOnBlock;
  StableNode _reification;

  IntermediateState _intermediateState;

  Runnable* _replicate;

  Runnable* _previous;
  Runnable* _next;
};

class RunnableList {
public:
  /** Defines a whole iterator for a linked-list like */
  struct iterator {
  public:
    iterator(Runnable* node) : node(node) {}

    bool operator==(const iterator& other) {
      return node == other.node;
    }

    bool operator!=(const iterator& other) {
      return node != other.node;
    }

    iterator operator++() {
      node = node->_next;
      return *this;
    }

    iterator operator++(int) {
      iterator result = *this;
      node = node->_next;
      return result;
    }

    Runnable* operator*() {
      return node;
    }
  private:
    Runnable* node;
  };
public:
  /** Initializes a runnable list */
  RunnableList() : first(nullptr), last(nullptr), length(0) {}

  /** Gets the beginning of the list */
  iterator begin() {
    return iterator(first);
  }

  /** Gets the end of the list */
  iterator end() {
    return iterator(nullptr);
  }

  size_t size() {
    return length;
  }
private:
  friend class Runnable;
  friend class Introspection;

  /**
   * Inserts a new runnable address at the end of the list
   * @param item The address of the runnable
   */
  void insert(Runnable* item) {
    item->_previous = last;
    item->_next = nullptr;

    if (first == nullptr)
      first = item;
    else
      last->_next = item;

    last = item;

    length++;
  }

  /**
   * Removes a runnable address from the beginning of the list
   * @param item The address of the runnable
   */
  void remove(Runnable* item) {
    assert(length > 0);

    if (item->_previous == nullptr)
      first = item->_next;
    else
      item->_previous->_next = item->_next;

    if (item->_next == nullptr)
      last = item->_previous;
    else
      item->_next->_previous = item->_previous;
    
    length--;
  }

  Runnable* first;
  Runnable* last;
  size_t length; // Used by the introspection to gain performance
};

}

#endif // MOZART_RUNNABLE_DECL_H
