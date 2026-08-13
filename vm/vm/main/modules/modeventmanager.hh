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

  static inline
  std::string nodeToString(VM vm, RichNode node) {
    auto& config = vm->getPropertyRegistry().config;
    std::basic_stringstream<char> buffer;
    buffer << repr(vm, node, config.printDepth, config.printWidth);
    return buffer.str();
  }

private:
  using RunnableInfo = VirtualMachineEventManager::RunnableInfo;
  using RunnablesVector = VirtualMachineEventManager::RunnablesVector;
  using RunnableAnnounce = VirtualMachineEventManager::RunnableAnnounce;
  // using RunnableToRecordLambda = std::function<UnstableNode(VM, Runnable*)>;

  static
  UnstableNode buildThreadStateRecord(VM vm, Runnable* runnable);

  static
  UnstableNode buildRunnablesList(VM vm, RunnablesVector& runnables);

  static
  UnstableNode buildRunnablesJournalRecord(VM vm, VirtualMachineEventManager& journal);

private:
  template<typename V>
  using VariableInfo = VirtualMachineEventManager::VariableInfo<V>;  
  template<typename V>
  using VariablesVector = VirtualMachineEventManager::VariablesVector<V>;
  template<typename V>
  using BoundVariableInfo = VirtualMachineEventManager::BoundVariableInfo<V>;
  template<typename V>
  using BoundVariablesVector = VirtualMachineEventManager::BoundVariablesVector<V>;
  template<typename V>
  using WaitedVariableInfo = VirtualMachineEventManager::WaitedVariableInfo<V>;
  template<typename V>
  using WaitedVariablesVector = VirtualMachineEventManager::WaitedVariablesVector<V>;
  template<typename V>
  using VariablesVectors = VirtualMachineEventManager::VariablesVectors<V>;
  using VariableAnnounce = VirtualMachineEventManager::VariableAnnounce;

  // template<typename V>
  // using VariableToRecordLambda = std::function<UnstableNode(VM, V*)>;
  // template<typename V>
  // using WaitedVariableToRecordLambda = std::function<UnstableNode(VM, WaitedVariable<V>*)>;

  template<typename V>
  static
  void fillInfo(VM vm, V* variable, size_t& id, std::string& type,
    bool& isNeeded, bool& isBound, bool& isWaited);

  template<typename V>
  static
  UnstableNode buildVariableStateRecord(VM vm, V* variable);

  template<typename V>
  static
  UnstableNode buildVariableStateRecord(VM vm, V* variable, RichNode self, RichNode src);

  static
  UnstableNode buildWaiterStateRecord(VM vm, RichNode waiter);


  template<typename V>
  static
  UnstableNode buildVariableStateRecord(VM vm, V* variable, RichNode waiter);

  template<typename V>
  static
  UnstableNode buildVariablesList(VM vm, VariablesVector<V>& variables);

  template<typename V>
  static
  UnstableNode buildBoundVariablesList(VM vm, BoundVariablesVector<V>& variables);

  template<typename V>
  static
  UnstableNode buildWaitedVariablesList(VM vm, WaitedVariablesVector<V>& variables);

  template<typename V>
  static
  UnstableNode buildVariablesSubJournalRecord(VM vm, VirtualMachineEventManager& journal);

  static
  UnstableNode buildVariablesJournalRecord(VM vm, VirtualMachineEventManager& journal);

  template<class S>
  using StructureInfo = VirtualMachineEventManager::StructureInfo<S>;
  template<class S>
  using StructuresVector = VirtualMachineEventManager::StructuresVector<S>;
  template<class S>
  using StructuresVectors = VirtualMachineEventManager::StructuresVectors<S>;

  template<class S>
  static
  UnstableNode buildStructuresList(VM vm, StructuresVector<S>& vector);

  template<class S>
  static
  UnstableNode buildStructuresSubJournalRecord(VM vm, VirtualMachineEventManager& journal);

  static
  UnstableNode buildStructuresJournalRecord(VM vm, VirtualMachineEventManager& journal);

public:
  class GetJournal: public Builtin<GetJournal> {
  public:
    GetJournal(): Builtin("getJournal") {}

    static void call(VM vm, Out result);
  };

  // class GetAggregatedJournal: public Builtin<GetAggregatedJournal> {
  // public:
  //   GetAggregatedJournal(): Builtin("getAggregatedJournal") {}

  //   static void call(VM vm, Out result) {
  //     VirtualMachineEventManager& journal = vm->getEventManager();
      
  //     result = buildRecord(vm,
  //       buildArity(vm,
  //         "journal",
  //         "runnables",
  //         "structures",
  //         "variables"
  //       ),
  //       buildRunnablesJournalRecord(vm, journal),
  //       buildStructuresJournalRecord(vm, journal),
  //       buildVariablesJournalRecord(vm, journal)
  //     );
  //   }
  // };


public:

  class Track: public Builtin<Track> {
  public:
    Track(): Builtin("track") {}

    static void call(VM vm, In event, In announce, In announcerId, In idsList);
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
