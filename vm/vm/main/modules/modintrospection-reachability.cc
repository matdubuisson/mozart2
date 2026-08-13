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

#include "../mozart.hh"
#include "modintrospection.hh"

namespace mozart {

namespace builtins {

UnstableNode ModIntrospection::buildReachabilityMapRecord(VM vm, Introspection::IdToIdsMap& map) {
  // for (Introspection::IdToIdsMap::iterator iter = map.begin();
  //   iter != map.end(); ++iter) {
  //   size_t fromId = iter->first;
  //   Introspection::IdsVector vector = iter->second;
  //   char numbers[vector.size()][21];

  //   size_t i = 0;
  //   for (Introspection::IdsVector::iterator iter2 = vector.begin();
  //     iter2 != vector.end(); ++iter2) {
  //     numbers[i++] = std::to_string(*iter2).c_str();
      
  //   }
  // }
  return build(vm, "none");
}

UnstableNode ModIntrospection::buildReachabilityGraphRecord(VM vm, Introspection::ReachabilityGraph& graph) {
  return buildRecord(vm,
    buildArity(vm,
      "graph",
      "threadToVariables",
      "variableToThreads"
    ),
    buildReachabilityMapRecord(vm, graph.threadToVariables),
    buildReachabilityMapRecord(vm, graph.variableToThreads)
  );
}

void ModIntrospection::GetReachabilityGraph::call(VM vm, Out result) {
  Introspection::ReachabilityGraph graph = vm->getIntrospection()
    .computeReachabilityGraph(vm);
  result = buildReachabilityGraphRecord(vm, graph);
}

}

}