# Debugger's commands

## Terminal interface

### help

Display the use manual of the specified command.

```
$ help <command>
```

Other forms are normally available.

```
$ command help
% or
$ command <some-args> help
```

### About everything

#### count

There are four things to study :
- vm: counters related to the Virtual Machine.
- threads: counters related specifically to threads.
- variables: counters related specifically to variables.
- nodes: counters related specifically to nodes.

Each option has its own manual describing which aggregates can be displayed.
Enter one or several names to display counter(s) or enter 'all' to display them all.

```
$ count vm <aggregates>
$ count threads <aggregates>
$ count variables <aggregates>
$ count nodes <aggregates>
$ count help
```

For each option you can display one or several or get the manual.

```
$ count <option> <aggregate>
$ count <option> <aggregate0> .... <aggregateN>
$ count <option> all
$ count <option> help
```

### About runnables/threads

#### thread

Display a full description for the states of the specified threads.

```
$ thread <id>
$ thread <id0> .... <idN>
$ thread help
```

The thread state includes several information :
  - Id: this is a unique id describing the thread.
  - KindId: this is a group id linking several threads to a same family ([see here](#kindid-and-generationid)).
  - GenerationId: indicating the nth member of the group ([see here](#kindid-and-generationid)).
  - Runnable: indicating if the thread is runnable (not dead, not terminated and not suspended).
  - Terminated: indicating if the thread is terminated.
  - Dead: indicating if the thread is dead (killed, disposed or terminated).
  - Preempted: indicating if the thread is preempted ([see here](#preempted-flag)).
  - Preemptible: indicating if the thread is preemptible but the scheduler or not ([see here](#non-preemptible-runnablesthreads))
  - Type: indicating which thread is it. In generally it will be thread pointing normal user threads out however there are special other lighter runnables used for specific operations.
  - Priority: indication the thread priority either low, medium, high or system.

#### threads

Display for each filtered thread, a compact description of its state or an aggregate about its activity.

Three options are available :
- state: a compact thread state description.
- statistics: counters about the thread activity as the number if operations, schedules, binds, runs, suspensions and variable suspensions.
- nodes: counters telling how many of each node type has the thread.

```
$ threads help
$ threads state
$ threads statistics
$ threads nodes
```

Optionally filtering parameters may be added to the query [see here](#filtering-arguments).

```
$ threads <option> <filtering-arguments>
```

### About variables

#### variable

Display the full description of one or several variables specified by their id.

```
$ variable <id>
$ variable <id0> .... <idN>
$ variable help
```

The variable state is composed of several things :
  - Id: this is a unique id describing the thread.
  - KindId: this is a group id linking several threads to a same family ([see here](#kindid-and-generationid)).
  - GenerationId: indicating the nth member of the group ([see here](#kindid-and-generationid)).
  - Bound: indicating if the variable has been assigned to a value.
  - Needed: indicating if the variable is needed (might be interesting in the context of lazy functions).
  - Type: the type of variable Variable or ReadOnlyVariable (stream-port related but same idea).
  - Value: a string representation of the variable so of its value.
  - Candidates: ids list of the threads having a reference on this variable.
  - Pendings: ids list of any threads, variables or other objects suspended/waiting this variable.

The interesting thing about Mozart variables is that they do not directly carry a value but they are suspension lists acting like a barrier to wait an access on a data.

#### variables

Display for each filtered variable, a compact description of its state or an aggregate about its activity.

There are four options are available :
- state: display compact description of the variable states.
- pendings: display the thread ids suspended on the variables.
- candidates: display the thread ids referencing the variables.
- value: display the variables' representations to help the user to identify which one is responsible for.

```
$ variables state
$ variables pendings
$ variables candidates
$ variables value
$ variables help
```

Optionally filtering parameters may be added to the query [see here](#filtering-arguments).

```
$ variables <option> <filtering-arguments>
```

### About nodes and registers

#### nodes

The nodes command allows to display a compact description for each node.

There are four families of nodes : variable, structural, token and value.

```
$ nodes variable
$ nodes structural
$ nodes token
$ nodes value
$ nodes all
$ nodes help
```

Optionally filtering parameters may be added to the query [see here](#filtering-arguments).

```
$ nodes <option> <filtering-arguments>
```

A node state is composed of several components :
- Name: the type name of the carried data.
- BindingPriority: the binding priority.
- StructuralBehavior: the node family so either variable, structural, token or value.
- Copyable: indicating if the carried value is copyable.
- Transient: indicating if the carried value is transient (modifiable or assignable).
- Feature: indicating if the carried value is a feature.
- UUID: a unique identifier related to the data.
- Value: a representation of the carried data.

#### register

Display the whole content of a register by showing one by one a small description of its nodes. A register is located inside a thread, has a certain type and might be located at a certain depth (if type is x then depth must be 0).

At first there are four options for the register type :
- x: contains the currently used nodes. Every thread has only one of this so there is no depth to find a x register.
- y: is a backup of a previous x register.
- g: contains nodes carrying constant values.
- k: contains nodes carrying constant values.

```
$ register x <id>
$ register y <id> <depth>
$ register g <id> <depth>
$ register k <id> <depth>
$ register help
```

#### registers

Display aggregates about nodes proportions for each specified thread id. Each line is a brief description of a register and its proportion versus the thread and VM total memory.

```
$ registers <id>
$ registers <id0> .... <idN>
$ registers help
```

#### depth

Display the stack depth for each specified thread.

```
$ depth <id>
$ depth <id0> .... <idN>
$ depth help
```

### About procedures

#### procedure

Display the source code or bytecode of a certain Mozart procedure specified by its name.
A procedure is located into a certain OZF file at a certain line.

```
$ procedure <name>
$ procedure <name> <module> % Module = OZF file name
$ procedure help
```

**Not implemented yet.**

#### procedures

Display the list of the different procedures.

```
$ procedures <module>
$ procedures % For all modules
$ procedures help
```

**Not implemented yet.**

### About VM exploration

#### status

Display the current VM status to the user. No arguments for this command.

### VM control

#### gc

Enable or disable the garbage collector. It might be useful for debugging some programs where we want to observe data out-of-reach longer.

```
$ gc on
$ gc off
$ gc help
```

### Alarms, breakpoints and flow control

#### breakpoint

A breakpoint is a predefined stop at a specific location or moment in the program execution.

There are two options :
- schedule: stop at a specific thread schedule before starting the thread.
- procedure: stop at the beginning of a Mozart procedure.
- operation: stop at a specific bytecode instruction.

```
$ breakpoint schedule <id>
$ breakpoint procedure <name>
$ breakpoint procedure <name> <module>
$ breakpoint operation <address> % Address still not defined
```

**Not implemented yet.**

#### alarm

An alarm is similar to a breakpoint but is still different. Whereas the breakpoint targets a specific thing already known in advance, the alarm targets an event that might potentially happen at any time.

The creation of a new alarm is a little complex and is shaped as below.
```
$ alarm <mode> <family> <event> <filtering-parameters>
$ alarm help
$ alarm <mode> help
$ alarm <mode> <family> help
$ alarm <mode> <family> <event> help
```

An alarm can be either active or passive : both will print an alert message when it is raised but the active alarm in addition will stop the user program execution. There are three families threads, variables and structures, and each one has a dedicated manual about how to filter matching events using state attributes. Also an alarm is based on a certain event variating according to the family.

Four threads events :
- inserted: new runnable was created.
- updated: runnable state changed.
- removed: runnable was removed from the schedule.
- collected: % runnable was garbage collected.

Five variables events :
- created: new created variable.
- needed: variable became needed.
- bound: variable was assigned to a value and has released its suspension list.
- collected: variable was garbage collected.
- waited: variable has been waited by another object (generally thread or variable).

Two structures events :
- created: new created structure.
- collected: structure was garbage collected.

#### continue

Continues the execution until reaching a new alarm or breakpoint.

#### reset

Resets the schedule at its normal execution mode.

#### run

This is the most complex command of the debugger and it gathers the most important functionalities.

There are 5 execution modes :
- schedules: executes n thread schedules before the next interruption and one schedule executes an arbitrary number of operations.
- operations: executes exactly n operations before the next interruption. One or several threads will execute during this mode.
- operationByOperation (alias OBO): executes one bytecode instruction at a time a always display it to the user before executing. This mode is very very slow but it allows a deep instruction control.
- stepByStep (alias SBS): executes step by step the program on a some specified data structures. The program might execute until one of the structures has a single change. Very relevant for streams debugging.
- until: executes the program until a breakpoint is reached or an alarm is raised.

```
$ run schedules <number>
$ run operations <number>
$ run OBO
$ run SBS <id> % Generally a stream id
$ run SBS <id0> .... <idN>
$ run until
$ run help
```

**Still not finished**

## Lexical

### KindId and GenerationId

The kind id is a big word used as group id and works with the generation id being the nth member of this group. Depending on the context it can have several meanings :
- Runnables/Threads: used for lazy functions, the kind id correlates all threads generated by the same lazy function together and the generation id is used to know at which iteration we are currently.
- Variables/Cons: the kind id is the stream id and the generation id is the nth element of this stream. It allows the debugger to target some interesting streams even we lost the beginning of it (as for instance it has been garbage collected). The generation id increments from cons to cons however the variables are just transmitters that do not generate a new generations.

### Filtering arguments

These parameters are only available for commands displaying a lot of information line by line and organized in columns (like a CSV file) and they limit/filter the number of displayed information. Normally these parameters are located on the right side of the whole command in order to ease the job of the interpreter. Each row has several attributes that may be used for filtering and they have additionally an index automatically attributed that may be used also.

Several filtering arguments are available and they take specific parameters :
- `from <index>`: display from the specified index included.
- `to <index>`: display to the specified index excluded.
- `Attribute == Something`: filter all rows having the specified attribute equals to the given value.
- `Attribute != Something`: same principle but check if different.
- `Attribute > Something`: same principle but check if greater.
- `Attribute < Something`: same principle but check if lower.
- `Attribute >= Something`: same principle but check if greater or equal.
- `Attribute <= Something`: same principle but check if lower or equal.
- `Attribute has <value>`: checks if the attribute is a list and contains the specified element.
- `(condition)`: put a condition in parenthesis.
- `condition0 and condition1`: filter using the conjunction of both conditions.
- `condition0 or condition1`: filter using the disjunction of both conditions.
- `condition0 xor condition1`: filter using the exclusive disjunction of both conditions.

### Preempted flag

It might seem stupid to give the preempted status of a thread as it will be always false for any-threads except the one executing. Thus in our case, when the debugger will run it will be always false for any thread except thread 0 being the non-preemptible system thread dedicated to the debugger.

Actually it is still part of the states and in the future if Mozart became multi-core or user threads would be allowed to run at the same time of the debugger it could make sense. In more, it might be used in rules to stop the program execution at a certain point as for example as soon as a certain thread is run.

### Non-preemptible runnables/threads

A non-preemptible is a thread that decides itself when her execution time is finished using a call like `{Thread.preempt {Thread.this $}}`. Thus the scheduler cannot decide to make a preemption when it is runnable. Nevertheless the thread is still preempted if it becomes to be terminated or dead (end of program or crash) or even if it is suspended (explicitly or on dataflow suspension) making this mode safe (cannot block the whole schedule). 

Non-preemptible threads are perfect fit to allow threads to make jobs that cannot be interrupted as for example a debugger analysis. The reason is that between two schedules of the same thread, the VM state changed a little or a lot and it is unacceptable for the debugger. With it the debugger has the warranty that is analysis is not disturbed and is based on a single VM state from the same epoch.

An user using this mode for its threads should be very careful as another non-preemptible thread might prevent the debugger and even the whole system to work properly. In the future it should be forbidden to use this mode from a non-internal OZ program.
