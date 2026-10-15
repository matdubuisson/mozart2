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

#ifndef MOZART_MODJOURNAL_H
#define MOZART_MODJOURNAL_H

#include "../mozartcore.hh"
#include "../introspection.hh"
#include "../emulate.hh"

#include <iostream>

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

class ModEventManager: public Module {
public:
  ModEventManager(): Module("EventManager") {}

public:
  class GetEventsJournal: public Builtin<GetEventsJournal> {
  public:
    GetEventsJournal(): Builtin("getEventsJournal") {}

    static void call(VM vm, Out result) {
      result = VirtualMachineEventManager::buildEventsJournal(vm, vm->getEventManager());
    }
  };

public:
  class Track: public Builtin<Track> {
  public:
    Track(): Builtin("track") {}

    static void call(VM vm, In event, In announce, In announcerId, In idsList);
  };

  class GetTracks: public Builtin<GetTracks> {
  public:
    GetTracks(): Builtin("getTracks") {}

    static void call(VM vm, Out result) {
      result = VirtualMachineEventManager::buildTracks(vm, vm->getEventManager());
    }
  };

  class IsTrackingTriggered: public Builtin<IsTrackingTriggered> {
  public:
    IsTrackingTriggered(): Builtin("isTrackingTriggered") {}

    static void call(VM vm, Out result) {
      result = build(vm,
        vm->getEventManager().isTrackingTriggered());
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // MOZART_MODINTROSPECTION_H
