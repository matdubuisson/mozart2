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

#ifndef MOZART_VARIABLES_DECL_H
#define MOZART_VARIABLES_DECL_H

#include "mozartcore-decl.hh"

// TO-COMPLETE : review all comments because some could be misunderstood or not 100% accurate

namespace mozart {

//////////////////
// VariableBase //
//////////////////

template <class This>
class VariableBase: public WithHome {
public:
  /**
   * Declares a new variable base
   * @param vm The virtual machine in which the variable base is being created
   * @note By default the variable is not needed
   */
  explicit VariableBase(VM vm): WithHome(vm), _needed(false) {}

  /**
   * Declares a new variable base
   * @param vm The virtual machine in which the variable base is being created
   * @param home The home space of the variable base
   * @note By default the variable is not needed
   */
  VariableBase(VM vm, Space* home): WithHome(home), _needed(false) {}

  /**
   * Declares a new variable base with copies of the provided variable's pendings
   * @param vm The virtual machine in which the variable base is being created
   * @param gr The graph replicator used to copy from's home space
   * @param from The variable base to copy its space
   * @note The variable is needed iff the provided variable is needed too
   */
  inline
  VariableBase(VM vm, GR gr, This& from);

public:
  // DataflowVariable interface

  /**
   * Adds a variable to the suspend list as a reference (pointer) of a stable node
   * @param vm The virtual machine
   * @param variable The variable to add to the suspend list as a rich node
   */
  inline
  void addToSuspendList(VM vm, RichNode variable);

  /**
   * Tells if the variable is needed
   * @param vm The virtual machine
   * @returns true if the variable is needed, false otherwise
   */
  bool isNeeded(VM vm) {
    return _needed;
  }

  /**
   * Marks the variable as needed
   * @param vm The virtual machine
   * @note Marking needed a variable for the first time may cause the wake up of some pendings
   */
  inline
  void markNeeded(VM vm);

  /* To be implemented in subclasses
  inline
  void bind(VM vm, RichNode src);
  */

protected:
  /**
   * Binds the provided self variable using the source variable
   * where both are rich nodes
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   * @note src can be triggered if it is lazyly generated
   */
  inline
  void doBind(RichNode self, VM vm, RichNode src);

private:
  // TODO Might be a good candidate for noinline
  /**
   * Binds the provided self variable using the source variable
   * where both are rich nodes in a subspace
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   * @note src can be triggered if it is lazyly generated
   */
  inline
  void bindSubSpace(RichNode self, VM vm, RichNode src);

  /**
   * Wakes up the pendings of the variable (ie threads suspended on the variable)
   * @param vm The virtual machine
   */
  inline
  void wakeUpPendings(VM vm);

  /**
   * Wakes up the pendings of the variable that are suspended under the provided space
   * @param vm The virtual machine
   * @param currentSpace The space under which the pendings to wake up are suspended
   * @note The general idea here is to wake up things whose home space is the current
   * space or any of its children, but not the others
   */
  inline
  void wakeUpPendingsSubSpace(VM vm, Space* currentSpace);

  VMAllocatedList<StableNode*> pendings;

  /* TODO maybe we can squeeze this bit of information into pendings
   * Idea: a leading `nullptr` element in pendings? */
  bool _needed;
};

//////////////
// Variable //
//////////////

#ifndef MOZART_GENERATOR
#include "Variable-implem-decl.hh"
#endif

class Variable: public DataType<Variable>, public VariableBase<Variable>,
  Transient, WithVariableBehavior<90> {
public:
  /**
   * Instantiates a new variable
   * @param vm The virtual machine in which the variable is being created
   * @note The variable is not needed by default
   */
  explicit Variable(VM vm): VariableBase(vm) {}

  /**
   * Instantiates a new variable
   * @param vm The virtual machine in which the variable is being created
   * @param home The home space of the variable
   * @note The variable is not needed by default
   */
  Variable(VM vm, Space* home): VariableBase(vm, home) {}

  /**
   * Instantiates a new variable with copies of the provided variable's pendings
   * @param vm The virtual machine in which the variable is being created
   * @param gr The graph replicator used to copy
   * @param from The variable to copy
   * @note The variable is not needed by default
   */
  inline
  Variable(VM vm, GR gr, Variable& from);

public:
  // Wakeable interface

  /**
   * Binds the provided self variable with a unit value
   * @param self The variable to bind
   * @param vm The virtual machine
   */
  inline
  void wakeUp(RichNode self, VM vm);

  /**
   * Tells if the the variable's home space should be woken up depending on
   * if the provided space is an ancestor
   * @param vm The virtual machine
   * @param space The potential ancestor space
   * @returns if the variable should be woken up
   */
  inline
  bool shouldWakeUpUnderSpace(VM vm, Space* space);

public:
  // DataflowVariable interface

  /**
   * Binds the provided self variable with the source variable
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   * @note src can be triggered if it is lazyly generated
   */
  inline
  void bind(RichNode self, VM vm, RichNode src);
public:
  // Miscellaneous

  /**
   * Prints a representation of the variable to the provided output stream
   * @param vm The virtual machine
   * @param out The output stream to print to
   * @param depth The depth of the representation
   * @param width The width of the representation
   */
  void printReprToStream(VM vm, std::ostream& out, int depth, int width) {
    out << "_";
  }
};

#ifndef MOZART_GENERATOR
#include "Variable-implem-decl-after.hh"
#endif

//////////////////////
// ReadOnlyVariable //
//////////////////////

#ifndef MOZART_GENERATOR
#include "ReadOnlyVariable-implem-decl.hh"
#endif

class ReadOnlyVariable: public DataType<ReadOnlyVariable>,
  public VariableBase<ReadOnlyVariable>,
  Transient, WithVariableBehavior<80> {
public:
  /**
   * Instantiates a new read-only variable exactly like a variable base
   * @param vm The virtual machine in which the variable is being created
   * @note The variable is not needed by default
   */
  explicit ReadOnlyVariable(VM vm): VariableBase(vm) {}

  /**
   * Instantiates a new variable exactly like a variable base
   * @param vm The virtual machine in which the variable is being created
   * @param home The home space of the variable
   * @note The variable is not needed by default
   */
  ReadOnlyVariable(VM vm, Space* home): VariableBase(vm, home) {}

  /**
   * Instantiates a new variable with copies of the provided variable's pendings
   * exactly like a variable base
   * @param vm The virtual machine in which the variable is being created
   * @param gr The graph replicator used to copy
   * @param from The variable to copy
   * @note The variable is not needed by default
   */
  inline
  ReadOnlyVariable(VM vm, GR gr, ReadOnlyVariable& from);

public:
  // DataflowVariable interface

  /**
   * Tries to bind the provided self variable with the source variable
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   * @note Triggers an exception as we cannot bind read-only variables as normal ones
   */
  inline
  void bind(RichNode self, VM vm, RichNode src);

public:
  // BindableReadOnly interface

  /**
   * Binds the provided self variable with the source variable where both are rich nodes
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   * @note src can be triggered if it is lazyly generated
   */
  inline
  void bindReadOnly(RichNode self, VM vm, RichNode src);

public:
  // Miscellaneous

  /**
   * Prints a representation of the variable to the provided output stream
   * @param vm The virtual machine
   * @param out The output stream to print to
   * @param depth The depth of the representation
   * @param width The width of the representation
   */
  void printReprToStream(VM vm, std::ostream& out, int depth, int width) {
    out << "!!_";
  }
};

#ifndef MOZART_GENERATOR
#include "ReadOnlyVariable-implem-decl-after.hh"
#endif

////////////
// OptVar //
////////////

#ifndef MOZART_GENERATOR
#include "OptVar-implem-decl.hh"
#endif

class OptVar: public DataType<OptVar>, public WithHome,
  Transient, StoredAs<SpaceRef>, WithVariableBehavior<100> {
public:
  /**
   * Instantiates a new OPT variable from a space reference
   * @param home The home space reference of the variable
   */
  explicit OptVar(SpaceRef home): WithHome(home) {}

  /**
   * Attributes to the provided referenced space reference VM's current space
   * @param self The variable to create
   * @param vm The virtual machine
   */
  static void create(SpaceRef& self, VM vm) {
    self = vm->getCurrentSpace();
  }

  /**
   * Attributes to the provided referenced space reference VM's current space
   * @param self The variable to create
   * @param vm The virtual machine
   * @param home The home space pointer to reference
   */
  static void create(SpaceRef& self, VM vm, Space* home) {
    self = home;
  }

  /**
   * Using the provided graph replicator, makes a copy of the provided variable's space
   * and references it into the provided referenced space reference
   * @param self The variable to create
   * @param vm The virtual machine
   * @param gr The graph replicator used to copy from's home space
   * @param from The variable to copy the space from
   */
  inline
  static void create(SpaceRef& self, VM vm, GR gr, OptVar from);

public:
  // DataflowVariable interface

  /**
   * Makes the provided rich node becoming an unbound variable then adds the provided
   * variable to the suspension list
   * @param self The rich node to become a variable
   * @param vm The virtual machine
   * @param variable The variable to add to the suspend list
   */
  inline
  void addToSuspendList(RichNode self, VM vm, RichNode variable);

  /**
   * Tells if the variable is needed
   * @param vm The virtual machine
   * @returns If the variable is needed
   */
  bool isNeeded(VM vm) {
    return false;
  }

  /**
   * Marks the provided variable as needed
   * @param self The rich node to become a variable
   * @param vm The virtual machine
   */
  inline
  void markNeeded(RichNode self, VM vm);

  /**
   * Binds the provided variable using the provided raw value (rvalue)
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The raw value to bind with
   */
  inline
  void bind(RichNode self, VM vm, UnstableNode&& src);

  /**
   * Binds the provided variable using the provided source variable
   * @param self The variable to bind
   * @param vm The virtual machine
   * @param src The source variable to bind with
   */
  inline
  void bind(RichNode self, VM vm, RichNode src);

private:
  /**
   * Makes a backup of the provided variable for speculative binding only iif
   * the VM's current space is not on top level
   * @param self The variable to make a backup for
   * @param vm The virtual machine
   */
  inline
  void makeBackupForSpeculativeBindingIfNeeded(RichNode self, VM vm);

public:
  // Miscellaneous

  /**
   * Prints the representation of the variable to the provided stream
   * @param vm The virtual machine
   * @param out The output stream
   * @param depth The depth of the representation
   * @param width The width of the representation
   */
  void printReprToStream(VM vm, std::ostream& out, int depth, int width) {
    out << "_<optimized>";
  }
};

#ifndef MOZART_GENERATOR
#include "OptVar-implem-decl-after.hh"
#endif

//////////////
// ReadOnly //
//////////////

#ifndef MOZART_GENERATOR
#include "ReadOnly-implem-decl.hh"
#endif

class ReadOnly: public DataType<ReadOnly>, Transient, StoredAs<StableNode*>,
  WithVariableBehavior<80> {
public:
  /**
   * Instantiates a read-only value from a stable node pointer
   * @param underlying The stable node pointer to store
   */
  explicit ReadOnly(StableNode* underlying): _underlying(underlying) {}

  /**
   * Makes the reference stable node pointer points to the stored pointed stable node
   * @param self The variable to create
   * @param vm The virtual machine
   * @param underlying The stable node pointer to reference
   */
  static void create(StableNode*& self, VM vm, StableNode* underlying) {
    self = underlying;
  }

  /**
   * Using the provided graph replicator, makes a copy of the provided referenced stable
   * node and references it into the provided reference stable node pointer
   * @param self The stable node pointer
   * @param vm The virtual machine
   * @param gr The graph replicator
   * @param from The read-only to copy the referenced stable node from
   */
  inline
  static void create(StableNode*& self, VM vm, GR gr, ReadOnly from);

public:
  /**
   * If the read-only is a variable, makes the provided stable node reference as
   * a new read-only pointing on this variable and added to its suspension list,
   * otherwise makes it as a copy of the pointer stable node
   * @param dest The stable node reference
   * @param vm The virtual machine
   * @param underlying The value (potentially a variable) to reference or copy
   */
  inline
  static void newReadOnly(StableNode& dest, VM vm, RichNode underlying);

  /**
   * If the read-only is a variable, makes a new read-only pointing on the provided
   * underlying potentially a variable
   * @param vm The virtual machine
   * @param underlying The value (potentially a variable) to reference
   */
  inline
  static UnstableNode newReadOnly(VM vm, RichNode underlying);

private:
  /**
   * Tells if the provided rich node is a variable and so needs protection
   * @param vm The virtual machine
   * @param underlying The rich node to check
   * @returns If the provided rich node is a variable
   */
  inline
  static bool needsProtection(VM vm, RichNode underlying);

public:
  /** @returns The underlying stable node pointer */
  StableNode* getUnderlying() {
    return _underlying;
  }

public:
  // Wakeable interface

  /**
   * Wakes up the provided variable by assigning the value of the underlying
   * or adds it to the suspension list of the underlying it this one is a variable
   * @param self The variable to wake up
   * @param vm The virtual machine
   */
  inline
  void wakeUp(RichNode self, VM vm);

  /**
   * Tells if the underspace of the provided space should be woken up
   * @param vm The virtual machine
   * @param space The potential ancestor space
   * @returns If the underspace of the provided space should be woken up
   * @note Always true
   */
  inline
  bool shouldWakeUpUnderSpace(VM vm, Space* space);

public:
  // DataflowVariable interface

  /**
   * Adds the provided variable to the suspension list of the read-only
   * @param vm The virtual machine
   * @param variable The variable to add to the suspension list
   */
  inline
  void addToSuspendList(VM vm, RichNode variable);

  /**
   * Tells if the read-only is needed
   * @param vm The virtual machine
   * @returns If the read-only is needed
   */
  inline
  bool isNeeded(VM vm);

  /**
   * Makes the read-only as needed
   * @param vm The virtual machine
   */
  inline
  void markNeeded(VM vm);

  /**
   * Tries to bind the read-only value with the provided source variable or value
   * @param vm The virtual machine
   * @param src The source variable or value to bind with
   * @note Triggers an exception as we cannot bind read-only variables as normal ones
   */
  inline
  void bind(VM vm, RichNode src);

public:
  // Miscellaneous

  /**
   * Prints the representation of the read-only value to the provided stream
   * @param vm The virtual machine
   * @param out The output stream
   * @param depth The depth of the representation
   * @param width The width of the representation
   */
  void printReprToStream(VM vm, std::ostream& out, int depth, int width) {
    out << "!!" << repr(vm, *_underlying, depth, width);
  }

private:
  StableNode* _underlying;
};

#ifndef MOZART_GENERATOR
#include "ReadOnly-implem-decl-after.hh"
#endif

/////////////////
// FailedValue //
/////////////////

#ifndef MOZART_GENERATOR
#include "FailedValue-implem-decl.hh"
#endif

class FailedValue: public DataType<FailedValue>,
  Transient, StoredAs<StableNode*>, WithVariableBehavior<10> {
public:
  /**
   * Instantiates a failed value from a stable node pointer
   * @param underlying The stable node pointer to store
   */
  explicit FailedValue(StableNode* underlying): _underlying(underlying) {}

  /**
   * Makes the reference stable node pointer points to the stored pointed stable node
   * @param self The variable to create
   * @param vm The virtual machine
   * @param underlying The stable node pointer to reference
   */
  static void create(StableNode*& self, VM vm, StableNode* underlying) {
    self = underlying;
  }

  /**
   * Using the provided graph replicator, makes a copy of the provided referenced stable
   * node and references it into the provided reference stable node pointer
   * @param self The stable node pointer
   * @param vm The virtual machine
   * @param gr The graph replicator
   * @param from The read-only to copy the referenced stable node from
   */
  inline
  static void create(StableNode*& self, VM vm, GR gr, FailedValue from);

public:
  /** @returns The underlying stable node pointer */
  StableNode* getUnderlying() {
    return _underlying;
  }

  /**
   * Raises the failed value as an exception
   * @param vm The virtual machine
   */
  inline
  void raiseUnderlying(VM vm);

public:
  // DataflowVariable interface

  /**
   * Adds the provided variable to the suspension list of the failed value
   * @param vm The virtual machine
   * @param variable The variable to add to the suspension list
   */
  inline
  void addToSuspendList(VM vm, RichNode variable);

  /**
   * Tells if the failed value is needed
   * @param vm The virtual machine
   * @returns If the failed value is needed
   */
  inline
  bool isNeeded(VM vm);

  /**
   * Makes the read-only as needed
   * @param vm The virtual machine
   */
  inline
  void markNeeded(VM vm);

  /**
   * Tries to bind the failed-value value with the provided source variable or value
   * @param vm The virtual machine
   * @param src The source variable or value to bind with
   * @note Actually it triggers the raise of the failed value
   */
  inline
  void bind(VM vm, RichNode src);

public:
  // Miscellaneous

  /**
   * Prints the representation of the failed value to the provided stream
   * @param vm The virtual machine
   * @param out The output stream
   * @param depth The depth of the representation
   * @param width The width of the representation
   */
  void printReprToStream(VM vm, std::ostream& out, int depth, int width) {
    out << "<Failed " << repr(vm, *_underlying, depth, width) << ">";
  }

private:
  StableNode* _underlying;
};

#ifndef MOZART_GENERATOR
#include "FailedValue-implem-decl-after.hh"
#endif

}

#endif // MOZART_VARIABLES_DECL_H
