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

#ifndef MOZART_VMDEBUGINFOMANAGER_H
#define MOZART_VMDEBUGINFOMANAGER_H

#include "mozartcore.hh"
#include "buildsupport.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

UnstableNode DebugInfoManager::buildSourceName(VM vm, const SourceName& name) {
  return buildRecord(vm,
    buildArity(vm,
      "sourceName",
      "column",
      "filename",
      "line",
      "name",
      "type"
    ),
    mozart::build(vm, name.column),
    mozart::build(vm, name.filename.c_str()),
    mozart::build(vm, name.line),
    mozart::build(vm, name.name.c_str()),
    name.type == Proc ? mozart::build(vm, "proc") :
      name.type == Fun ? mozart::build(vm, "fun") : mozart::build(vm, "lazyFun")
  );
}

UnstableNode DebugInfoManager::buildIdentifiedSourceName(VM vm, const IdentifiedSourceName& name) {
  return buildRecord(vm,
    buildArity(vm,
      "sourceName",
      "column",
      "filename",
      "generationId",
      "kindId",
      "line",
      "name",
      "type"
    ),
    mozart::build(vm, name.column),
    mozart::build(vm, name.filename.c_str()),
    mozart::build(vm, name.group.generationId),
    mozart::build(vm, name.group.kindId),
    mozart::build(vm, name.line),
    mozart::build(vm, name.name.c_str()),
    name.type == Proc ? mozart::build(vm, "proc") :
      name.type == Fun ? mozart::build(vm, "fun") :
      name.type == LazyFun ? mozart::build(vm, "lazyFun") :
      mozart::build(vm, "arg")
  );
}

UnstableNode DebugInfoManager::buildSourceNamesMaps(VM vm, const SourceNamesMaps& snms) {
  return buildRecord(vm,
    buildArity(vm,
      "sourceNamesMaps",
      "abstractionsMap",
      "variablesMap"
    ),
    buildMap(vm, snms.abstractionsMap),
    buildMap(vm, snms.variablesMap)
  );
}

UnstableNode DebugInfoManager::buildThreadSourceNames(VM vm, const ThreadSourceNames& tsns) {
  return buildVector(vm, tsns.stack);
}

template<class Value>
UnstableNode DebugInfoManager::build(VM vm, Value value) {
  if constexpr (std::is_same_v<Value, SourceName>)
    return buildSourceName(vm, value);
  else if constexpr (std::is_same_v<Value, IdentifiedSourceName>)
    return buildIdentifiedSourceName(vm, value);
  else if constexpr (std::is_same_v<Value, SourceNamesMaps>)
    return buildSourceNamesMaps(vm, value);
  else if constexpr (std::is_same_v<Value, ThreadSourceNames>)
    return buildThreadSourceNames(vm, value);
  else
    return mozart::build(vm, value);
}

void DebugInfoManager::insertStack(Id threadId) {
  tsnm.insert({threadId, ThreadSourceNames()});
  pushMap(threadId);
}

void DebugInfoManager::removeStack(Id threadId) {
  if (tsnm.contains(threadId))
    tsnm.erase(threadId);
}

size_t DebugInfoManager::getStackSize(Id threadId) {
  if (tsnm.contains(threadId))
    return tsnm.at(threadId).stack.size();
  else return 0;
}

void DebugInfoManager::pushMap(Id threadId) {
  assert(tsnm.contains(threadId));
  tsnm.at(threadId).stack.push_back(SourceNamesMaps());
}

void DebugInfoManager::popMap(Id threadId) {
  assert(tsnm.contains(threadId));
  tsnm.at(threadId).stack.pop_back();
}

void DebugInfoManager::attachSourceName(Id threadId, Id objectId, Id objectKindId, Id objectGenerationId, SourceName name) {
  if (!enabled) return;

  assert(tsnm.contains(threadId));
  
  SourceNamesMapsStack& stack = tsnm.at(threadId).stack;
  assert(!stack.empty());
  
  IdentifiedSourceNamesMap& map = stack.back().variablesMap;
  if (!map.contains(objectId))
    map.insert({objectId, IdentifiedSourceNames()});
  map.at(objectId).push_back(IdentifiedSourceName(name,
    GroupIdentity(objectKindId, objectGenerationId)));

  std::printf("Link %ld to %s\n", objectId, name.name.c_str());
}

const DebugInfoManager::IdentifiedSourceNames DebugInfoManager::getSourceNames(Id threadId, Id objectId) {
  assert(tsnm.contains(threadId));
  IdentifiedSourceNames names;
  SourceNamesMapsStack& stack = tsnm.at(threadId).stack;

  for (SourceNamesMaps& maps : stack) {
    IdentifiedSourceNamesMap& map = maps.variablesMap;
    if (map.contains(objectId)) {
      IdentifiedSourceNames& otherNames = map.at(objectId);
      names.insert(names.begin(), otherNames.begin(), otherNames.end());
    }
  }

  return names;
}

const DebugInfoManager::IdentifiedSourceNamesMap DebugInfoManager::getVariables() {
  IdentifiedSourceNamesMap map;

  for (auto iter = tsnm.begin(); iter != tsnm.end(); ++iter) {
    Id threadId = iter->first;
    const SourceNamesMapsStack& stack = iter->second.stack;
    
    for (const SourceNamesMaps& maps : stack) {
      const IdentifiedSourceNamesMap& variablesMap = maps.variablesMap;

      for (auto subiter = variablesMap.begin(); subiter != variablesMap.end(); ++subiter) {
        Id variableId = subiter->first;

        if (!map.contains(variableId)) {
          map.insert({variableId, subiter->second});
        }
      }
    }
  }

  return map;
}

bool DebugInfoManager::containsAbstractionSourceNames(Id threadId, Id objectId) {
  if (!enabled) return false;

  SourceNamesMapsStack& stack = tsnm.at(threadId).stack;

  for (SourceNamesMaps& maps : stack) {
    SourceNamesMap& map = maps.abstractionsMap;
    if (map.contains(objectId)) return true;
  }

  return false;
}

const DebugInfoManager::SourceNames& DebugInfoManager::getAbstractionSourceNames(Id threadId, Id objectId) {
  SourceNamesMapsStack& stack = tsnm.at(threadId).stack;

  for (SourceNamesMaps& maps : stack) {
    SourceNamesMap& map = maps.abstractionsMap;
    if (map.contains(objectId))
      return map.at(objectId);
  }

  std::printf("ALERT\n");
  assert(false);
  return tsnm.at(threadId).currentNames;
}

const DebugInfoManager::SourceNamesMap DebugInfoManager::getAbstractions() {
  SourceNamesMap map;

  for (auto iter = tsnm.begin(); iter != tsnm.end(); ++iter) {
    Id threadId = iter->first;
    const SourceNamesMapsStack& stack = iter->second.stack;
    
    for (const SourceNamesMaps& maps : stack) {
      const SourceNamesMap& abstractionsMap = maps.abstractionsMap;

      for (auto subiter = abstractionsMap.begin(); subiter != abstractionsMap.end(); ++subiter) {
        Id abstractionId = subiter->first;

        if (!map.contains(abstractionId)) {
          map.insert({abstractionId, subiter->second});
        }
      }
    }
  }

  return map;
}

void DebugInfoManager::signalSourceName(VM vm, Id threadId, Type type,
  StableNode& name, StableNode& filename, size_t line, size_t column) {
  if (!enabled) return;

  assert(tsnm.contains(threadId));

  atom_t atomName = AtomLike(name).getValue(vm);
  atom_t atomFilename = AtomLike(filename).getValue(vm);

  SourceNames& currentNames = tsnm.at(threadId).currentNames;

  // Sometimes Mozart procedures can be inlined but the debug info is still present
  // and so must be ignored to not interfere with the next definition
  if (type != Arg && !currentNames.empty()) currentNames.clear();

  currentNames.push_back(
    SourceName(type, atomName, atomFilename, line, column));
}

void DebugInfoManager::signalAbstraction(VM vm, Id threadId, UnstableNode& abstraction) {
  if (!enabled) return;

  assert(tsnm.contains(threadId));

  ThreadSourceNames& tsn = tsnm.at(threadId);
  SourceNames& names = tsn.currentNames;

  if (!names.empty() && !tsn.stack.empty()) {
    SourceNamesMap& abstractionsMap = tsn.stack.back().abstractionsMap;

    Id abstractionId = Identifiable(abstraction).getId(vm);

    if (abstractionsMap.contains(abstractionId))
      abstractionsMap.at(abstractionId) = names;
    else
      abstractionsMap.insert({abstractionId, SourceNames(names)});

    // std::printf("New abstraction %ld: ", abstractionId);
    // for (SourceName& name : names) {
    //   std::printf("%s ", name.name.c_str());
    // }
    // std::printf("\n");
    
    names.clear();
  } // else std::printf("ALERT EMPTY\n");
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_VMDEBUGINFOMANAGER_H
