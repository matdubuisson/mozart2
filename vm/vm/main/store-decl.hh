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

#ifndef MOZART_STORE_DECL_H
#define MOZART_STORE_DECL_H

#include "core-forward-decl.hh"
#include "type-decl.hh"
#include "memword.hh"
#include "storage-decl.hh"
#include "uuid-decl.hh"

#include <string>

namespace mozart {

/**
 * A value node in the store.
 * The store is entirely made of nodes. A node is basically a typed value.
 * Non-atomic values, such as records, contain references to other nodes in the
 * store, hence forming a graph, and the name "node".
 * There are two kinds of node: stable and unstable node. A stable node is
 * guaranteed never to change, whereas unstable node can change. In order to
 * maintain consistency in the store, non-atomic values are only allowed to
 * reference stable nodes. Unstable nodes are used for working data, and
 * inherently mutable data (such as the contents of a cell).
 */
class Node { // TO-COMPLETE : checks comments if they are 100% accurate
private:
  friend struct NodeBackup;
  friend struct NodeHole;
  friend class StableNode;
  friend class UnstableNode;
  friend class RichNode;
  friend class GraphReplicator;
  friend class Space;

  template <class T>
  friend class TypeInfoOf;

  /** Declares a new node */
  Node() {}

  /**
   * Initiates the node based on some referenced arguments
   * @param vm The virtual machine
   * @param args The references on arguments
   * @note Attributes a type and a value to the node based on the arguments
   */
  template<class T, class... Args>
  void make(VM vm, Args&&... args) {
    Accessor<T>::init(data.type, data.value, vm, std::forward<Args>(args)...);
  }

  /** Gets the type of the node */
  Type type() {
    return data.type;
  }

  /**
   * Gets access to the value of the node
   * @returns The value of the node TO-COMPLETE : what is the type ?
   */
  template <typename T>
  __attribute__((always_inline))
  auto access() -> decltype(Accessor<T>::get(std::declval<MemWord>())) { // Trailing return type
    return Accessor<T>::get(data.value);
  }

  /** @returns if the node is copyable */
  bool isCopyable() {
    return type().isCopyable();
  }

  /** @returns A stable node pointer of the node */
  inline
  StableNode* asStable();

  /** @returns An unstable node pointer of the node */
  inline
  UnstableNode* asUnstable();

  /**
   * From another node, copies its type and value into the current node
   * @param from A reference on the other node
   */
  void set(const Node& from) {
    data.type = from.data.type;
    data.value = from.data.value;
  }

  /**
   * Most of the time a node is a typed value but the graph replicator
   * can have other uses for it
   */
  union {
    // Regular structure
    struct {
      Type type;
      MemWord value;
    } data;

    // Graph replicator hack
    struct {
      Node* grNext;
      Node* grFrom;
    };
  };
};

struct NodeBackup {
public:
  /**
   * Restores the state of the referenced node to the state it had when the backup
   */
  void restore() {
    *node = saved;
  }
private:
  friend class StableNode;
  friend class UnstableNode;
  friend class RichNode;

  /**
   * Instantiates a backup node to be able to restore the state of the referenced
   * node later
   * @param node The node to backup
   */
  NodeBackup(Node* node) : node(node), saved(*node) {}
private:
  Node* node;
  Node saved;
};

struct NodeHole {
public:
  /**
   * Instantiates a new node hole
   */
  NodeHole() {}

  /**
   * Instantiates a new node hole from a stable node pointer
   * @param node The stable node pointer to reference
   */
  inline
  NodeHole(StableNode* node);

  /**
   * Instantiates a new node hole from a unstable node pointer
   * @param node The unstable node pointer to reference
   */
  inline
  NodeHole(UnstableNode* node);
public:
  /**
   * Copies the state of the referenced unstable node (rvalue) into the pointed node 
   * @param vm The virtual machine
   * @param value The unstable node (rvalue) to copy from
   */
  inline
  void fill(VM vm, UnstableNode&& value);
public:
  /**
   * Tells if the stable node pointer points to the same node as the stored node pointer
   * @param rhs The stable node pointer to compare with
   * @returns true if the addresses match, false otherwise
   */
  inline
  bool operator==(StableNode* rhs);

  /**
   * Tells if the unstable node pointer points to the same node as the stored node pointer
   * @param rhs The unstable node pointer to compare with
   * @returns true if the addresses match, false otherwise
   */
  inline
  bool operator==(UnstableNode* rhs);
private:
  friend class RichNode;

  /** @returns A pointer to the node */
  Node* node() {
    return _node;
  }

  /** @returns True if the node is stable */
  bool isStable() {
    return _isStable;
  }
private:
  Node* _node;
  bool _isStable;
};

template <class T>
class TypedRichNode {
};

/**
 * A rich node provides read access to nodes, with transparent dereferencing.
 */
class RichNode {
public:
  /** Instantiates a new rich node */
  RichNode() {}

  /**
   * Instantiates a new rich node with a reference of a stable node
   * @param origin The stable node to reference
   * @note If the referenced node is itself a reference, it is dereferenced to get
   * a direct reference on the actual node
   */
  __attribute__((always_inline))
  inline
  RichNode(StableNode& origin);

  /**
   * Instantiates a new rich node with a reference of a unstable node
   * @param origin The unstable node to reference
   * @note If the referenced node is itself a reference, it is dereferenced to get
   * a direct reference on the actual node and it is stable because reference nodes
   * are stable
   */
  __attribute__((always_inline))
  inline
  RichNode(UnstableNode& origin);

  /**
   * Instantiates a new rich node with a reference of a node hole pointing on a stable or unstable node
   * @param origin The node hole containing the node to reference
   * @note If the referenced node is itself a reference, it is dereferenced to get
   * a direct reference on the actual node and it is stable because reference nodes
   * are stable otherwise it is stable if the referenced node is stable
   */
  __attribute__((always_inline))
  inline
  RichNode(NodeHole origin);

  /**
   * Instantiates a new rich node from a nullptr
   * @param nullptr The nullptr value
  * @note The rich node will reference a nullptr and will be unstable
   */
  __attribute__((always_inline))
  RichNode(std::nullptr_t): _node(nullptr), _isStable(false) {}

  /** @returns The type of the referenced node */
  __attribute__((always_inline))
  Type type() {
    return node()->type();
  }

  /** @returns If type of the referenced node is transient */
  __attribute__((always_inline))
  inline
  bool isTransient();

  /** @returns If type of the referenced node is a feature */
  __attribute__((always_inline))
  inline
  bool isFeature();

  /** @returns If the referenced node type is the provided type */
  template <class T>
  __attribute__((always_inline))
  bool is() {
    return type() == T::type();
  }

  /**
   * Converts the current rich node to a typed rich node of the provided type
   * @returns A typed rich node
   * @note The type of the rich node must be the provided type
   */
  template <class T>
  __attribute__((always_inline))
  TypedRichNode<T> as() {
    assert(is<T>());
    return TypedRichNode<T>(*this);
  }

  /**
   * Gets a stable node pointer of the referenced node
   * @param vm The virtual machine
   * @returns A stable node pointer
   * @note The pointer pointed node has to be stable
   */
  __attribute__((always_inline))
  inline
  StableNode* getStableRef(VM vm);

  /**
   * If the pointer node is a reference node then it is dereferenced and
   * the pointed node by the current rich node becomes stable
   */
  inline
  void update();

  /**
   * Ensures the pointed node is stable, if not it changes the pointed node
   * by a new stable node pointer referencing the previous pointed node as unstale
   * and this new pointed node is stable
   * @param vm The virtual machine
   */
  inline
  void ensureStable(VM vm);

  /**
   * @returns A new backup node on the pointed node
   */
  NodeBackup makeBackup() {
    return NodeBackup(node());
  }

  /**
   * @returns If the pointed node is the same as the pointed node of the provided rich node
   * @param right The rich node to compare with
   */
  bool isSameNode(RichNode right) {
    return node() == right.node();
  }

  /**
   * The rich node becomes as the provided node reference
   * @param vm The virtual machine
   * @param value A reference on the node value (stable, unstable, unstable (rvalue) or rich)
   * @note The structural behavior of the type has to be either sbTokenEq or sbVariable
   */
  template <typename T>
  void become(VM vm, T&& value) {
    assert((type().getStructuralBehavior() == sbTokenEq) ||
           (type().getStructuralBehavior() == sbVariable));
    reinit(vm, std::forward<T>(value));
  }

  /**
   * Gets a string representing the node
   * @returns A printable string
   */
  inline
  std::string toDebugString();
private:
  /**
   * Dereferences a node reference until to have a direct pointer on a stable node
   * @param node A pointer on a node
   * @returns A direct pointer on a stable node
   * @note The referenced node is of type reference
   * @remark Optimizations for references with a distance of 1 to the real node
   */
  __attribute__((always_inline))
  inline
  static StableNode* dereference(Node* node);

  /**
   * Dereferences a node reference until to have a direct pointer on a stable node
   * @param node A pointer on a node
   * @returns A direct pointer on a stable node
   * @note The referenced node is of type reference
   */
  __attribute__((noinline))
  inline
  static StableNode* dereferenceLoop(StableNode* node);

  /**
   * Gets a pointer on the stable node reference by the reference node provided
   * @param node The reference node pointing on a stable node
   * @returns A pointer on the stable node
   */
  __attribute__((always_inline))
  inline
  static StableNode* destOf(Node* node);
private:
  template <class T>
  friend class TypedRichNode;

  template <typename T>
  friend class TypeInfoOf;

  friend class GarbageCollector;
  friend class SpaceCloner;
  friend struct StructuralDualWalk;
  friend class Serializer;
  friend class Pickler;

  /**
   * Re-initiates the rich node from the referenced stable node
   * @param vm The virtual machine
   * @param from The stable node reference
   * @note There are three behavior, if current node is referenced node then it does nothing,
   * else if it is copyable it sets the value and type of the pointer node with the values of
   * the reference node else it makes the pointed node becoming a reference of the referenced
   * stable node
   */
  inline void reinit(VM vm, StableNode& from);

  /**
   * Re-initiates the rich node from the referenced unstable node
   * @param vm The virtual machine
   * @param from The stable node reference
   * @note There are three behavior, if current node is referenced node then it does nothing,
   * else it initiates the pointed node providing the referenced unstable node
   */
  inline void reinit(VM vm, UnstableNode& from);

  /**
   * Re-initiates the rich node from the referenced unstable node (rvalue)
   * @param vm The virtual machine
   * @param from The stable node reference
   * @note An unstable node rvalue reference is copyable
   */
  inline void reinit(VM vm, UnstableNode&& from);

  /**
   * Re-initiates the rich node from the referenced rich node
   * @param vm The virtual machine
   * @param from The stable node reference
   * @note Re-initiates according to the referenced node type thus stable or unstable
   */
  inline void reinit(VM vm, RichNode from);

  /**
   * Gets access to the value of the rich node
   * @returns The value of the node
   */
  template <typename T>
  __attribute__((always_inline))
  auto access() -> decltype(std::declval<Node>().access<T>()) { // Trailing return type
    return node()->access<T>();
  }
private:
  friend class StableNode;
  friend class UnstableNode;

  /** @returns The pointer of the pointed node */
  __attribute__((always_inline))
  Node* node() {
    return _node;
  }

  /** @returns If the pointed node is stable */
  __attribute__((always_inline))
  bool isStable() {
    return _isStable;
  }

  /** @returns A reference as stable node of the pointed node */
  __attribute__((always_inline))
  StableNode& asStable() {
    return *node()->asStable();
  }

  /** @returns A reference as unstable node of the pointed node */
  __attribute__((always_inline))
  UnstableNode& asUnstable() {
    return *node()->asUnstable();
  }
private:
  Node* _node;
  bool _isStable;
};

/**
 * Stable node, which is guaranteed never to change
 */
class StableNode: public Node {
public:
  /**
   * Declares a new stable node
   */
  StableNode() {}

  /**
   * Declares a new stable node from another reference node
   * (stable, unstable, unstable rvalue or rich) TO-COMPLETE : check if the comments are 100% accurate
   * @param vm The virtual machine
   * @param from The reference node to copy from
   * @note The new stable node will have the same type and value as from
   */
  template <typename T>
  StableNode(VM vm, T&& from) {
    init(vm, std::forward<T>(from));
  }

  /**
   * Initiates the stable node based on a referenced stable node
   * @param vm The virtual machine
   * @param from The stable node to copy from
   * @note If the referenced node is copyable then a copy is made otherwise
   * only a reference to it will be made by the new node
   */
  inline void init(VM vm, StableNode& from);

  /**
   * Initiates the stable node based on a referenced unstable node
   * @param vm The virtual machine
   * @param from The stable node to copy from
   * @note If the referenced node is copyable then a copy is made otherwise
   * only a reference to it will be made by the new node
   */
  inline void init(VM vm, UnstableNode& from);

  /**
   * Initiates the stable node based on a referenced unstable node (rvalue)
   * @param vm The virtual machine
   * @param from The unstable node to copy from
   * @note An unstable node (rvalue) is copyable
   */
  inline void init(VM vm, UnstableNode&& from);

  /**
   * Initiates the stable node based on a rich node
   * @param vm The virtual machine
   * @param from The rich node to copy from
   * @note If the rich node is stable then the new node will be initiated based on its
   * stable reference, otherwise it will be initiated based on its unstable reference
   */
  inline void init(VM vm, RichNode from);

  /**
   * Initiates the stable node as unit
   * @param vm The virtual machine
   */
  inline void init(VM vm);

  /**
   * Initiates the stable node based on a referenced node
   * @param vm The virtual machine
   * @param from The node to copy from
   */
  template <typename T>
  inline void init(VM vm, T&& from);
public:
  // Make this class non-copyable and non-movable
  StableNode(const StableNode& from) = delete;
  StableNode& operator=(const StableNode& from) = delete;

  StableNode(StableNode&& from) = delete;
  StableNode& operator=(StableNode&& from) = delete;
};

/**
 * Unstable node, which is allowed to change over time
 */
class UnstableNode: public Node {
public:
  /**
   * Declares a new unstable node
   */
  UnstableNode() {}

  /**
   * Initiates the unstable node based on a referenced node
   * (stable, unstable, unstable rvalue or rich) TO-COMPLETE : check
   * @param vm The virtual machine
   * @param from The node to copy from
   * @note The new unstable node will have the same type and value as from
   */
  template <typename T>
  UnstableNode(VM vm, T&& from) {
    init(vm, std::forward<T>(from));
  }

  /**
   * Initiates the unstable node as a unit
   * @param vm The virtual machine
   */
  inline void init(VM vm);

  /**
   * Initiates the unstable node based on a referenced node
   * (stable, unstable, unstable rvalue or rich) TO-COMPLETE : check
   * @param vm The virtual machine
   * @param from The node to copy from
   * @note The new unstable node will have the same type and value as from
   */
  template <typename T>
  void init(VM vm, T&& from) {
    copy(vm, std::forward<T>(from));
  }

  /**
   * Copies the content of the referenced stable node into the unstable node
   * @param vm The virtual machine
   * @param from The stable node to copy from
   * @note If the referenced node is copyable then a copy is made otherwise only
   * a reference to it is made by the new node
   */
  inline void copy(VM vm, StableNode& from);

  /**
   * TO-COMPLETE : check if the comments are 100% accurate
   * Copies the content of the referenced unstable node into the unstable node
   * @param vm The virtual machine
   * @param from The unstable node to copy from
   * @note If the referenced node is copyable then a copy is made otherwise both
   * the current node and the referenced node becomes references on a new stable
   * node carrying the value of the referenced node
   */
  inline void copy(VM vm, UnstableNode& from);

  /**
   * Copies the content of the referenced unstable (rvalue) node into the unstable node
   * @param vm The virtual machine
   * @param from The unstable node (rvalue) to copy from
   * @note The referenced node is copyable
   */
  inline void copy(VM vm, UnstableNode&& from);

  /**
   * Copies the content of the referenced rich node into the unstable node
   * @param vm The virtual machine
   * @param from The rich node to copy from
   * @note If the rich node is stable then it is processed as a stable reference,
   * otherwise it is processed as an unstable reference
   */
  inline void copy(VM vm, RichNode from);

  /**
   * Copies the content of the referenced node into the unstable node
   * @param vm The virtual machine
   * @param from The node to copy from
   */
  template <typename T>
  inline void copy(VM vm, T&& from);

  /**
   * Builds a new unstable node from provided referenced arguments
   * @param vm The virtual machine
   * @param args The references on arguments
   * @returns A new unstable node
   */
  template<class T, class... Args>
  static UnstableNode build(VM vm, Args&&... args) {
    UnstableNode result;
    result.make<T>(vm, std::forward<Args>(args)...);
    return result;
  }
public:
  // Make this class non-copyable
  UnstableNode(const UnstableNode& from) = delete;
  UnstableNode& operator=(const UnstableNode& from) = delete;

  // But make it movable
  UnstableNode(UnstableNode&& from) = default;
  UnstableNode& operator=(UnstableNode&& from) = default;
};

/**
 * Global node, which can be present in others VMs, pickles, etc.
 */
class GlobalNode {
private:
  /**
   * Instantiates a new global node with an uuid
   * @param uuid The uuid to identify the global node
   * @note Global nodes form a binary search tree sorted by uuids
   */
  inline
  GlobalNode(UUID uuid);

public:
  /**
   * Makes a new global node or re-use an existing one having the provided uuid
   * and initiates it with the provided referenced parameters
   * @param vm The virtual machine
   * @param uuid The uuid for the global node
   * @param self A referenced stable node to copy representing the content
   * @param proto A referenced stable node to copy representing the protocol
   * @returns A pointer on the (new) global node
   */
  template <typename Self, typename Proto>
  inline
  static GlobalNode* make(VM vm, const UUID& uuid, Self&& self, Proto&& proto);

  /**
   * Makes a new global node with a new generated uuid from the virtual machine
   * and initiates it with the provided referenced parameters
   * @param vm The virtual machine
   * @param self A referenced stable node to copy representing the content
   * @param proto A referenced stable node to copy representing the protocol
   * @returns A pointer on the (new) global node
   */
  template <typename Self, typename Proto>
  inline
  static GlobalNode* make(VM vm, Self&& self, Proto&& proto);

  /**
   * Finds or creates (if not present) a global node with the provided uuid
   * then returns it
   * @param vm The virtual machine
   * @param uuid The uuid to identify the global node
   * @param to A reference on a global node pointer to store the result
   * @returns true if the global node already exists, false otherwise
   * @note Global nodes form a binary search tree sorted by uuids
   */
  inline
  static bool get(VM vm, UUID uuid, GlobalNode*& to);

public:
  UUID uuid;
  StableNode self;
  StableNode protocol;

private:
  GlobalNode* left;
  GlobalNode* right;
};

/**
 * Base class for specializations of TypedRichNode<T>
 */
class BaseTypedRichNode {
public:
  /**
   * Instantiates a new base typed rich node from a rich node reference
   * @param self The rich node to copy
   */
  explicit BaseTypedRichNode(RichNode self) : _self(self) {}

  /** @returns The copied rich node */
  operator RichNode() {
    return _self;
  }
protected:
  RichNode _self;
};

/**
 * The returned value of 'vm->protect()', a node protected from GC.
 * This really is a std::shared_ptr<StableNode*>, but for convenience the
 * * and -> operators dereference twice to get at the StableNode&.
 * A ProtectedNode can be implicitly converted back and forth to a genuine
 * std::shared_ptr<StableNode*>.
 */
class ProtectedNode {
public:
  /** Instantiates an empty protected node */
  ProtectedNode() {}

  /**
   * Instantiates a new protected node and sets the protected referenced stable node
   * pointer to nullptr
   * @param nullptr The nullptr value
   */
  ProtectedNode(std::nullptr_t): _node(nullptr) {}

  /**
   * Instantiates a new protected node from a shared pointer on a stable node pointer
   * @param from The shared pointer reference to copy the referenced stable node pointer
   * from (might be an rvalue)
   */
  ProtectedNode(std::shared_ptr<StableNode*>&& from): _node(std::move(from)) {}

  /**
   * Instantiates a new protected node from a shared pointer on a stable node pointer
   * @param from The shared pointer reference to copy the referenced stable node pointer
   * from
   */
  ProtectedNode(const std::shared_ptr<StableNode*>& from): _node(from) {}

  /** @returns A new constant copy of the protected node */
  operator std::shared_ptr<StableNode*>() const {
    return { _node };
  }

  /**
   * @returns The address of the protected stable node
   * @note The referenced stable node pointer is dereferenced twice to get rid first of
   * the reference then of the pointer
   */
  StableNode& operator*() const {
    return **_node;
  }

  /**
   * @returns The pointer to the protected stable node
   * @note Single dereference to get only rid of the reference
   */
  StableNode* operator->() const {
    return *_node;
  }

  /** @returns If the referenced stable node pointer is not null */
  explicit operator bool() const {
    return (bool) _node;
  }

  /** Resets the protected node */
  void reset() {
    _node.reset();
  }

private:
  std::shared_ptr<StableNode*> _node;
};

}

#endif // MOZART_STORE_DECL_H
