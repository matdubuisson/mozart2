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

#ifndef MOZART_VMDEBUGINFOMANAGER_DECL_H
#define MOZART_VMDEBUGINFOMANAGER_DECL_H

#include "core-forward-decl.hh"
#include "identity-decl.hh"
#include "buildsupport-decl.hh"

namespace mozart {

class DebugInfoManager: public BuildSupport<DebugInfoManager> {
public:
  template<class Value>
  static inline
  UnstableNode build(VM vm, Value value);

public:
  enum Type {
    Proc,
    Fun,
    LazyFun,
    Arg
  };

  struct SourceName {
    SourceName(Type type, atom_t& name,
      atom_t& filename, size_t line, size_t column) :
      type(type), line(line), column(column) {
      this->name = std::string(name.contents());
      this->filename = std::string(filename.contents());
    }

    Type type;
    std::string name, filename;
    size_t line, column;
  };

  static inline
  UnstableNode buildSourceName(VM vm, const SourceName& name);

  using SourceNames = Vector<SourceName>;
  using SourceNamesMap = Map<Id, SourceNames>;
  
  struct SourceNamesMaps {
    SourceNamesMap abstractionsMap;
    SourceNamesMap variablesMap;
  };

  static inline
  UnstableNode buildSourceNamesMaps(VM vm, const SourceNamesMaps& snms);

  using SourceNamesMapsStack = Vector<SourceNamesMaps>;

  struct ThreadSourceNames {
    SourceNames currentNames;
    SourceNamesMapsStack stack;
  };

  static inline
  UnstableNode buildThreadSourceNames(VM vm, const ThreadSourceNames& tsns);

  using ThreadSourceNamesMap = Map<Id, ThreadSourceNames>;

public:
  DebugInfoManager() : tsnm(ThreadSourceNamesMap()) {}

public:
  const ThreadSourceNamesMap& getThreadSourceNamesMap() {
    return tsnm;
  }

public:
  inline
  void insertStack(Id threadId);

  inline
  void removeStack(Id threadId);

  inline
  size_t getStackSize(Id threadId);

public:
  inline
  void pushMap(Id threadId);

  inline
  void popMap(Id threadId);

public:
  inline
  void attachSourceName(Id threadId, Id objectId, SourceName name);

  inline
  const SourceNames getSourceNames(Id threadId, Id objectId);

public:
  inline
  bool containsAbstractionSourceNames(Id threadId, Id objectId);

  inline
  const SourceNames& getAbstractionSourceNames(Id threadId, Id objectId);

  inline
  const SourceNamesMap getAbstractions();

public:
  inline
  void signalSourceName(VM vm, Id threadId, Type type,
    StableNode& name, StableNode& filename, size_t line, size_t column);

  inline
  void signalAbstraction(VM vm, Id threadId, UnstableNode& abstraction);

private:
  ThreadSourceNamesMap tsnm;
};

using SourceName = DebugInfoManager::SourceName;
using SourceNames = DebugInfoManager::SourceNames;

}

#endif // MOZART_DEBUGINFOMANAGER_DECL_H
