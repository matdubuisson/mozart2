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

#include <iostream>

namespace mozart {

template<class Object>
static inline
void printId(AdvancedIdentifiable<Object>& identifiable) {
  std::cout << "\t- ("
    << identifiable.getId()
    << ", " << identifiable.getKindId()
    << ", " << identifiable.getGenerationId()
    << ")" << std::endl;
}

template<class SrcType, class DstType>
void transmitIds(VM vm, SrcType& src, DstType& dst) {
  if constexpr (!isIdentifiable<SrcType>()
    || !isIdentifiable<DstType>()) return;

  AdvancedIdentifiable<SrcType>& srcId = src.getAdvancedIdentity();
  AdvancedIdentifiable<DstType>& dstId = dst.getAdvancedIdentity();

  Runnable* currentThread = vm->getCurrentThread();

  // if (currentThread != nullptr && currentThread->getPriority() != tpSystem
  //   && srcId.getId() == 222222) {
  //   if constexpr (isVariable<SrcType>() && isVariable<DstType>()) {
  //     std::cout << "VtoV => " << std::endl;
  //     printId<SrcType>(srcId);
  //     printId<DstType>(dstId);
  //   } else if constexpr (isStructure<SrcType>() && isVariable<DstType>()) {
  //     std::cout << "StoV => " << std::endl;
  //     printId<SrcType>(srcId);
  //     printId<DstType>(dstId);
  //   } else if constexpr (isVariable<SrcType>() && isStructure<DstType>()) {
  //     std::cout << "VtoS => " << std::endl;
  //     printId<SrcType>(srcId);
  //     printId<DstType>(dstId);
  //   } else if constexpr (isStructure<SrcType>() && isStructure<DstType>()) {
  //     std::cout << "StoS => " << std::endl;
  //     printId<SrcType>(srcId);
  //     printId<DstType>(dstId);
  //   }
  // }

  if constexpr (isVariable<DstType>()) {
    dstId.followIdentity(srcId, false);
  } else if constexpr (isStructure<DstType>()) {
    dstId.followIdentity(srcId, true);
  }
}

template<class SrcType>
void transmitIds(VM vm, SrcType& srcObject, RichNode dst) {
  if (dst.is<Variable>()) {
    transmitIds<SrcType, Variable>(vm, srcObject, dst);
  } else if (dst.is<ReadOnlyVariable>()) {
    transmitIds<SrcType, ReadOnlyVariable>(vm, srcObject, dst);
  } else if (dst.is<Cons>()) {
    transmitIds<SrcType, Cons>(vm, srcObject, dst);
  } else if (dst.is<Reference>()) {
    transmitIds<SrcType>(vm, srcObject, *dst.as<Reference>().dest());
  }
}

void transmitIds(VM vm, RichNode src, RichNode dst) {
  if (src.is<Variable>()) {
    transmitIds<Variable>(vm, src, dst);
  } else if (src.is<ReadOnlyVariable>()) {
    transmitIds<ReadOnlyVariable>(vm, src, dst);
  } else if (src.is<Cons>()) {
    transmitIds<Cons>(vm, src, dst);
  } else if (src.is<Reference>()) {
    transmitIds(vm, *src.as<Reference>().dest(), dst);
  }
}

void ozListWatch(VM vm, RichNode list) {
  RichNode previous = list;
  RichNode current = getNext(vm, list);

  while (current.is<Cons>()) {
    printId(current.as<Cons>().getSelf());
    previous = current;
    current = getNext(vm, current);
  }
}

}