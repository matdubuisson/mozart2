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

UnstableNode ModIntrospection::buildListNodeRecord(VM vm, Introspection::OwnedRichNode& ownedNode) {
  Introspection::RunnableVector runnables = ownedNode.runnables;
  RichNode node = ownedNode.node;

  OzListBuilder builder(vm);
  for (auto iter = runnables.begin(); iter != runnables.end(); ++iter) {
    Runnable* runnable = *iter;
    builder.push_back(vm, build(vm, runnable->getId()));
  }

  //ozListPropagateKind(vm, node);
  //std::cout << "Cons: " << node.as<Cons>().getId() << std::endl;
  //ozListWatch(vm, node);

  TypedRichNode<Cons> cons = node.as<Cons>();

  return buildRecord(vm,
    buildArity(vm,
      "list",
      "generationId",
      "hash",
      "id",
      "kindId",
      "list",
      "owners"
    ),
    build(vm, cons.getGenerationId()),
    build(vm, ozListHash(vm, node)),
    build(vm, cons.getId()),
    build(vm, cons.getKindId()),
    Reference::build(vm, node.getStableRef(vm)),
    builder.get(vm)
  );
}

UnstableNode ModIntrospection::buildListNodesListRecord(VM vm, RichNode idsList, Introspection::NodesMap& map) {
  std::unordered_set<size_t> set;

  bool isNil = ozListIsNil(vm, idsList);

  if (!isNil) {
    ozListForEach(vm, idsList, [&set](nativeint id) {
      set.insert(id);
    }, "List of integer ids");
  }

  OzListBuilder builder(vm);
  for (auto iter = map.begin(); iter != map.end(); ++iter) {
    // size_t nodeId = iter->first;
    Introspection::OwnedRichNode& ownedNode = iter->second;

    Cons& cons = ownedNode.node.as<Cons>().getSelf();

    if (isNil
      || set.find(cons.getId()) != set.end()
      || set.find(cons.getKindId()) != set.end()
    ) {
      builder.push_back(vm, buildListNodeRecord(vm, ownedNode));
    }
  }
  return builder.get(vm);
}

void ModIntrospection::GetThreadLists::call(VM vm, In runnableNode, In idsList, Out result) {
  Runnable* runnable = getArgument<Runnable*>(vm, runnableNode);
  Introspection::NodesMap map = vm->getIntrospection()
    .getLists(vm, runnable);
  result = buildListNodesListRecord(vm, idsList, map);
}

void ModIntrospection::GetLists::call(VM vm, In idsList, Out result) {
  Introspection::NodesMap map = vm->getIntrospection().getLists(vm);
  result = buildListNodesListRecord(vm, idsList, map);
}

}

}