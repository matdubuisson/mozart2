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

// Nodes counters

void ModIntrospection::GetNodesCounts::call(VM vm, Out result) {
  Introspection::NodesCounts counts =
    vm->getIntrospection().getNodesCounts(vm);
  result = Introspection::buildNodesCounts(vm, counts);
}

// Register types sizes

using NodesRegister = Introspection::NodesRegister;

UnstableNode ModIntrospection::getThreadNodesRegisterSize(VM vm, In threadNode, In depthNode,
  NodesRegister nodesRegister) {

  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t depth = getArgument<size_t>(vm, depthNode);
  Introspection& introspection = vm->getIntrospection();

  size_t count = 0;
  switch (nodesRegister) {
    case NodesRegister::yRegister:
      count = introspection.getYNodesRegisterSize(vm, runnable, depth); break;
    case NodesRegister::gRegister:
      count = introspection.getGNodesRegisterSize(vm, runnable, depth); break;
    case NodesRegister::kRegister:
      count = introspection.getKNodesRegisterSize(vm, runnable, depth); break;
    default: assert(false);
  }

  return build(vm, count);
}

// Nodes getters

UnstableNode ModIntrospection::getThreadXNode(VM vm, In threadNode, In indexNode) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t index = getArgument<size_t>(vm, indexNode);
  Introspection& introspection = vm->getIntrospection();

  assert(index < introspection.getXNodesRegisterSize(vm, runnable));
  RichNode node = introspection.getXNode(vm, runnable, index);
  return Introspection::buildNode(vm, node);
}

UnstableNode ModIntrospection::getThreadNode(VM vm, In threadNode, In depthNode,
  In indexNode, NodesRegister nodesRegister) {
  
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t depth = getArgument<size_t>(vm, depthNode);
  size_t index = getArgument<size_t>(vm, indexNode);
  Introspection& introspection = vm->getIntrospection();

  assert(depth < introspection.getStackDepth(vm, runnable));
  RichNode node;
  switch (nodesRegister) {
    case NodesRegister::yRegister: {
      assert(index < introspection.getYNodesRegisterSize(vm, runnable, depth));
      node = introspection.getYNode(vm, runnable, depth, index);
      break;
    } case NodesRegister::gRegister: {
      assert(index < introspection.getGNodesRegisterSize(vm, runnable, depth));
      node = introspection.getGNode(vm, runnable, depth, index);
      break;
    } case NodesRegister::kRegister: {
      assert(index < introspection.getKNodesRegisterSize(vm, runnable, depth));
      node = introspection.getKNode(vm, runnable, depth, index);
      break;
    } default: assert(false);
  }

  return Introspection::buildNode(vm, node);
}

// Nodes lists getters

UnstableNode ModIntrospection::getThreadXNodes(VM vm, In threadNode, In fromNode, In toNode) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  Introspection& introspection = vm->getIntrospection();

  assert(from < to);
  // assert(to < introspection.getXNodesRegisterSize(vm, runnable));

  OzListBuilder builder(vm);
  introspection.doForEachXNode(vm, runnable, from, to,
    Introspection::allNodes,
    [&builder](VM vm, Runnable* runnable, RichNode node) {
      builder.push_back(vm, Introspection::buildNode(vm, node));
    }
  );
  return builder.get(vm);
}

UnstableNode ModIntrospection::getThreadNodes(VM vm, In threadNode, In depthNode, In fromNode, In toNode,
  NodesRegister nodesRegister) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t depth = getArgument<size_t>(vm, depthNode);
  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  Introspection& introspection = vm->getIntrospection();

  assert(depth < introspection.getStackDepth(vm, runnable));
  assert(from < to);

  OzListBuilder builder(vm);

  switch (nodesRegister) {
    case NodesRegister::yRegister: {
      // assert(to < introspection.getYNodesRegisterSize(vm, runnable, depth));
      introspection.doForEachYNode(vm, runnable, depth, from, to,
        Introspection::allNodes,
        [&builder](VM vm, Runnable* runnable, RichNode node) {
          builder.push_back(vm, Introspection::buildNode(vm, node));
        }
      );
      break;
    } case NodesRegister::gRegister: {
      // assert(to < introspection.getGNodesRegisterSize(vm, runnable, depth));
      introspection.doForEachGNode(vm, runnable, depth, from, to,
        Introspection::allNodes,
        [&builder](VM vm, Runnable* runnable, RichNode node) {
          builder.push_back(vm, Introspection::buildNode(vm, node));
        }
      );
      break;
    } case NodesRegister::kRegister: {
      // assert(to < introspection.getKNodesRegisterSize(vm, runnable, depth));
      introspection.doForEachKNode(vm, runnable, depth, from, to,
        Introspection::allNodes,
        [&builder](VM vm, Runnable* runnable, RichNode node) {
          builder.push_back(vm, Introspection::buildNode(vm, node));
        }
      );
      break;
    } default: assert(false);
  }

  return builder.get(vm);
}

}

}