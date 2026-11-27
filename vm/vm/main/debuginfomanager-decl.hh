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

class XRegArray;

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

    SourceName(const SourceName& other):
      name(other.name), filename(other.filename),
      type(other.type), line(other.line), column(other.column) {}

    Type type;
    std::string name, filename;
    size_t line, column;
  };

  static inline
  UnstableNode buildSourceName(VM vm, const SourceName& name);

  struct IdentifiedSourceName : public SourceName {
    IdentifiedSourceName(SourceName sourceName, GroupIdentity group) :
      SourceName(sourceName), group(group) {}

    GroupIdentity group;
  };

  static inline
  UnstableNode buildIdentifiedSourceName(VM vm, const IdentifiedSourceName& name);

  using SourceNames = Vector<SourceName>;
  using SourceNamesMap = Map<Id, SourceNames>;

  using IdentifiedSourceNames = Vector<IdentifiedSourceName>;
  using IdentifiedSourceNamesMap = Map<Id, IdentifiedSourceNames>;
  
  struct SourceNamesMaps {
    SourceNamesMap abstractionsMap;
    IdentifiedSourceNamesMap variablesMap;
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
  DebugInfoManager() : enabled(false), tsnm(ThreadSourceNamesMap()) {}

public:
  void enable() {
    enabled = true;
  }

  void disable() {
    enabled = false;
  }

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
  void signalSourceName(VM vm, Id threadId, Type type,
    StableNode& name, StableNode& filename, size_t line, size_t column);

  inline
  void signalAbstraction(VM vm, Id threadId, UnstableNode& abstraction);

public:
  inline
  void attachSourceName(Id threadId, Id objectId, Id objectKindId, Id objectGenerationId, SourceName name);

  inline
  void attachSourceName(Id threadId, Id objectId, SourceName name) {
    attachSourceName(threadId, objectId, SIZE_MAX, SIZE_MAX, name);
  }

  inline
  void attachSourceNames(VM vm, Id threadId, StableNode*& abstraction, ProgramCounter& PC, XRegArray*& xregs);

public:
  inline
  const IdentifiedSourceNames getSourceNames(Id threadId, Id objectId);

  inline
  const IdentifiedSourceNamesMap getVariables();

public:
  inline
  bool containsAbstractionSourceNames(Id threadId, Id objectId);

  inline
  const SourceNames& getAbstractionSourceNames(Id threadId, Id objectId);

  inline
  const SourceNamesMap getAbstractions();

public:
  inline
  Vector<Id> resoleName(const atom_t& name);

  inline
  Vector<Id> resoleName(const atom_t& name, const atom_t& filename,
    const size_t line, const size_t column);

private:
  bool enabled;
  ThreadSourceNamesMap tsnm;
};

using SourceName = DebugInfoManager::SourceName;
using SourceNames = DebugInfoManager::SourceNames;

}

#endif // MOZART_DEBUGINFOMANAGER_DECL_H
