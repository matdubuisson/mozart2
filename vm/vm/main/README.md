# Virtual machine source code documentation

## Authors

## Motivation

This document has for purpose to help future maintainer or simply peoples wanting to get inspiration from this repo to understand more easily the structure of the project, what parts are for and why it does that.

### Purpose
### Key Classes
### Important Functions
### Dependencies
### Notes

<!-- ====================================================== -->
## array-decl.hh / array.hh

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
## atom-decl.hh / atom.hh

<!-- ====================================================== -->
## atomtable.hh

### Purpose

Atoms definition and operation.
Atom names definition and checks (if not a keyword, format it to be printable).
Atom table definition used to retrieve atoms.

### Key Classes

- struct BasicAtomStreamer<char, 1>;
- struct BasicAtomStreamer<char, 2>;
- class Atom;
- class UniqueName;
- class AtomTable;
- class AtomImpl;
- class AtomTable;

### Important Functions

- size_t AtomImpl::length();
- const char* AtomImpl::contents();
- int AtomImpl::compare(const AtomImpl* rhs);

### Dependencies

- "core-forward-decl.hh" : source code
- cstring : legacy string utils (from string.h)
- string : string utils
- type_traits : template meta-programming
- sstream : strings manipulations as streams
- "utf-decl.hh" : source code

### Notes

<!-- ====================================================== -->
## bigint-decl.hh / bigint.hh

<!-- ====================================================== -->
## bigintimplem-decl.hh

<!-- ====================================================== -->
## boolean-decl.hh / boolean.hh

<!-- ====================================================== -->
## builtins-decl.hh / builtins.hh

<!-- ====================================================== -->
## bytestring-decl.hh / bytestring.hh

<!-- ====================================================== -->
## callables-decl.hh / callables.hh

<!-- ====================================================== -->
## cell-decl.hh / cell.hh

<!-- ====================================================== -->
## codearea-decl.hh / codearea.hh

<!-- ====================================================== -->
## coders-decl.cc / coders.hh

<!-- ====================================================== -->
## coreatoms-decl.hh / coreatoms.hh

<!-- ====================================================== -->
## corebuilders.hh

<!-- ====================================================== -->
## coredatatypes-decl.hh / coredatatypes.hh / coredatatypes.cc

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
## coreinterfaces-decl.hh / coreinterfaces.hh

<!-- ====================================================== -->
## coremodules.hh / coremodules.cc

<!-- ====================================================== -->
## datatype-decl.hh / datatype.hh

<!-- ====================================================== -->
## datatypeshelpers-decl.hh / datatypeshelpers.hh

<!-- ====================================================== -->
## debugger-decl.hh / debugger.hh / debugger.cc

TODO (currently on another branch)

<!-- ====================================================== -->
## dictionary-decl.hh / dictionary.hh

<!-- ====================================================== -->
## dynbuilders-decl.hh / dynbuilders.hh

<!-- ====================================================== -->
## emulate.hh / emulate.cc

<!-- ====================================================== -->
## exceptions-decl.hh / exceptions.hh

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

<!-- ====================================================== -->
## exchelpers-decl.hh / exchelpers.hh

<!-- ====================================================== -->
## float-decl.hh / float.hh

<!-- ====================================================== -->
## foreignpointer-decl.hh / foreignpointer.hh

<!-- ====================================================== -->
## functiontraits-decl.hh

### Purpose

Function traits definition.

### Key Classes
### Important Functions
### Dependencies

<!-- ====================================================== -->
## gcollect-decl.hh / gcollect.hh / gcollect.cc

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

<!-- ====================================================== -->
## graphreplicator-decl.hh / graphreplicator.hh

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

<!-- ====================================================== -->
## grtypes-decl.hh / grtypes.hh

<!-- ====================================================== -->
## lstring-decl.hh

### Purpose

Defines a lstring as a bounded mutable string of a certain generic type of character.

### Key Classes

- struct BaseLString;
- struct LString : BaseLString<C>;
- struct ContainedLString : mut::BaseLString<typename T::value_type>;
- enum UnicodeErrorReason;

### Important Functions
### Dependencies

<!-- ====================================================== -->
## lstring.hh

<!-- ====================================================== -->
## matchdsl.hh

<!-- ====================================================== -->
## memmanager.hh / memmanager.cc
- <cstdlib>
- <forward_list>
- <atomic>

- "core-forward-decl.hh"

- "memmanager.hh"

- "store-decl.hh"
- "threadpool-decl.hh"
- "gcollect-decl.hh"
- "sclone-decl.hh"
- "space-decl.hh"
- "uuid-decl.hh"
- "vmallocatedlist-decl.hh"

- "atomtable.hh"
- "bigintimplem-decl.hh"
- "coreatoms-decl.hh"
- "properties-decl.hh"
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
## mozartcore-decl.hh / mozartcore.hh

<!-- ====================================================== -->
## mozart.hh

<!-- ====================================================== -->
## names-decl.hh / names.hh

<!-- ====================================================== -->
## nullable-decl.hh

<!-- ====================================================== -->
## objects-decl.hh / objects.hh

<!-- ====================================================== -->
## opcodes.hh

<!-- ====================================================== -->
## ozcalls-decl.hh / ozcalls.hh

<!-- ====================================================== -->
## patmattypes-decl.hh / patmattypes.hh

<!-- ====================================================== -->
## pickler.hh / pickler.cc

<!-- ====================================================== -->
## port-decl.hh / port.hh

<!-- ====================================================== -->
## properties.cc

<!-- ====================================================== -->
## properties-config.cc.in

<!-- ====================================================== -->
## properties-decl.hh / properties.hh

<!-- ====================================================== -->
## records-decl.hh / records.hh

<!-- ====================================================== -->
## reference-decl.hh / reference.hh

<!-- ====================================================== -->
## reflectivetypes-decl.hh / reflectivetypes.hh

<!-- ====================================================== -->
## reifiedgnode-decl.hh / reifiedgnode.hh

<!-- ====================================================== -->
## reifiedspace-decl.hh / reifiedspace.hh

<!-- ====================================================== -->
## reifiedthread-decl.hh / reifiedthread.hh

<!-- ====================================================== -->
## runnable-decl.hh / runnable.hh

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

<!-- ====================================================== -->
## sclone-decl.hh / sclone.hh / sclone.cc

### Purpose

Defines a space cloner as a graph replicator.

### Key Classes

- class SpaceCloner: public GraphReplicator;

### Important Functions

- SpaceCloner::doCloneSpace(....);

### Dependencies

- "core-forward-decl.hh";
- "graphreplicator-decl.hh";

<!-- ====================================================== -->
## serializer-decl.hh / serializer.hh / serializer.cc

<!-- ====================================================== -->
## smallint-decl.hh / smallint.hh

<!-- ====================================================== -->
## space-decl.hh / space.hh

### Purpose

The space is an area used by many threads to run their computations.
Spaces can form a tree where nodes are spaces.

### Key Classes

- struct ScriptEntry;
- struct TrailEntry;
- class SpaceScript : public VMAllocatedList<ScriptEntry>;
- class SpaceTrail : public VMAllocatedList<TrailEntry>;
- class Distributor;
- class Space;

### Important Functions
### Dependencies

- "core-forward-decl.hh" : source code
- "store-decl.hh" : source code
- "exceptions-decl.hh" : source code
- "vmallocatedlist-decl.hh" : source code

<!-- ====================================================== -->
## storage-decl.hh / storage.hh

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
- type_traits : template meta-programming

<!-- ====================================================== -->
## store-decl.hh / store.hh

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

<!-- ====================================================== -->
## string-decl.hh / string.hh

<!-- ====================================================== -->
## threadpool-decl.hh / threadpool.hh

### Purpose

Defines the thread queue (queue of pointers of runnables) used by the virtual machine to schedule threads.

Defines also the thread pool used by the virtual machine too to schedule threads and uses a thread queue to do that.

### Key Classes

- class ThreadQueue : public std::queue<Runnable*>;
- class ThreadPool;

### Important Functions
### Dependencies

<!-- ====================================================== -->
## type-decl.hh / type.hh

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

<!-- ====================================================== -->
## typeinfo-decl.hh / typeinfo.hh

<!-- ====================================================== -->
## unify-decl.hh / unify.hh / unify.cc

<!-- ====================================================== -->
## unit-decl.hh / unit.hh

<!-- ====================================================== -->
## unpickler.hh / unpickler.cc

<!-- ====================================================== -->
## utf-decl.hh / utf.hh

### Purpose

Some utf conversions.

### Key Classes
### Important Functions
### Dependencies
### Notes

<!-- ====================================================== -->
## utils-decl.hh / utils.hh

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

<!-- ====================================================== -->
## variables-decl.hh / variables.hh

<!-- ====================================================== -->
## vmallocatedlist-decl.hh / vmallocatedlist.hh

### Purpose

Tiny file defining the virtual machine allocated list as a memory managed list.

### Key Classes

- class VMAllocatedList;

### Important Functions
### Dependencies

- "memmanager.hh";
- "memmanlist.hh";

<!-- ====================================================== -->
## vm-decl.hh / vm.hh / vm.cc

### Purpose

#### Builtin module

A builtin module is a C++ written code that can be called directly by the OZ code at runtime. A builtin module has two values : an atom and a protected node. The atom allows OZ program to import the module using the name defined by this atom. The protected node allows to store the builtin module state into the space memory without being destroyed at a moment by the garbage collector.

All defined modules are inside the directory `modules/` and you can read the dedicated [README.md](modules/README.md) about it.

#### Virtual machine environment

A class defines the whole VM environment. Basically it is a VM manager that can test/set the dynamic preemption mode (of the VM scheduler), it can returns the second memory manager used to replace the old one when the garbage collector does a clean up (TODO : check this is true) or can do other stuff like killing a VM, generating an uuid or launching the garbage collector.

#### Virtual machine

This is one of the most complex class of the project. It is the core of the virtual machine that orchestrates all other parts of the project. This object will run bytecode and schedules threads. Upon instructions it will decide to provide access to data, will manage memory space, will launch the garbage collector when requested, will (de)alloc memory from the memory manager and is the threads scheduler.

A virtual machine has different key components :
- thread pool
- atom table
- global node as root node of the data tree
- virtual machine environment
- memory manager
- exception mechanism
- spaces
- threads
- builtin modules
- garbage collector
- space cloner
- other logistical stuff

### Key Classes

- struct AlarmRecord;
- class BuiltinModule;
- class VirtualMachineEnvironment;
- class VirtualMachine;

### Important Functions

#### BuiltinModule

- atom_t BuiltinModule::getName() : returns builtin module's name;
- StableNode& BuiltinModule::getModule() : returns builtin module;

#### VirtualMachineEnvironment

- bool VirtualMachineEnvironment::useDynamicPreemption() : gets the flag telling if dynamic mode is set or not;
- virtual bool VirtualMachineEnvironment::testDynamicPreemption/ExitRun() : can be redefined by inheriting classes;

- virtual void VirtualMachineEnvironment::withSecondMemoryManager(const std::function<void(MemoryManager&)>& doGC) : runs the garbage collector on the second memory manager;

- virtual void VirtualMachineEnvironment::killVM(VM vm, nativeint exitCode, const std::string& reason) : kills the VM;

- virtual UUID genUUID(VM vm) : generates an UUID;

- virtual void gCollect(GC gc) : runs the garbage collector;

#### VirtualMachine

- void* VirtualMachine::malloc(size_t size) : allocates memory;
- void VirtualMachine::free(void* ptr, size_t size) : frees memory;
- StaticArray<T> VirtualMachine::newStaticArray(size_t size) : static array allocation;
- void VirtualMachine::deleteStaticArray(StaticArray<T> array, size_t size) : static array deallocation;

- void VirtualMachine::onCleanup(VMCleanupListNode* node, const VMCleanupProc& handler) : somewhat shutdowns the VM;

- run_return_type VirtualMachine::run() : core function, this is the thread scheduler;

- void VirtualMachine::setCurrentSpace(Space* space);

- void VirtualMachine::registerBuiltinModule(const std::shared_ptr<BuiltinModule>& module);
- UnstableNode VirtualMachine::findBuiltinModule(T&& name);
- UnstableNode VirtualMachine::findBuiltin(T&& moduleName, U&& builtinName);

- UUID VirtualMachine::genUUID();

- void VirtualMachine::setAlarm(std::int64_t delay, StableNode* wakeable);

- ProtectedNode VirtualMachine::protect(T&& node);

- void VirtualMachine::doGC() : runs the garbage collector;

- atom_t VirtualMachine::getAtom(...., const chat* data);
- atom_t VirtualMachine::getAtom(....);
- unique_name_t VirtualMachine::getUniqueName(...., const char* data);

- void VirtualMachine::requestPreempt() : requests the preemption of the current thread (TODO check if it is true);
- void VirtualMachine::requestExitRun() : requests the ending of the current thread (TODO check if it is true);
- void VirtualMachine::requestGC() : requests the garbage collect to be run;

### Dependencies

- [cstdlib](https://cplusplus.com/reference/cstdlib/) : reference of <stdlib.h> from C.
- [forward_list](https://cplusplus.com/reference/forward_list/forward_list/) : singly linked list definition template.
- [atomic](https://cplusplus.com/reference/atomic/atomic/) : atomic variables.
- [core-forward-decl.hh](#core-forward-declhh)
- [memmanager.hh](#memmanagerhh--memmanagercc)
- "store-decl.hh"
- "threadpool-decl.hh"
- "gcollect-decl.hh"
- "sclone-decl.hh"
- "space-decl.hh"
- "uuid-decl.hh"
- "vmallocatedlist-decl.hh"
- "atomtable.hh"
- "bigintimplem-decl.hh"
- "coreatoms-decl.hh"
- "properties-decl.hh"

<!-- ====================================================== -->
## weakrefs-decl.hh / weakrefs.hh

