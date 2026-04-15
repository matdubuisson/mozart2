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

#ifndef MOZART_EMULATE_H
#define MOZART_EMULATE_H

#include "mozartcore.hh"

#include <utility>
#include <stack>
#include <cassert>

namespace mozart {

struct DebugEntry {
  StableNode* file;
  size_t lineNumber;
  size_t columnNumber;
  StableNode* kind;
  bool valid;

  DebugEntry();
  DebugEntry(DebugEntry&&);
  DebugEntry(const DebugEntry&) = delete;
  DebugEntry& operator=(DebugEntry&&);
  DebugEntry& operator=(const DebugEntry&) = delete;

  DebugEntry(GR gr, const DebugEntry& other);
};

/**
 * Entry of a thread stack
 */
struct StackEntry {
  /** Create a regular stack entry */
  StackEntry(StableNode* abstraction, ProgramCounter PC, size_t yregCount,
    StaticArray<UnstableNode> yregs, StaticArray<StableNode> gregs,
    StaticArray<StableNode> kregs, DebugEntry&& debugEntry):
    abstraction(abstraction), PC(PC), yregCount(yregCount),
    yregs(yregs), gregs(gregs), kregs(kregs), debugEntry(std::move(debugEntry)) {}

  /** Create a catch stack entry */
  StackEntry(ProgramCounter PC, DebugEntry&& debugEntry):
    abstraction(nullptr), PC(PC), yregCount(0),
    yregs(nullptr), gregs(nullptr), kregs(nullptr), debugEntry(std::move(debugEntry)) {}

  inline
  StackEntry(GR gr, StackEntry& from);

  inline
  void beforeGR(VM vm, StableNode*& abs);

  inline
  void afterGR(VM vm, StableNode*& abs);

  bool isExceptionHandler() {
    return abstraction == nullptr;
  }

  StableNode* abstraction; // nullptr means this is an exception handler

  union {
    ProgramCounter PC;       // Normal
    std::ptrdiff_t PCOffset; // During GR
  };

  // The following is meaningfull only for regular stack entries

  size_t yregCount;
  StaticArray<UnstableNode> yregs;

  StaticArray<StableNode> gregs; // Irrelevant during GR
  StaticArray<StableNode> kregs; // Irrelevant during GR

  // Debugging information
  DebugEntry debugEntry;
};

/**
 * Thread stack with frames and exception handlers
 */
class ThreadStack: public VMAllocatedList<StackEntry> {
public:
  void pushExceptionHandler(VM vm, ProgramCounter PC, DebugEntry&& entry) {
    push_front_new(vm, PC, std::move(entry));
  }

  void popExceptionHandler(VM vm, DebugEntry& entry) {
    assert(front().isExceptionHandler());
    entry = std::move(front().debugEntry);
    remove_front(vm);
  }

  inline
  bool findExceptionHandler(VM vm, StableNode*& abstraction,
                            ProgramCounter& PC, size_t& yregCount,
                            StaticArray<UnstableNode>& yregs,
                            StaticArray<StableNode>& gregs,
                            StaticArray<StableNode>& kregs);

  inline
  UnstableNode buildStackTrace(VM vm, StableNode* abstraction,
                               ProgramCounter PC,
                               const DebugEntry& debugEntry);
};

class XRegArray {
public:
  /** Instantiates an empty array of X registers */
  XRegArray() : _array(nullptr, 0), _size(0) {}

  /**
   * Initializes the array with 'initialSize' X registers
   * @param vm The virtual machine
   * @param initialSize The initial size of the array
   * @note All X registers are initialized to unit
   */
  void init(VM vm, size_t initialSize) {
    assert(_array == nullptr);
    allocArray(vm, initialSize);

    for (size_t i = 0; i < initialSize; i++)
      _array[i].init(vm);
  }

  /**
   * Initializes a new array with more capacity for X registers 'newSize',
   * copies 'elemsToKeep' first old X registers into the new array and
   * initializes the new X registers
   * @param vm The virtual machine
   * @param newSize The new size of the array
   * @param elemsToKeep The number of X registers to copy from the old array
   */
  void grow(VM vm, size_t newSize, size_t elemsToKeep) {
    if (newSize <= _size)
      return;

    StaticArray<UnstableNode> oldArray = _array;
    size_t oldSize = _size;

    allocArray(vm, newSize);

    for (size_t i = 0; i < elemsToKeep; i++)
      _array[i] = std::move(oldArray[i]); // freed just below, so that's OK

    freeArray(vm, oldArray, oldSize);

    for (size_t i = elemsToKeep; i < newSize; i++)
      _array[i].init(vm);
  }

  /**
   * Releases the allocated memory of the X registers array
   * @param vm The virtual machine
   */
  void release(VM vm) {
    freeArray(vm, _array, _size);
    _array = nullptr;
    _size = 0;
  }

  /** Gets the size of the array */
  size_t size() {
    return _size;
  }

  /**
   * Gets a reference on the indexth element of the X registers array
   * @param index The index of the X register to get
   * @returns A reference on the indexth element of the X registers array as unstable node
   */
  UnstableNode& operator[](size_t index) {
    return _array[index];
  }

  /**
   * Gets the array of X registers as a static array of unstable nodes
   * @returns The array of X registers
   */
  StaticArray<UnstableNode> getArray() {
    return _array;
  }
private:
  /**
   * Asks the provided virtual machine to allocate a new static array
   * of unstable nodes and set the size
   * @param vm The virtual machine
   * @param size The size of the new array
   */
  void allocArray(VM vm, size_t size) {
    _array = vm->newStaticArray<UnstableNode>(size);
    _size = size;
  }

  /**
   * Asks the provided virtual machine to deallocate a static array of unstable nodes
   * @param vm The virtual machine
   * @param array The array to deallocate
   * @param size The size of the array to deallocate
   */
  void freeArray(VM vm, StaticArray<UnstableNode> array, size_t size) {
    vm->deleteStaticArray<UnstableNode>(array, size);
  }

  StaticArray<UnstableNode> _array;
  size_t _size;
};

/**
 * Lightweight thread.
 * The Thread class contains the information about the execution of a
 * lightweight thread. It contains the main emulator loop.
 */
class Thread : public Runnable {
private:
  typedef Runnable Super;

  static constexpr size_t InitXRegisters = 64;
public:
  /**
   * Creates a new light weight thread without arguments
   * @param vm The virtual machine
   * @param space The space for the runnable (that is the thread)
   * @param abstraction The body of the thread TO-COMPLETE
   * @param createSuspended True if the thread has to be scheduled at creation
   */
  Thread(VM vm, Space* space, RichNode abstraction,
         bool createSuspended = false);

  /**
   * Creates a new light weight thread
   * @param vm The virtual machine
   * @param space The space for the runnable (that is the thread)
   * @param abstraction The body of the thread TO-COMPLETE
   * @param argc The number of values passed (the number of X registers)
   * @param args The values to copy into X registers
   * @param createSuspended True if the thread has to be scheduled at creation
   */
  Thread(VM vm, Space* space, RichNode abstraction,
         size_t argc, RichNode args[],
         bool createSuspended = false);

  /**
   * Creates a new light weight thread as a copy of another one
   * X registers, thread stack, exceptions and termination variable are copied
   * @param gr The graph replicator
   * @param from A reference on the light weight thread to copy
   */
  inline
  Thread(GR gr, Thread& from);

  /** Runs the thread */
  void run();

  /** Kills the thread */
  void kill() {
    Super::kill();
  }

public:
  /** Gets a reference on the termination variable returned by the thread at end of life */
  StableNode& getTerminationVar() {
    return _terminationVar;
  }

  /**
   * Injects a new into the thread
   * @param exception A pointer on the stable node representing the exception
   */
  void injectException(StableNode* exception) {
    injectedException = exception;

    if (!isRunnable())
      resume();
  }

public:
  /** Prepares the thread and all its elements on its stack to graph replication */
  void beforeGR();

  /** Prepares the thread and all its elements on its stack to get back to normal after graph replication */
  void afterGR();

  /**
   * Collects the new thread for the garbage collector
   * @param gc The garbage collector
   */
  Runnable* gCollect(GC gc);

  /**
   * Collects the new thread for the space cloner
   * @param sc The space cloner
   */
  Runnable* sClone(SC sc);

protected:
  /** Terminates the thread and binds the termination variable */
  inline
  void terminate();

  /** Disposes the thread and destroy X and Y registers */
  void dispose() {
    xregs.release(vm);

    while (!stack.empty()) {
      StackEntry& entry = stack.front();
      if (entry.yregCount != 0)
        vm->deleteStaticArray<UnstableNode>(entry.yregs, entry.yregCount);
      stack.remove_front(vm);
    }

    Super::dispose();
  }
public:
  /**
   * Dumps the thread
   * It actually prints the thread on stderr
   */
  void dump();
private:
  /**
   * Creates a new light weight thread
   * @param vm The virtual machine
   * @param abstraction The body of the thread TO-COMPLETE
   * @param argc The number of values passed (the number of X registers)
   * @param args The values to copy into X registers
   * @param createSuspended True if the thread has to be scheduled at creation
   */
  inline
  void constructor(VM vm, RichNode abstraction,
                   size_t argc, RichNode args[],
                   bool createSuspended);

  /**
   * Pushes a new frame on the thread stack with the provided information
   * @param vm The virtual machine
   * @param abstraction The abstraction
   * @param PC The program counter
   * @param yregCount The number of Y registers
   * @param yregs The Y registers
   * @param gregs The G registers
   * @param kregs The K registers
   * @param debugEntry The debug entry
   */
  inline
  void pushFrame(VM vm, StableNode* abstraction,
                 ProgramCounter PC, size_t yregCount,
                 StaticArray<UnstableNode> yregs,
                 StaticArray<StableNode> gregs,
                 StaticArray<StableNode> kregs,
                 DebugEntry&& debugEntry);

  /**
   * Pops a frame from the thread stack and loads this frame into
   * the provided references as arguments
   * @param vm The virtual machine
   * @param abstraction A reference to store the abstraction
   * @param PC A reference to store the program counter
   * @param yregCount A reference to store the number of Y registers
   * @param yregs A reference to store the Y registers
   * @param gregs A reference to store the G registers
   * @param kregs A reference to store the K registers
   * @param debugEntry A reference to store the debug entry
   */
  inline
  void popFrame(VM vm, StableNode*& abstraction,
                ProgramCounter& PC, size_t& yregCount,
                StaticArray<UnstableNode>& yregs,
                StaticArray<StableNode>& gregs,
                StaticArray<StableNode>& kregs,
                DebugEntry& debugEntry);

  
  void call(RichNode target, size_t actualArity, bool isTailCall,
            VM vm, StableNode*& abstraction,
            ProgramCounter& PC, size_t& yregCount,
            XRegArray* xregs,
            StaticArray<UnstableNode>& yregs,
            StaticArray<StableNode>& gregs,
            StaticArray<StableNode>& kregs,
            DebugEntry&& debugEntry,
            bool& preempted,
            std::ptrdiff_t opcodeArgCount = 2);

  void sendMsg(RichNode target, RichNode labelOrArity, size_t width,
               bool isTailCall,
               VM vm, StableNode*& abstraction,
               ProgramCounter& PC, size_t& yregCount,
               XRegArray* xregs,
               StaticArray<UnstableNode>& yregs,
               StaticArray<StableNode>& gregs,
               StaticArray<StableNode>& kregs,
               DebugEntry&& debugEntry,
               bool& preempted);

  /**
   * Gets call information for a reflective target containing a callable object
   * @param vm The virtual machine
   * @param target A reference on a rich node representing the reflective target to call
   * @param arity A reference to store the arity
   * @param start A reference to store the program counter of the beginning
   * @param Xcount A reference to store the number of X registers to copy
   * @param Gs A reference to store the G registers to copy
   * @param Ks A reference to store the K registers to copy
   */
  inline
  void doGetCallInfo(VM vm, RichNode& target, size_t& arity,
                     ProgramCounter& start, size_t& Xcount,
                     StaticArray<StableNode>& Gs,
                     StaticArray<StableNode>& Ks);

  /**
   * TO-COMPLETE
   * Dereferences a reflective target and sets the result into
   * the original target reference
   * @param vm The virtual machine
   * @param target A reference on a rich node representing the reflective target to dereference
   */
  inline
  void derefReflectiveTarget(VM vm, RichNode& target);

  /**
   * Applies pattern matching on the provided value according to the provided patterns
   * @param vm The virtual machine
   * @param value A rich node representing the value
   * @param patterns A rich node representing the patterns
   * @param abstraction A reference pointer to store the abstraction
   * @param PC A reference to store the program counter
   * @param yregCount A reference to store the number of Y registers
   * @param xregs A reference to the array of X registers
   * @param yregs A reference to the array of Y registers
   * @param gregs A reference to the array of G registers
   * @param kregs A reference to the array of K registers
   * @param preempted A reference to store if the pattern matching has been preempted or not
   */
  void patternMatch(VM vm, RichNode value, RichNode patterns,
                    StableNode*& abstraction,
                    ProgramCounter& PC, size_t& yregCount,
                    XRegArray* xregs,
                    StaticArray<UnstableNode>& yregs,
                    StaticArray<StableNode>& gregs,
                    StaticArray<StableNode>& kregs,
                    bool& preempted);

  /**
   * Applies a failure on the provided information for the thread.
   * The thread raises an exception that will terminate it and prints
   * the failure message to the screen
   * @param vm The virtual machine
   * @param info A rich node representing the information about the failure
   * @param abstraction A reference pointer to store the abstraction
   * @param PC A reference to store the program counter
   * @param yregCount A reference to store the number of Y registers
   * @param xregs A reference to the array of X registers
   * @param yregs A reference to the array of Y registers
   * @param gregs A reference to the array of G registers
   * @param kregs A reference to the array of K registers
   * @param debugEntry The debug entry to use for the failure
   */
  void applyFail(VM vm, RichNode info,
                 StableNode*& abstraction,
                 ProgramCounter& PC, size_t& yregCount,
                 XRegArray* xregs,
                 StaticArray<UnstableNode>& yregs,
                 StaticArray<StableNode>& gregs,
                 StaticArray<StableNode>& kregs,
                 DebugEntry&& debugEntry);

  /**
   * Applies a wait before on the provided information for the thread.
   * The thread is suspended on the waitee variable and the variable is
   * marked as needed by the thread
   * @param vm The virtual machine
   * @param waitee A rich node representing the waitee
   * @param isQuiet True if the wait before is quiet, false otherwise
   * @param abstraction A reference pointer to store the abstraction
   * @param PC A reference to store the program counter
   * @param yregCount A reference to store the number of Y registers
   * @param xregs A reference to the array of X registers
   * @param yregs A reference to the array of Y registers
   * @param gregs A reference to the array of G registers
   * @param kregs A reference to the array of K registers
   * @param debugEntry The debug entry to use for the wait before
   * @note If the waitee value is a failed value, the apply wait before will result in a apply raise
   */
  void applyWaitBefore(VM vm, RichNode waitee, bool isQuiet,
                       StableNode*& abstraction,
                       ProgramCounter& PC, size_t& yregCount,
                       XRegArray* xregs,
                       StaticArray<UnstableNode>& yregs,
                       StaticArray<StableNode>& gregs,
                       StaticArray<StableNode>& kregs,
                       DebugEntry&& debugEntry);

  /**
   * Applies a raise on the provided information for the thread.
   * If there is an handler, it will be called with the raised exception else
   * the thread is immediately terminated and an uncaught exception message is
   * printed to the screen
   * @param vm The virtual machine
   * @param exception A rich node representing the exception to raise
   * @param abstraction A reference pointer to store the abstraction
   * @param PC A reference to store the program counter
   * @param yregCount A reference to store the number of Y registers
   * @param xregs A reference to the array of X registers
   * @param yregs A reference to the array of Y registers
   * @param gregs A reference to the array of G registers
   * @param kregs A reference to the array of K registers
   * @param debugEntry The debug entry to use for the raise
   */
  void applyRaise(VM vm, RichNode exception,
                  StableNode*& abstraction,
                  ProgramCounter& PC, size_t& yregCount,
                  XRegArray* xregs,
                  StaticArray<UnstableNode>& yregs,
                  StaticArray<StableNode>& gregs,
                  StaticArray<StableNode>& kregs,
                  DebugEntry&& debugEntry);

  /**
   * Preprocesses an exception TO-COMPLETE
   * @param vm The virtual machine
   * @param exception A rich node representing the exception to preprocess
   * @param abstraction A pointer to the stable node representing the abstraction
   * @param PC The program counter
   * @param kregs The K registers to use
   * @param debugEntry The debug entry to use
   */
  UnstableNode preprocessException(VM vm, RichNode exception,
                                   StableNode* abstraction,
                                   ProgramCounter PC,
                                   StaticArray<StableNode>& kregs,
                                   const DebugEntry& debugEntry);

  XRegArray xregs;
  ThreadStack stack;
  StableNode* injectedException;
  StableNode _terminationVar;
};

}

#endif // MOZART_EMULATE_H
