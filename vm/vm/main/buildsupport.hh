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

#ifndef MOZART_BUILDSUPPORT_H
#define MOZART_BUILDSUPPORT_H

#include <cstdlib>
#include <algorithm>

#ifndef MOZART_GENERATOR

namespace mozart {

template<class Derived>
template<class Value>
UnstableNode BuildSupport<Derived>::build(VM vm, Value value) {
  // If there are not build() defined for Value inside Introspection
  // then go look outside if there exists one.
  return Derived::build(vm, value);
}

template<class Derived>
template<class Value>
UnstableNode BuildSupport<Derived>::buildVector(VM vm, const Vector<Value>& vector) {
  OzListBuilder builder(vm);
  for (Value value : vector) {
    builder.push_back(vm, build<Value>(vm, value));
  }
  return builder.get(vm);
}

template<class Derived>
template<class Key, class Value>
UnstableNode BuildSupport<Derived>::buildMap(VM vm, const Map<Key, Value>& map) {
  UnstableNode label = Atom::build(vm, "map");
  size_t i = 0, width = map.size();
  UnstableField fields[width];

  for (auto pair : map) {
    Key key = pair.first;

    if constexpr (std::is_same_v<Value, UnstableNode>)
      fields[i].value.copy(vm, pair.second);
    else {
      Value value = pair.second;
      fields[i].feature = build<Key>(vm, key);

      if constexpr (is_Vector_v<Value>)
        fields[i].value = buildVector(vm, value);
      else if constexpr (is_Map_v<Value>)
        fields[i].value = buildMap(vm, value);
      else
        fields[i].value = build<Value>(vm, value);
    }
    
    i++;
  }

  return buildRecordDynamic(vm, label, width, fields);
}

}

#endif // MOZART_GENERATOR

#endif // MOZART_BUILDSUPPORT_H
