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

#ifndef MOZART_VMEVENTMANAGER_DECL_H
#define MOZART_VMEVENTMANAGER_DECL_H

#include <cstdlib>

#include "core-forward-decl.hh"
#include "identity-decl.hh"

#include "runnable-decl.hh"
#include "store-decl.hh"

namespace mozart {

////////////////////////////////
// VirtualMachineEventManager //
////////////////////////////////

class VirtualMachineEventManager {
public:
  VirtualMachineEventManager() : trackingTriggered(false) {}

public:
  using Id = size_t;

  using IdsVector = std::vector<Id>;

  static inline
  UnstableNode buildIdsVector(VM vm, const IdsVector& vector);

public:
  struct Tracking {
  protected:
    Tracking(VM vm, Id authorId, IdsVector victimIds) :
      authorId(authorId), victimIds(victimIds) {}
  public:
    Id authorId;
    IdsVector victimIds;
  };

  /**
   * Note tracking rules could be composed as for example
   * Runnable vectors where there is one vector for each type of event
   * making event lighter (minus one announce to store) and faster to
   * filter based on the announce.
   * 
   * However tracking rules are hand written and will be probably less
   * numberous compared to events thus the iteration of the tracking vector
   * should be quite short.
   */
  using TrackingVector = std::vector<Tracking>;

private:
  static inline
  Runnable* getCurrentThread(VM vm);

  static inline
  bool contains(const IdsVector& idsVector, Id id);

  /** ================ Runnable events ================ */
public:
  enum RunnableAnnounce {
    Created,
    Updated,
    Collected,
    Scheduled,
    Unscheduled,
    Rescheduled
  };

  struct RunnableEvent {
    explicit RunnableEvent(VM vm, Runnable* runnable) : author(nullptr), runnable(runnable) {
      author = getCurrentThread(vm);
    }

    explicit RunnableEvent(VM vm, Id id) : id(id) {}
    
    union {
      struct {
        Runnable* author;
        Runnable* runnable;
      };

      struct {
        Id id;
      };
    };
  };

  static inline
  UnstableNode buildRunnableEvent(VM vm,
    const RunnableEvent& event, RunnableAnnounce announce);

  using RunnableEventsVector = std::vector<RunnableEvent>;

  static inline
  UnstableNode buildRunnableEventsVector(VM vm,
    const RunnableEventsVector& vector, RunnableAnnounce announce);

  struct RunnableEventsVectors {
    RunnableEventsVector created;
    RunnableEventsVector updated;
    RunnableEventsVector collected; // When event is read, the collected thread state has been destroyed
    RunnableEventsVector scheduled;
    RunnableEventsVector unscheduled;
    RunnableEventsVector rescheduled;

    void clear() {
      created.clear();
      updated.clear();
      collected.clear();
      scheduled.clear();
      unscheduled.clear();
      rescheduled.clear();
    }
  };

  static inline
  UnstableNode buildRunnableEventsVectors(VM vm,
    const RunnableEventsVectors& vectors);

public:
  void announceRunnable(VM vm, Runnable* runnable, RunnableAnnounce announce) {
    RunnableEvent info = RunnableEvent(vm, runnable);

    detectTriggeredRunnableTracking(vm, info, announce);

    switch (announce) {
      case Created:
        runnableEventsVectors.created.push_back(RunnableEvent(vm, runnable));
        break;
      case Updated:
        runnableEventsVectors.updated.push_back(RunnableEvent(vm, runnable));
        break;
      case Collected:
        runnableEventsVectors.collected.push_back(
          RunnableEvent(vm, runnable->getId()));
        break;
      case Scheduled:
        runnableEventsVectors.scheduled.push_back(RunnableEvent(vm, runnable));
        break;
      case Unscheduled:
        runnableEventsVectors.unscheduled.push_back(RunnableEvent(vm, runnable));
        break;
      case Rescheduled:
        runnableEventsVectors.rescheduled.push_back(RunnableEvent(vm, runnable));
        break;
      default: assert(false);
    }
  }

  RunnableEventsVector& getRunnables(RunnableAnnounce announce) {
    switch (announce) {
      case Created:
        return runnableEventsVectors.created;
      case Updated:
        return runnableEventsVectors.updated;
      case Collected:
        return runnableEventsVectors.collected;
      case Scheduled:
        return runnableEventsVectors.scheduled;
      case Unscheduled:
        return runnableEventsVectors.unscheduled;
      case Rescheduled:
        return runnableEventsVectors.rescheduled;
      default: assert(false); return getRunnables(Created);
    }
  }

  bool empty(RunnableAnnounce announce) {
    return getRunnables(announce).empty();
  }

private:
  RunnableEventsVectors runnableEventsVectors;

  /** ================ Runnable tracks ================ */
public:
  struct RunnableTracking : public Tracking {
    explicit RunnableTracking(VM vm, size_t authorId,
      IdsVector victimIds, RunnableAnnounce announce) :
      Tracking(vm, authorId, victimIds),
      announce(announce) {}

    RunnableAnnounce announce;
  };

  static inline
  UnstableNode buildRunnableTrack(VM vm, const RunnableTracking& track);

  using RunnableTrackingVector = std::vector<RunnableTracking>;

  static inline
  UnstableNode buildRunnableTracks(VM vm, const RunnableTrackingVector& vector);

private:
  inline
  bool matchTracking(VM vm, const RunnableTracking& tracking, const RunnableEvent& event);

  inline
  void detectTriggeredRunnableTracking(VM vm, const RunnableEvent& event, RunnableAnnounce announce) {
    for (RunnableTracking tracking : runnableTrackingVector) {
      if (tracking.announce == announce && matchTracking(vm, tracking, event)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  inline
  void detectTriggeredRunnableTracking(VM vm, const RunnableTracking& tracking) {
    for (const RunnableEvent event : getRunnables(tracking.announce)) {
      if (matchTracking(vm, tracking, event)) {
        trackingTriggered = true;
        break;
      }
    }
  }

public:
  void track(RunnableTracking tracking) {
    runnableTrackingVector.push_back(tracking);
  }

private:
  RunnableTrackingVector runnableTrackingVector;

  /** ================ Variable events ================ */
public:
  enum VariableAnnounce {
    VCreated,
    VCollected,
    Needed,
    Waited,
    Bound
  };

  template<class V>
  struct VariableEvent {
    explicit VariableEvent(VM vm, V* v) :
      author(nullptr), variable(v) {
      author = getCurrentThread(vm);
    }

    explicit VariableEvent(VM vm, Id id) : id(id) {}
    
    union {
      struct {
        Runnable* author;
        V* variable;
      };

      struct {
        Id id;
      };
    };
  };

  template<class V>
  static inline
  UnstableNode buildVariableEvent(VM vm,
    const VariableEvent<V>& event, VariableAnnounce announce);

  template<class V>
  using VariableEventsVector = std::vector<VariableEvent<V>>;

  template<class V>
  static inline
  UnstableNode buildVariableEventsVector(VM vm,
    const VariableEventsVector<V>& vector, VariableAnnounce announce);

  template<class V>
  struct VariableEventsVectors {
    VariableEventsVector<V> created;
    VariableEventsVector<V> collected;
    VariableEventsVector<V> needed;
    VariableEventsVector<V> bound;
    VariableEventsVector<V> waited;

    void clear() {
      created.clear();
      collected.clear();
      needed.clear();
      bound.clear();
      waited.clear();
    }
  };

  template<class V>
  static inline
  UnstableNode buildVariableEventsVectors(VM vm,
    const VariableEventsVectors<V>& vectors);

private:
  template<class V>
  inline
  VariableEventsVectors<V>& getVariableEventsVectors() {
    if constexpr (std::is_same_v<V, Variable>)
      return variableEventsVectors;
    else
      return readOnlyVariableEventsVectors;
  }

public:
  template<class V>
  inline
  void announceVariable(VM vm, V* variable, VariableAnnounce announce) {
    VariableEventsVectors<V>& vectors = getVariableEventsVectors<V>();

    detectTriggeredVariableTracking(vm, VariableEvent(vm, variable), announce);

    switch (announce) {
      case VCreated:
        vectors.created.push_back(VariableEvent(vm, variable));
        break;
      case VCollected:
        vectors.collected.push_back(VariableEvent(vm, variable));
        break;
      case Needed:
        vectors.needed.push_back(VariableEvent(vm, variable));
        break;
      case Bound:
        vectors.bound.push_back(VariableEvent(vm, variable));
        break;
      case Waited:
        vectors.waited.push_back(VariableEvent(vm, variable));
        break;
    }
  }

private:
  template<typename V>
  VariableEventsVector<V>& getVariables(VariableEventsVectors<V>& vectors, VariableAnnounce announce) {
    switch (announce) {
      case VCreated:
        return vectors.created;
      case VCollected:
        return vectors.collected;
      case Needed:
        return vectors.needed;
      case Bound:
        return vectors.bound;
      case Waited:
        return vectors.waited;
      default: assert(false); return vectors.created;
    }
  }

public:
  template<typename V>
  VariableEventsVector<V>& getVariables(VariableAnnounce announce) {
    return getVariables<V>(getVariableEventsVectors<V>(), announce);
  }

  template<typename V>
  bool empty(VariableAnnounce announce) {
    return getVariables<V>(announce).empty();
  }

  bool empty(VariableAnnounce announce) {
    return empty<Variable>(announce)
      && empty<ReadOnlyVariable>(announce);
  }

private:
  VariableEventsVectors<Variable> variableEventsVectors;
  VariableEventsVectors<ReadOnlyVariable> readOnlyVariableEventsVectors;

  /** ================ Variable tracks ================ */
public:
  struct VariableTracking : public Tracking {
    explicit VariableTracking(VM vm, Id authorId,
      IdsVector victimIds, VariableAnnounce announce) :
      Tracking(vm, authorId, victimIds),
      announce(announce) {}

    VariableAnnounce announce;
  };

  static inline
  UnstableNode buildVariableTrack(VM vm, const VariableTracking& track);

  using VariableTrackingVector = std::vector<VariableTracking>;

  static inline
  UnstableNode buildVariableTracks(VM vm, const VariableTrackingVector& vector);

private:
  template<class V>
  inline
  bool matchTracking(VM vm, const VariableTracking& tracking, const VariableEvent<V>& event);

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, const VariableEvent<V>& event, VariableAnnounce announce) {
    for (VariableTracking tracking : variableTrackingVector) {
      if (tracking.announce == announce && matchTracking(vm, tracking, event)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    VariableEventsVector<V>& vector, VariableAnnounce announce) {
    for (VariableEvent<V> event : vector) {
      if (matchTracking(vm, tracking, event)) {
        trackingTriggered = true;
        break;
      }
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking,
    VariableEventsVectors<V>& vectors) {
    switch (tracking.announce) {
      case VCreated:
        detectTriggeredVariableTracking<V>(vm, tracking, vectors.created, VCreated);
        break;
      case VCollected:
        detectTriggeredVariableTracking<V>(vm, tracking, vectors.collected, VCollected);
        break;
      case Needed:
        detectTriggeredVariableTracking<V>(vm, tracking, vectors.needed, Needed);
        break;
      case Waited:
        detectTriggeredVariableTracking<V>(vm, tracking, vectors.waited, Waited);
        break;
      case Bound:
        detectTriggeredVariableTracking<V>(vm, tracking, vectors.bound, Bound);
        break;
    }
  }

  template<class V>
  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking) {
    VariableEventsVectors<V> vectors = getVariableEventsVectors<V>();
    detectTriggeredVariableTracking<V>(vm, tracking, vectors);
  }


  inline
  void detectTriggeredVariableTracking(VM vm, VariableTracking& tracking) {
    detectTriggeredVariableTracking<Variable>(vm, tracking);
    detectTriggeredVariableTracking<ReadOnlyVariable>(vm, tracking);
  }

public:
  void track(VariableTracking tracking) {
    variableTrackingVector.push_back(tracking);
  }

private:
  VariableTrackingVector variableTrackingVector;

  /** ================ Other ================ */
public:
  static inline
  UnstableNode buildEventsJournal(VM vm, const VirtualMachineEventManager& eventManager);

  static inline
  UnstableNode buildTracks(VM vm, const VirtualMachineEventManager& eventManager);

  bool isTrackingTriggered() {
    return trackingTriggered;
  }

  void clear() {
    runnableEventsVectors.clear();
    variableEventsVectors.clear();
    readOnlyVariableEventsVectors.clear();
  }

private:
  bool trackingTriggered;
};

}

#endif // MOZART_EVENTMANAGER_DECL_H
