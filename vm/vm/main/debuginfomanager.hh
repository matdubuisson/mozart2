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

template<class Value>
UnstableNode DebugInfoManager::build(VM vm, Value value) {
  return mozart::build(vm, value);
}

void DebugInfoManager::insertStack(Id threadId) {
  msm.insert({threadId, SourceNamesMapStack()});
}

void DebugInfoManager::removeStack(Id threadId) {
  if (msm.contains(threadId))
    msm.erase(threadId);
}

void DebugInfoManager::pushMap(Id threadId) {
  assert(msm.contains(threadId));
  msm.at(threadId).push_back(SourceNamesMap());
}

void DebugInfoManager::popMap(Id threadId) {
  assert(msm.contains(threadId));
  msm.at(threadId).pop_back();
}

void DebugInfoManager::insertIdentity(Id threadId, Id objectId, SourceName name) {
  assert(msm.contains(threadId));
  
  SourceNamesMapStack& stack = msm.at(threadId);
  assert(!stack.empty());
  
  SourceNamesMap& map = stack.back();
  if (!map.contains(objectId))
    map.insert({objectId, SourceNames()});
  
  map.at(objectId).push_back(name);
}

const SourceNames DebugInfoManager::identify(Id threadId, Id objectId) {
  assert(msm.contains(threadId));
  SourceNames names;
  SourceNamesMapStack& stack = msm.at(threadId);

  for (SourceNamesMap& map : stack) {
    if (map.contains(objectId)) {
      SourceNames& otherNames = map.at(objectId);
      names.insert(names.begin(), otherNames.begin(), otherNames.end());
    }
  }

  return names;
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_VMDEBUGINFOMANAGER_H
