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

UnstableNode ModIntrospection::buildNodesCountsRecord(VM vm, Introspection::NodesCounts& properties) {
  return buildRecord(vm,
    buildArity(vm,
      "nodes",
      "gNodesCount",
      "kNodesCount",
      "nodesCount",
      "stableNodesCount",
      "stackDepth",
      "structuralNodesCount",
      "tokenNodesCount",
      "unstableNodesCount",
      "valueNodesCount",
      "variableNodesCount",
      "xNodesCount",
      "yNodesCount"
    ),
    build(vm, properties.gNodesCount),
    build(vm, properties.kNodesCount),
    build(vm, properties.nodesCount),
    build(vm, properties.stableNodesCount),
    build(vm, properties.stackDepth),
    build(vm, properties.structuralNodesCount),
    build(vm, properties.tokenNodesCount),
    build(vm, properties.unstableNodesCount),
    build(vm, properties.valueNodesCount),
    build(vm, properties.variableNodesCount),
    build(vm, properties.xNodesCount),
    build(vm, properties.yNodesCount)
  );
}

void ModIntrospection::GetNodesCounts::call(VM vm, Out result) {
  Introspection::NodesCounts properties =
    vm->getIntrospection().getNodesCounts(vm);
  result = buildNodesCountsRecord(vm, properties);
}

// Register types sizes

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

UnstableNode ModIntrospection::buildNodeRecord(VM vm, RichNode node) {
  Type type = node.type();

  return buildRecord(vm,
    buildArity(vm,
      "node",
      "bindingPriority",
      "copyable",
      "feature",
      "id",
      "name",
      "structuralBehavior",
      "transient",
      "uuid",
      "value"
    ),
    build(vm, type->getBindingPriority()),
    build(vm, type->isCopyable()),
    build(vm, type->isFeature()),
    build(vm, node.getId()),
    build(vm, type->getName().c_str()),
    build(vm,
      nodeStructuralBehaviorToString(
        type->getStructuralBehavior()
      ).c_str()
    ),
    build(vm, type->isTransient()),
    build(vm, type->getUUID()),
    build(vm, nodeToString(vm, node).c_str())
  );
}

UnstableNode ModIntrospection::getThreadXNode(VM vm, In threadNode, In indexNode) {
  Runnable* runnable = getArgument<Runnable*>(vm, threadNode);
  size_t index = getArgument<size_t>(vm, indexNode);
  Introspection& introspection = vm->getIntrospection();

  assert(index < introspection.getXNodesRegisterSize(vm, runnable));
  RichNode node = introspection.getXNode(vm, runnable, index);
  return buildNodeRecord(vm, node);
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

  return buildNodeRecord(vm, node);
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
      builder.push_back(vm, buildNodeRecord(vm, node));
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
          builder.push_back(vm, buildNodeRecord(vm, node));
        }
      );
      break;
    } case NodesRegister::gRegister: {
      // assert(to < introspection.getGNodesRegisterSize(vm, runnable, depth));
      introspection.doForEachGNode(vm, runnable, depth, from, to,
        Introspection::allNodes,
        [&builder](VM vm, Runnable* runnable, RichNode node) {
          builder.push_back(vm, buildNodeRecord(vm, node));
        }
      );
      break;
    } case NodesRegister::kRegister: {
      // assert(to < introspection.getKNodesRegisterSize(vm, runnable, depth));
      introspection.doForEachKNode(vm, runnable, depth, from, to,
        Introspection::allNodes,
        [&builder](VM vm, Runnable* runnable, RichNode node) {
          builder.push_back(vm, buildNodeRecord(vm, node));
        }
      );
      break;
    } default: assert(false);
  }

  return builder.get(vm);
}

void ModIntrospection::GetNodes::call(VM vm, In nodeFamily, In fromNode, In toNode, Out result) {
  using namespace patternmatching;

  Introspection& introspection = vm->getIntrospection();
  Introspection::NodeBoolLambda filter;
  if (matches(vm, nodeFamily, "variable")) {
    filter = [&introspection](VM vm, RichNode node) {
      return introspection.isVariableNode(vm, node);
    };
  } else if (matches(vm, nodeFamily, "token")) {
    filter = [&introspection](VM vm, RichNode node) {
      return introspection.isTokenNode(vm, node);
    };
  } else if (matches(vm, nodeFamily, "structural")) {
    filter = [&introspection](VM vm, RichNode node) {
      return introspection.isStructuralNode(vm, node);
    };
  } else if (matches(vm, nodeFamily, "value")) {
    filter = [&introspection](VM vm, RichNode node) {
      return introspection.isValueNode(vm, node);
    };
  } else {
    filter = Introspection::allNodes;
  }

  size_t from = getArgument<size_t>(vm, fromNode);
  size_t to = getArgument<size_t>(vm, toNode);
  size_t i = 0;

  OzListBuilder builder(vm);
  vm->getIntrospection().doForEachNode(vm,
    Introspection::allRunnables,
    filter,
    [&builder, from, to, &i](VM vm, Runnable* _, RichNode node) {
      if (i >= from && i < to) { // TODO Ugly make it better
        builder.push_back(vm, buildNodeRecord(vm, node));
      }
      i++;
    }
  );

  result = builder.get(vm);
}

}

}