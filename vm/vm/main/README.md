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

<!-- ====================================================== -->
## gcollect.hh

<!-- ====================================================== -->
## graphreplicator-decl.hh

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

<!-- ====================================================== -->
## memword.hh

### Purpose

Defines the structure of a memory word.

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

<!-- ====================================================== -->
## runnable.hh

<!-- ====================================================== -->
## sclone.cc

<!-- ====================================================== -->
## sclone-decl.hh

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

<!-- ====================================================== -->
## space.hh

<!-- ====================================================== -->
## storage-decl.hh

<!-- ====================================================== -->
## storage.hh

<!-- ====================================================== -->
## store-decl.hh

<!-- ====================================================== -->
## store.hh

<!-- ====================================================== -->
## string-decl.hh

<!-- ====================================================== -->
## string.hh

<!-- ====================================================== -->
## threadpool-decl.hh

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

<!-- ====================================================== -->
## variables-decl.hh

<!-- ====================================================== -->
## variables.hh

<!-- ====================================================== -->
## vmallocatedlist-decl.hh

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

