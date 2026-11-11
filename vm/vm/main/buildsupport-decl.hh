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

#ifndef MOZART_BUILDSUPPORT_DECL_H
#define MOZART_BUILDSUPPORT_DECL_H

#include "core-forward-decl.hh"

namespace mozart {

template<class Derived>
class BuildSupport {
public:
  template<class Value>
  static inline
  UnstableNode build(VM vm, Value value);

public:
  template<class Value>
  using Vector = std::vector<Value>;

  template<class Type>
  struct IsVector : std::false_type {};

  template<class Value>
  struct IsVector<Vector<Value>> : std::true_type {};

  template<class Type>
  static inline constexpr bool is_Vector_v = IsVector<Type>::value;

  template<class Value>
  static inline
  UnstableNode buildVector(VM vm, const Vector<Value>& vector);

public:
  template<class Key, class Value>
  using Map = std::unordered_map<Key, Value>;

  template<class Type>
  struct IsMap : std::false_type {};

  template<class Key, class Value>
  struct IsMap<Map<Key, Value>> : std::true_type {};

  template<class Type>
  static inline constexpr bool is_Map_v = IsMap<Type>::value;

  template<class Key, class Value>
  static inline
  UnstableNode buildMap(VM vm, const Map<Key, Value>& map);
};

}

#endif // MOZART_BUILDSUPPORT_DECL_H
