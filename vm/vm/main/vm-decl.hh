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

#ifndef MOZART_VM_DECL_H
#define MOZART_VM_DECL_H

#include <cstdlib>
#include <forward_list>
#include <atomic>

#include "core-forward-decl.hh"

#include "memmanager.hh"

#include "store-decl.hh"
#include "threadpool-decl.hh"
#include "gcollect-decl.hh"
#include "sclone-decl.hh"
#include "space-decl.hh"
#include "uuid-decl.hh"
#include "vmallocatedlist-decl.hh"

#include "atomtable.hh"
#include "bigintimplem-decl.hh"
#include "coreatoms-decl.hh"
#include "properties-decl.hh"

#include "debugger-decl.hh"

namespace mozart {

///////////////////
// BuiltinModule //
///////////////////

class BuiltinModule {
public:
  /**
   * @param vm The virtual machine
   * @param name The name of the built-in module to invoke
   */
  inline
  BuiltinModule(VM vm, const char* name);

  virtual ~BuiltinModule() {}

  /** Gets the name of the built-in module as an atom */
  atom_t getName() {
    return _name;
  }

  /**
   * Provides access to a built-in module
   * @returns The built-in module as a stable node
   */
  StableNode& getModule() {
    return *_module;
  }
protected:
  /**
   * Inits the built-in module into a protected node
   * @param vm The virtual machine
   * @param module A pointer on a node containing the module (generally unstable)
   */
  template <typename T>
  inline
  void initModule(VM vm, T&& module);
private:
  atom_t _name;
  ProtectedNode _module;
};

////////////////////
// VirtualMachine //
////////////////////

class VirtualMachineEnvironment {
public:
  VirtualMachineEnvironment(): _useDynamicPreemption(false) {}

  VirtualMachineEnvironment(bool useDynamicPreemption):
    _useDynamicPreemption(useDynamicPreemption) {}

  VirtualMachineEnvironment(const VirtualMachineEnvironment&) = delete;

  bool useDynamicPreemption() {
    return _useDynamicPreemption;
  }

  virtual bool testDynamicPreemption() {
    return false;
  }

  virtual bool testDynamicExitRun() {
    return false;
  }

// The following methods assume a single VM per process
public:
  // Must only be used for initialization
  MemoryManager& getSecondMemoryManagerRef() {
    return _secondMemoryManager;
  }

  virtual void withSecondMemoryManager(const std::function<void(MemoryManager&)>& doGC) {
    doGC(_secondMemoryManager);
  }

  virtual void killVM(VM vm, nativeint exitCode, const std::string& reason) {
    std::exit(exitCode);
  }

// Miscellaneous
public:
  virtual UUID genUUID(VM vm) = 0;

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, nativeint value);

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, double value);

  inline
  virtual std::shared_ptr<BigIntImplem> newBigIntImplem(VM vm, const std::string& value);

  inline
  virtual void sendOnVMPort(VM from, VMIdentifier to, RichNode value);

  virtual void gCollect(GC gc) {
  }

protected:
  MemoryManager _secondMemoryManager;
private:
  bool _useDynamicPreemption;
};

class VirtualMachine {
public:
  /**
   * Exit codes allows the virtual machine to process runnable entities differently when they exit :
   * - recNeverInvokeAgain: ....
   * - recInvokeAgainNow: ....
   * - recInvokeAgainLater: ....
   */
  enum RunExitCode {
    recNeverInvokeAgain, recInvokeAgainNow, recInvokeAgainLater
  };

  typedef std::pair<RunExitCode, std::int64_t> run_return_type;
private:
  struct AlarmRecord {
    AlarmRecord(std::int64_t expiration, StableNode* wakeable):
      expiration(expiration), wakeable(wakeable) {}

    std::int64_t expiration;
    StableNode* wakeable;
  };
public:
  /**
   * Inits the virtual machine, basically it means several parts to initiate :
   * 0) Garbage collector
   * 1) Space cloner
   * 2) Memory manager
   * 3) Top level space
   * 4) Built-in modules dictionary
   * 5) VM configuration TO-COMPLETE
   * @param environment The VM environment defines the memory manager and the preemption mode
   * @param options The VM options define the heap allocation boundaries
   */
  inline
  VirtualMachine(VirtualMachineEnvironment& environment,
                 VirtualMachineOptions options);

  /** The virtual machine cannot be copied */
  VirtualMachine(const VirtualMachine& src) = delete;

  /** Deconstructor = doCleanup() */
  inline
  ~VirtualMachine();

  /**
   * Allocates memory using VM's memory manager
   * @param size The size of the new memory block
   */
  void* malloc(size_t size) {
    return memoryManager.malloc(size);
  }

   /**
   * Frees memory using VM's memory manager
   * @param ptr The address of the memory block previously allocated
   * @param size The size of the new memory block
   */
  void free(void* ptr, size_t size) {
    memoryManager.free(ptr, size);
  }

  /**
   * Allocates a static array able to contain N elements of type T
   * T is generally UnstableNode, RichNode, nativeint or UnstableField
   * @param size The number of elements of size sizeof(T)
   * @note So the total allocated memory is size * sizeof(T)
   */
  template <class T>
  StaticArray<T> newStaticArray(size_t size) {
    void* memory = malloc(size * sizeof(T));
    return StaticArray<T>(static_cast<T*>(memory), size);
  }

  /**
   * Deallocates a pre-allocated static array
   * @param size The number of elements of size * sizeof(T)
   * @note So the total allocated memory is size * sizeof(T)
   */
  template <class T>
  void deleteStaticArray(StaticArray<T> array, size_t size) {
    void* memory = static_cast<void*>((T*) array);
    free(memory, size * sizeof(T));
  }

  /**
   * Pushs a new cleaning task on the clean-up list
   * @param handler Simple void procedure taking the VM as argument
   */
  void onCleanup(const VMCleanupProc& handler) {
    onCleanup(new (this) VMCleanupListNode, handler);
  }

  /**
   * Pushs a new cleaning task on the clean-up list
   * @param node Cleanup node containing a clean-up task (handler) and the next cleanup node
   * @param handler Simple void procedure taking the VM as argument
   */
  void onCleanup(VMCleanupListNode* node, const VMCleanupProc& handler) {
    node->handler = handler;
    node->next = _cleanupList;
    _cleanupList = node;
  }

  /**
   * Runs the virtual machine
   * @returns The virtual machine's exit code
   * @note This is a main procedure that is basically the scheduler orchestrating threads and internal entities as the garbage collector for example
   */
  run_return_type run();

  /**
   * Tests if the threads have to be preempted (for example running the garbage collector)
   * @returns true if the threads should be preempted
   */
  inline
  bool testPreemption();

  /** Gets the thread-pool of the VM */
  ThreadPool& getThreadPool() { return threadPool; }

  /** Get the memory manager of the VM */
  MemoryManager& getMemoryManager() {
    return memoryManager;
  }

  /** Gets the heap size of the VM from property registry */
  size_t getHeapSize() {
    return getPropertyRegistry().config.heapSize;
  }

  /**
   * Adjusts the heap size according to property registry and using the garbage collector
   * @note
   *   It uses a doubling size approach for increasing or decreasing the heap size
   *   The wished new heap size is based on the garbage collector's threshold
   *   and is bound by a minimal and maximal heap size from the property registry
   *   Once the size is modified, the garbage collector is requested to modify the heap according to the new size
   */
  inline
  void adjustHeapSize();

  /** Gets global exception mechanism */
  GlobalExceptionMechanism& getGlobalExceptionMechanism() {
    return exceptionMechanism;
  }

  /** Gets the top level space */
  Space* getTopLevelSpace() {
    return _topLevelSpace;
  }

  /** Gets the current space */
  Space* getCurrentSpace() {
    return _currentSpace;
  }

  /** Gets the current thread (runnable) */
  Runnable* getCurrentThread() {
    return _currentThread;
  }

  /** Tells if the current space is on top level */
  bool isOnTopLevel() {
    return _isOnTopLevel;
  }

  /** Tells if a current thread containing an intermediate state is available */
  bool isIntermediateStateAvailable() {
    return _currentThread != nullptr;
  }

  /**
   * Gets the intermediate state of the current thread
   * @note A current thread has to be available isIntermediateStateAvailable() has to be true
   */
  IntermediateState& getIntermediateState() {
    assert(isIntermediateStateAvailable());
    return _currentThread->getIntermediateState();
  }

  /**
   * Sets a new current space
   * @param space The new space (it might be on top level)
   */
  inline
  void setCurrentSpace(Space* space);

  /**
   * Clones the space using VM's space cloner
   * @param space The space to clone
   * @returns The cloned space
   */
  inline
  Space* cloneSpace(Space* space);

  /** Gets the virtual machine environment as reference */
  VirtualMachineEnvironment& getEnvironment() {
    return environment;
  }

public:
  /**
   * Registers a new built-in modulo into the built-in modules dictionary owned by the VM
   * @param module A shared pointer on the module
   */
  inline
  void registerBuiltinModule(const std::shared_ptr<BuiltinModule>& module);

  /**
   * Finds the built-in module with a lookup in the built-in modules dictionary
   * @param name A referenced reference of an object of type T (generally UnstableNode)
   * @note If the module cannot be found a run-time error is raised
   */
  template <typename T>
  inline
  UnstableNode findBuiltinModule(T&& name);

  /**
   * Finds the built-in from a specified built-in module by performing a lookup
   * @param moduleName The built-in module name
   * @param builtinName The built-in name
   */
  template <typename T, typename U>
  inline
  UnstableNode findBuiltin(T&& moduleName, U&& builtinName);

public:
  /** Gets property registry */
  PropertyRegistry& getPropertyRegistry() {
    return _propertyRegistry;
  }

  /**
   * Generates a new UUID from the VM environment
   * @returns The generated UUID
   */
  inline
  UUID genUUID();

  /**
   * Gets atomically the reference time
   * @returns The reference time
   */
  std::int64_t getReferenceTime() {
    return _referenceTime.load(std::memory_order_acquire);
  }

  /**
   * Inserts a new alarm in the list sorted by expiration according to the reference time
   * @param delay The delay in milliseconds TO-COMPLETE : Checks it true
   * @param wakeable A reference on the alarm as a stable node
   */
  inline
  void setAlarm(std::int64_t delay, StableNode* wakeable);

  /** Gets the pickle types record */
  StableNode* getPickleTypesRecord() {
    return _pickleTypesRecord;
  }

public:
  /**
   * Converts a native int to a big int
   * @param value A native int
   * @returns The converted value to big int
   */
  inline
  std::shared_ptr<BigIntImplem> newBigIntImplem(nativeint value);

public:
  CoreAtoms coreatoms;

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const char* data) {
    return atomTable.get(this, data);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param length The length to read in data
   * @param data The atom name
   * @returns An atom
   */
  atom_t getAtom(size_t length, const char* data) {
    return atomTable.get(this, length, data);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const BaseLString<char>& data) {
    return atomTable.get(this, data.length, data.string);
  }

  /**
   * Gets an atom from the atom table by its name
   * @param data The atom name TO-COMPLETE : check if really the name
   * @returns An atom
   */
  atom_t getAtom(const std::string& data) {
    return atomTable.get(this, data.length(), data.c_str());
  }

  /**
   * Gets an unique name from the atom table by its name
   * @param data The unique name
   * @returns An unique name
   */
  unique_name_t getUniqueName(const char* data) {
    return atomTable.getUniqueName(this, data);
  }

  /**
   * Gets an unique name from the atom table by its name
   * @param length The length to read in data
   * @param data The unique name
   * @returns An unique name
   */
  unique_name_t getUniqueName(size_t length, const char* data) {
    return atomTable.getUniqueName(this, length, data);
  }
public:
  /**
   * Protects a node from the garbage collector
   * @param node The node to protect
   * @returns A reference-counted ref to that node
   */
  template <typename T>
  inline
  ProtectedNode protect(T&& node);
public:
  // Influence from the external world

  /** Requests preemption of the current thread TO-COMPLETE : check if 100% correct */
  void requestPreempt() {
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  /** Requests virtual machine run termination */
  void requestExitRun() {
    // The order of these two operations *is* important
    _exitRunRequestedNot.clear(std::memory_order_release);
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  /** Requests VM's garbage collector to be run */
  void requestGC() {
    // The order of these two operations *is* important
    _gcRequestedNot.clear(std::memory_order_release);
    _preemptRequestedNot.clear(std::memory_order_release);
  }

  /**
   * Sets the reference time
   * @param value The new time in milliseconds TO-COMPLETE : check unit
   */
  void setReferenceTime(std::int64_t value) {
    _referenceTime.store(value, std::memory_order_release);
  }
private:
  /** Checks if preemption is requested and clears the flag */
  bool testAndClearPreemptRequested() {
    return !_preemptRequestedNot.test_and_set(std::memory_order_acquire);
  }

  /** Checks if termination is requested and clears the flag */
  bool testAndClearExitRunRequested() {
    return !_exitRunRequestedNot.test_and_set(std::memory_order_acquire);
  }

  /** Checks if garbage collector is requested and clears the flag */
  bool testAndClearGCRequested() {
    return !_gcRequestedNot.test_and_set(std::memory_order_acquire);
  }
private:
  friend class GarbageCollector;
  friend class SpaceCloner;
  friend class Runnable;
  friend class GlobalNode;

  friend void* ::operator new (size_t size, mozart::VM vm);
  friend void* ::operator new[] (size_t size, mozart::VM vm);

  /**
   * Gets memory from the pre-allocated memory of VM's memory manager
   * @param size The amount of memory to alloc
   * @returns A pointer on the allocated block
   */
  void* getMemory(size_t size) {
    return memoryManager.getMemory(size);
  }

  /** Initializes the coreatomes from VM's atom table */
  inline
  void initialize();

  /**
   * Using a second memory manager from VM's environment
   * it runs the garbage collector
   */
  inline
  void doGC();

  /**
   * Updates the top level space if necessary
   * Prepares all alive threads to a garbage collection
   * @param gr The garbage collector
   */
  inline
  void beforeGR(GR gr);

  /**
   * Updates the top level space if necessary
   * Prepares all alive threads to return to normal after garbage collection
   * @param gr The garbage collector
   */
  inline
  void afterGR(GR gr);

  /**
   * To process with the garbage collection it does several steps :
   * 0) Swaps a the second memory manager and initializes it
   * 1) Reinitializes the VM (atom table, alive threads list and alarms)
   * 2) Copies the top level space
   * 3) Reinitializes the built-in modules
   * 4) Runs the garbage collector on property registry
   * 5) Runs the garbage collector on the thread pool
   * 6) Runs the garbage collector on the protected nodes
   * 7) Copies the alarms in the new alarms list
   * 8) Copies the types record
   * 9) Runs the garbage collector on the VM's environment
   * @param gc The garbage collector
   * @param secondMemoryManager The new memory manager
   */
  inline
  void startGC(GC gc, MemoryManager& secondMemoryManager);

  /**
   * Thanks to smart pointers, detects the protected nodes non-referenced anymore
   * and so deletes them
   * @param gc The garbage collector
   */
  inline
  void gcProtectedNodes(GC gc);

  /**
   * Gets the cleanup list and sets the internal reference to null
   * @returns A cleanup list
   */
  inline
  VMCleanupListNode* acquireCleanupList();

  /**
   * Runs each handler of the cleanup list one by one on the virtual machine
   * @param cleanupList A list of VMCleanupListNode representing the handlers
   */
  inline
  void doCleanup(VMCleanupListNode* cleanupList);

  /** Runs each handler of the cleanup list one by one on the virtual machine */
  void doCleanup() {
    doCleanup(acquireCleanupList());
  }

  ThreadPool threadPool;
  AtomTable atomTable;
  GlobalNode* rootGlobalNode;

  VirtualMachineEnvironment& environment;

  MemoryManager memoryManager;

  GlobalExceptionMechanism exceptionMechanism;

  Space* _topLevelSpace;
  Space* _currentSpace;
  Runnable* _currentThread;
  bool _isOnTopLevel;

  NodeDictionary* _builtinModules;
  PropertyRegistry _propertyRegistry;

  RunnableList aliveThreads;
  VMCleanupListNode* _cleanupList;

  GarbageCollector gc;
  SpaceCloner sc;

  Debugger dbg;

  VMAllocatedList<AlarmRecord> _alarms;
  StableNode* _pickleTypesRecord;
  std::forward_list<std::weak_ptr<StableNode*>> _protectedNodes;

  // Flags set externally for preemption etc.
  // TODO Use atomic data types
  bool _envUseDynamicPreemption;
  std::atomic_flag _preemptRequestedNot;
  std::atomic_flag _exitRunRequestedNot;
  std::atomic_flag _gcRequestedNot;
  std::atomic<std::int64_t> _referenceTime;

  // During GC, we need a SpaceRef version of the top-level space
  SpaceRef _topLevelSpaceRef;
};

}

#endif // MOZART_VM_DECL_H
