# Virtual machine source code

### Purpose
### Key Classes
### Important Functions
### Dependencies
### Notes

<!-- ====================================================== -->
## array-decl.hh

<!-- ====================================================== -->
## array.hh

<!-- ====================================================== -->
## arrays.hh

### Purpose

Defines a static array potentially sized.

### Key Classes

- class StaticArray;

### Important Functions
### Dependencies

- core-forward-decl.hh : source code
- cassert : Runtime debugging checks
- cstdint : Fixed-size integer types

### Notes

<!-- ====================================================== -->
## atom-decl.hh

<!-- ====================================================== -->
## atom.hh

<!-- ====================================================== -->
## atomtable.hh

<!-- ====================================================== -->
## bigint-decl.hh

<!-- ====================================================== -->
## bigint.hh

<!-- ====================================================== -->
## bigintimplem-decl.hh

<!-- ====================================================== -->
## boolean-decl.hh

<!-- ====================================================== -->
## boolean.hh

<!-- ====================================================== -->
## builtins-decl.hh

<!-- ====================================================== -->
## builtins.hh

<!-- ====================================================== -->
## bytestring-decl.hh

<!-- ====================================================== -->
## bytestring.hh

<!-- ====================================================== -->
## cached

<!-- ====================================================== -->
## callables-decl.hh

<!-- ====================================================== -->
## callables.hh

<!-- ====================================================== -->
## cell-decl.hh

<!-- ====================================================== -->
## cell.hh

<!-- ====================================================== -->
## CMakeLists.txt

<!-- ====================================================== -->
## codearea-decl.hh

<!-- ====================================================== -->
## codearea.hh

<!-- ====================================================== -->
## coders.cc

<!-- ====================================================== -->
## coders-decl.hh

<!-- ====================================================== -->
## coreatoms-decl.hh

<!-- ====================================================== -->
## coreatoms.hh

<!-- ====================================================== -->
## corebuilders.hh

<!-- ====================================================== -->
## coredatatypes.cc

<!-- ====================================================== -->
## coredatatypes-decl.hh

<!-- ====================================================== -->
## coredatatypes.hh

<!-- ====================================================== -->
## core-forward-decl.hh

### Purpose

- Main objects definition.
- New operator redefinition to include a VM object with it.

### Key Classes

- struct LateStaticAssert;
- struct unit_t;
- struct basic_atom_t;
- struct BasicAtomStreamer;
- struct SpaceRef;
- struct VirtualMachineOptions;
- struct VMCleanupListNode;
- struct AlternativeToInt;
- struct AlternativeToInt64;

- class AtomImpl;
- class TypeInfo;
- class Node;
- class StableNode;
- class UnstableNode;
- class VirtualMachine;
- class GraphReplicator;
- class GarbageCollector;
- class SpaceCloner;
- class SerializationCallback;
- class TypeInfoOf;
- class NodeDictionary;
- class Space;

### Important Functions



### Dependencies

- cassert : Runtime debugging checks
- cstdint : Fixed-size integer types
- cstdlib : General utilities from the C standard library
- functional : Function wrappers and functional programming utilities
- limits : Numeric limits of data types
- memory : Memory management utilities
- ostream : Output stream functionality
- stdexcept : Standard exception classes
- utility : Small helper utilities

### Notes

<!-- ====================================================== -->
## coreinterfaces-decl.hh

<!-- ====================================================== -->
## coreinterfaces.hh

<!-- ====================================================== -->
## coremodules.cc

<!-- ====================================================== -->
## coremodules.hh

<!-- ====================================================== -->
## datatype-decl.hh

<!-- ====================================================== -->
## datatype.hh

<!-- ====================================================== -->
## datatypeshelpers-decl.hh

<!-- ====================================================== -->
## datatypeshelpers.hh

<!-- ====================================================== -->
## debugger.cc

<!-- ====================================================== -->
## debugger.hh

<!-- ====================================================== -->
## dictionary-decl.hh

<!-- ====================================================== -->
## dictionary.hh

<!-- ====================================================== -->
## dynbuilders-decl.hh

<!-- ====================================================== -->
## dynbuilders.hh

<!-- ====================================================== -->
## emulate.cc

<!-- ====================================================== -->
## emulate.hh

<!-- ====================================================== -->
## exceptions-decl.hh

### Purpose

There are four kinds of exception : ekFail, ekWaitBefore, ekWaitQuietBefore and ekRaise.
It defines the exception handler as a pair of a jump to the handler and a linked list pointing on the next exception handler.
The global exception mechanism is defined like a stack where handlers are pushed and popped from the top of the stack.

### Key Classes

- struct ExceptionHandler;
- enum class ExceptionKind;
- class GlobalExceptionMechanism;

### Important Functions
### Dependencies

- setjmp : non-local jumps, control flow different from usual one
- "core-forward-decl.hh"
- "store-decl.hh"

### Notes

<!-- ====================================================== -->
## exceptions.hh

<!-- ====================================================== -->
## exchelpers-decl.hh

<!-- ====================================================== -->
## exchelpers.hh

<!-- ====================================================== -->
## float-decl.hh

<!-- ====================================================== -->
## float.hh

<!-- ====================================================== -->
## foreignpointer-decl.hh

<!-- ====================================================== -->
## foreignpointer.hh

<!-- ====================================================== -->
## functiontraits-decl.hh

<!-- ====================================================== -->
## gcollect.cc

<!-- ====================================================== -->
## gcollect-decl.hh

### Purpose

Defines the garbage collector as a graph replicator. The garbage collector travels the data graph, replicates only nodes that are still reachable so in use and let the others non-replicated in the old space. Once the new space is fulfilled, the old one is freed. A garbage collector process three kind of things : a space, a thread or a node (data).

### Key Classes

- class GarbageCollector: public GraphReplicator;

### Important Functions

- GarbageCollector::isGCRequired();
- GarbageCollector::doGC(....);

### Dependencies

- "core-forward-decl.hh";
- "graphreplicator-decl.hh";

### Notes

<!-- ====================================================== -->
## gcollect.hh

<!-- ====================================================== -->
## graphreplicator-decl.hh

### Purpose

All data interconnections are modeled by a graph where data are nodes and references are links. The graph replicator is here to copy all the elements as stable/unstable/global nodes, spaces, threads, references and atoms.

### Key Classes

- class GraphReplicator;

### Important Functions

- GraphReplicator::copySpace;
- GraphReplicator::copyThread;
- GraphReplicator::copyStableNode;
- GraphReplicator::copyUnstableNode;
- GraphReplicator::copyStableRef;
- GraphReplicator::copyWeakStableRef;
- GraphReplicator::copyStableNodes;
- GraphReplicator::copyUnstableNodes;
- GraphReplicator::copyGNode;
- GraphReplicator::copyAtom;

### Dependencies

- "core-forward-decl.hh" : source code
- "memmanager.hh" : source code
- "memmanlist.hh" : source code
- "store-decl.hh" : source code
- "runnable-decl.hh" : source code

### Notes

<!-- ====================================================== -->
## graphreplicator.hh

<!-- ====================================================== -->
## grtypes-decl.hh

<!-- ====================================================== -->
## grtypes.hh

<!-- ====================================================== -->
## lstring-decl.hh

<!-- ====================================================== -->
## lstring.hh

<!-- ====================================================== -->
## matchdsl.hh

<!-- ====================================================== -->
## memmanager.hh / memmanager.cc

### Purpose

Memory allocation and deallocation : 
- Small blocks comes from bucket lists that are pre-allocated memory.
- Big blocks are classic heap allocated blocks (extra memory).

### Key Classes

- class MemoryManager;

### Important Functions

- void MemoryManager::init(VM vm) : allocation of the base block and set all buckets to null
- void* MemoryManager::getMemory(size_t size) : if enough gives memory from pre-allocated block else call getMoreMemory to have more
- void* MemoryManager::getMoreMemory(size_t size) : allocates more memory and recomputes thresholds
- void* MemoryManager::malloc(size_t size) : gets a memory block from one of the bucket lists
- void MemoryManager::free(void *ptr, size_t size) : frees a memory block and puts it on top of its bucket list
- void MemoryManager::releaseExtraAllocs() : releases extra allocation done by getMoreMemory

### Dependencies

- core-forward-decl.hh : source file
- cstdlib : General utilities from the C standard library
- algorithm : Collection of general algorithms
- forward_list : Singly linked list container

### Notes

<!-- ====================================================== -->
## memmanlist.hh

### Purpose

The memory management list is defined as a linked list with a complete set of operations for insertions, deletions, aggregations and getters.

### Key Classes

- struct ListNode;
- struct iterator;
- struct removable_iterator;
- class MemManagedList;

### Important Functions
### Dependencies

- "memmanager.hh" : source code
- utility : move/forward/helpers functions
- cassert : runtime assertions

### Notes

<!-- ====================================================== -->
## memword.hh

### Purpose

Defines the structure of a memory word. A memory word is a type which is defined as having the size of a void* and behaving as the union of a number of other types. If they are small enough, they are actually stored in the MemWord. If not, they are stored as a pointer to external memory.

### Key Classes

- struct MWUAccessor;
- struct MWUAccessor<U, T, T>;
- struct MWUAccessor<MWUnion<>, T*, T*>;
- struct MWUAccessor<MWUnion<>, T, T>;
- union MWUnion;
- union MWUnion<>;
- union MWUnion<T, Args...>;

### Important Functions

- & MWU*::get(....)
- void MWU*::alloc(....)
- constexpr bool MWU*::requiresExternalMemory(....)

### Dependencies

- core-forward-decl.hh : source file

### Notes

<!-- ====================================================== -->
## mozartcore-decl.hh

<!-- ====================================================== -->
## mozartcore.hh

<!-- ====================================================== -->
## mozart.hh

<!-- ====================================================== -->
## names-decl.hh

<!-- ====================================================== -->
## names.hh

<!-- ====================================================== -->
## nullable-decl.hh

<!-- ====================================================== -->
## objects-decl.hh

<!-- ====================================================== -->
## objects.hh

<!-- ====================================================== -->
## opcodes.hh

<!-- ====================================================== -->
## ozcalls-decl.hh

<!-- ====================================================== -->
## ozcalls.hh

<!-- ====================================================== -->
## patmattypes-decl.hh

<!-- ====================================================== -->
## patmattypes.hh

<!-- ====================================================== -->
## pickler.cc

<!-- ====================================================== -->
## pickler.hh

<!-- ====================================================== -->
## port-decl.hh

<!-- ====================================================== -->
## port.hh

<!-- ====================================================== -->
## properties.cc

<!-- ====================================================== -->
## properties-config.cc.in

<!-- ====================================================== -->
## properties-decl.hh

<!-- ====================================================== -->
## properties.hh

<!-- ====================================================== -->
## records-decl.hh

<!-- ====================================================== -->
## records.hh

<!-- ====================================================== -->
## reference-decl.hh

<!-- ====================================================== -->
## reference.hh

<!-- ====================================================== -->
## reflectivetypes-decl.hh

<!-- ====================================================== -->
## reflectivetypes.hh

<!-- ====================================================== -->
## reifiedgnode-decl.hh

<!-- ====================================================== -->
## reifiedgnode.hh

<!-- ====================================================== -->
## reifiedspace-decl.hh

<!-- ====================================================== -->
## reifiedspace.hh

<!-- ====================================================== -->
## reifiedthread-decl.hh

<!-- ====================================================== -->
## reifiedthread.hh

<!-- ====================================================== -->
## runnable-decl.hh

### Purpose

Three levels of thread priority are defined : low, middle and high.

Defines the intermediate state of a runnable such that it can be reset, fetched or stored.
This intermediate state maintains a VMAllocatedList (array list) of unstable nodes.

An class interface is defined for runnable entities (threads). Each runnable has its own space and has a priority of execution and contains an intermediate state. A thread can be suspended, resumed, terminated or killed. 

Defines also a list of runnables that typically will be used by the scheduler of the virtual machine to pick up new threads to execute.

### Key Classes

- struct iterator;
- class IntermediateState;
- class Runnable;
- class RunnableList;

### Important Functions
### Dependencies

- "core-forward-decl.hh" : source file
- "store-decl.hh" : source file
- "vmallocatedlist-decl.hh": source file
- cassert : runtime assertions

### Notes

<!-- ====================================================== -->
## runnable.hh

<!-- ====================================================== -->
## sclone.cc

<!-- ====================================================== -->
## sclone-decl.hh

### Purpose

Defines a space cloner as a graph replicator.

### Key Classes

- class SpaceCloner: public GraphReplicator;

### Important Functions

- SpaceCloner::doCloneSpace(....);

### Dependencies

- "core-forward-decl.hh";
- "graphreplicator-decl.hh";

### Notes

<!-- ====================================================== -->
## sclone.hh

<!-- ====================================================== -->
## serializer.cc

<!-- ====================================================== -->
## serializer-decl.hh

<!-- ====================================================== -->
## serializer.hh

<!-- ====================================================== -->
## smallint-decl.hh

<!-- ====================================================== -->
## smallint.hh

<!-- ====================================================== -->
## space-decl.hh

### Purpose



### Key Classes

- struct ScriptEntry;
- struct TrailEntry;
- class SpaceScript : public VMAllocatedList<ScriptEntry>;
- class SpaceTrail : public VMAllocatedList<TrailEntry>;
- class Distributor;
- class Space;

### Important Functions
### Dependencies
### Notes

<!-- ====================================================== -->
## space.hh

<!-- ====================================================== -->
## storage-decl.hh

### Purpose

There are two types of storage : the default storage (TODO complete more) and the implementation with arrays. Accessors are provided also allowing to initiate a storage and to get memory words from it.

### Key Classes

- struct have_cxx11_trait_helper;
- struct have_cxx11_trait;
- struct ImplAndCleanupListNode;
- struct DerefPotentialImplAndCleanup;
- struct DerefPotentialImplAndCleanup<ImplAndCleanupListNode<T>>;
- class ImplWithArray;
- class DefaultStorage;
- class Storage;
- class AccessorHelper;
- class AccessorHelper<T, DefaultStorage<T>>;
- class AccessorHelper<T, ImplWithArray<T, E>>;
- class DataTypeStorageHelper;
- class DataTypeStorageHelper<T, DefaultStorage<T>>;
- class DataTypeStorageHelper<T, ImplWithArray<T, E>>;

### Important Functions
### Dependencies

- core-forward-decl.hh : source file
- type-decl.hh : source file
- memword.hh : source file
- arrays.hh : source file
- type_traits : template metaprogramming

### Notes

<!-- ====================================================== -->
## storage.hh

<!-- ====================================================== -->
## store-decl.hh

### Purpose

A value node in the store.
The store is entirely made of nodes. A node is basically a typed value.
Non-atomic values, such as records, contain references to other nodes in the
store, hence forming a graph, and the name "node".
There are two kinds of node: stable and unstable node. A stable node is
guaranteed never to change, whereas unstable node can change. In order to
maintain consistency in the store, non-atomic values are only allowed to
reference stable nodes. Unstable nodes are used for working data, and
inherently mutable data (such as the contents of a cell).

#### Nodes

A node is essentially used for regular data structure consisting of one describing type and one memory word as value or pointer, however it can be used for graph replicators (TODO complete this). A node is copyable and can be seen as stable (immutable) or unstable (mutable).

A backup node is a node with only purpose to keep a pointer on an other one and copy its values (type and memory word) to the pointer node upon restore call.

A node hole is a meta node defining condition checks on it as for instance equality or is stable. It is not an instance of Node but it has a pointer on a node.

A typed rich node is a node without any behavior probably just for grouping.
A rich node contains a pointer on a node that is either a stable, either a unstable or an hole node. It offers some checks to know if the node is transient or is feature and other useful operations.

A stable node inherits from Node and can be only instantiated.

An unstable node inherits from Node and can be instantiated and copied.

A global node is like a container forming a binary tree where it has three value an uuid and two stable nodes (self and protocol) plus two pointers on other global nodes (left and right). If offers a get method to find an other global node according to its uuid.

A base typed rich node is a specialization of typed rich node.

A protected node is a type of node that is protected from the garbage collector.

### Key Classes

- struct NodeBackup;
- struct NodeHole;
- class Node;
- class TypedRichNode;
- class RichNode;
- class StableNode;
- class UnstableNode;
- class GlobalNode;
- class BaseTypedRichNode;
- class ProtectedNode;

### Important Functions
### Dependencies
### Notes


<!-- ====================================================== -->
## store.hh

<!-- ====================================================== -->
## string-decl.hh

<!-- ====================================================== -->
## string.hh

<!-- ====================================================== -->
## threadpool-decl.hh

### Purpose

Defines the thread queue (queue of pointers of runnables) used by the virtual machine to schedule threads.

Defines also the thread pool used by the virtual machine too to schedule threads and uses a thread queue to do that.

### Key Classes

- class ThreadQueue : public std::queue<Runnable*>;
- class ThreadPool;

### Important Functions
### Dependencies
### Notes

<!-- ====================================================== -->
## threadpool.hh

<!-- ====================================================== -->
## type-decl.hh

### Purpose

Main structure defining a general Type with different properties copyable, transient and feature. Equality conditions and pointing operation as '->' can be performed on such types defined with Type.

Types :
- Copyable : can be fully duplicated without modifying the original data.
- Transient : short living ephemeral data.
- Feature : describing meta-data of what is a certain data.

### Key Classes

- struct Type;

### Important Functions

- bool Type::isCopyable()
- bool Type::isTransient()
- bool Type::isFeature()

### Dependencies

- "core-forward-decl.hh" : source code

### Notes

<!-- ====================================================== -->
## type.hh

<!-- ====================================================== -->
## typeinfo-decl.hh

<!-- ====================================================== -->
## typeinfo.hh

<!-- ====================================================== -->
## unify.cc

<!-- ====================================================== -->
## unify-decl.hh

<!-- ====================================================== -->
## unify.hh

<!-- ====================================================== -->
## unit-decl.hh

<!-- ====================================================== -->
## unit.hh

<!-- ====================================================== -->
## unpickler.cc

<!-- ====================================================== -->
## unpickler.hh

<!-- ====================================================== -->
## utf-decl.hh

<!-- ====================================================== -->
## utf.hh

<!-- ====================================================== -->
## utils-decl.hh

<!-- ====================================================== -->
## utils.hh

<!-- ====================================================== -->
## uuid-decl.hh

### Purpose

UUID class definition with comparison operations.
Defines global operators to be executed on UUIDs.

### Key Classes

- struct UUID;
- class word_print;

### Important Functions
### Dependencies

- "core-forward-decl.hh" : source code
- cstdint : precise integers
- string : text handling
- sstream : string streams
- iomanip : formatting output

### Notes

<!-- ====================================================== -->
## variables-decl.hh

<!-- ====================================================== -->
## variables.hh

<!-- ====================================================== -->
## vmallocatedlist-decl.hh

### Purpose

Tiny file defining the virtual machine allocated list as a memory managed list.

### Key Classes

- class VMAllocatedList;

### Important Functions
### Dependencies

- "memmanager.hh";
- "memmanlist.hh";

### Notes


<!-- ====================================================== -->
## vmallocatedlist.hh

<!-- ====================================================== -->
## vm.cc

<!-- ====================================================== -->
## vm-decl.hh

<!-- ====================================================== -->
## vm.hh

<!-- ====================================================== -->
## weakrefs-decl.hh

<!-- ====================================================== -->
## weakrefs.hh

