{DisplayFrame "Mozart debugger command descriptions" [
  "this" "display the debugger system thread id"
  
  "count" "display aggregated counters related to the vm, runnables, variables and nodes"
  "vm" "introspect the virtual machine state"
  "thread" "introspect a thread state"
  "threads" "introspect several threads in a CSV format"
  "variable" "introspect a variable state"
  "variables" "introspect several variables in a CSV format"

  "journal" "display the journal with the last generated events"
  "track" "add a new tracking rule"
  "untrack" "delete a predefined tracking rule"
  "tracks" "display all registered tracking rules"

  "run" "run the debugged program a fixed amount of time or until a certain event arises"
  "reset" "restore the execution mode of the virtual machine to normal (basically to stop a special run)"
  "continue" "continue until the next schedule or step (if run steps being used)"

  "gc" "enable or disable the garbage collector, must be used with care"
]}
