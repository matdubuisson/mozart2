%%%
%%% Authors:
%%%   Mattéo Dubuisson
%%%
%%% Contributors:
%%%   
%%%
%%% Copyright:
%%%   
%%%
%%% Last change:
%%%   $Date$ by $Author$
%%%   $Revision$
%%%
%%% This file is part of Mozart, an implementation
%%% of Oz 3
%%%    http://www.mozart-oz.org
%%%
%%% See the file "LICENSE" or
%%%    http://www.mozart-oz.org/LICENSE.html
%%% for information on usage and redistribution
%%% of this file, and for a DISCLAIMER OF ALL
%%% WARRANTIES.
%%%

functor

import
  Boot        at 'x-oz://boot/Boot'
define
  Boot_Thread = {Boot.getInternal 'Thread'}
  Boot_System = {Boot.getInternal 'System'}
  Boot_Introspection = {Boot.getInternal 'Introspection'}
  Boot_VirtualMachine = {Boot.getInternal 'VirtualMachine'}

  proc {GetVMStatus ?Status}
    Status = stats(
      threads: threads(
        active: {Boot_Introspection.getActiveThreadsCount $}
        passive: {Boot_Introspection.getPassiveThreadsCount $}
        total: {Boot_Introspection.getThreadsCount $}
      )
      variables: variables(
        bound: {Boot_Introspection.getBoundVariablesCount $}
        unbound: {Boot_Introspection.getUnBoundVariablesCount $}
        total: {Boot_Introspection.getVariablesCount $}
      )
    )
  end

  proc {GetThreadStatus Thread ?Status}
    Status = 'thread'(
      id: {Boot_Thread.getId Thread $}
      priority: {Boot_Thread.getPriority Thread $}
      preemptible: {Boot_Thread.isPreemptible Thread $}
    )
  end

  MARKER = "[DEBUGGER]"

  proc {Loop
    NonPreemptible
    NContinues}

    This = {Boot_Thread.this $}
    proc {DefaultLoop}
      {Loop
        NonPreemptible
        NContinues}
    end
  in
    if NContinues == 0 then
      Status
      Command
    in
      Status = {GetThreadStatus This $}
      
      /*
        It ensures the debugger will not be preempted during its analysis
        and so risking to produce an inconsistent result. However it is
        responsible to release the VM often to let other threads
        enough running time
      */
      if Status.preemptible andthen NonPreemptible then
        {Boot_Thread.setPreemptible This false}
      end
      
      % Shows the next running thread along with the next byte code instruction
      local
        NextThread = {Boot_Introspection.getNextScheduledThread false $}
      in
        {Boot_System.printVS MARKER#": "#
          "Next scheduled thread is: "#
          {Int.toString {Boot_Thread.getId NextThread $} $}
        false true}
      end

      % Shows the debugger's thread status
      {Boot_System.printVS MARKER#
        "(id: "#{Int.toString Status.id $}#
        ", "#
        "priority: "#{Atom.toString Status.priority $}#
        ", "#
        "preemptible: "#{Bool.toString Status.preemptible $}#
        "): "
        false false}

      /*
        Terminal commands management and execution of them
      */
      Command = {Boot_System.inputVS $}

      %%%%%%%% help
      case Command of "help" then
        {Boot_System.printVS "Useful help doc will be here" false true}
        {DefaultLoop}

      [] "count" then
        Arguments = {Boot_System.inputVSLine $}
        SplitArguments = {String.tokens Arguments 32 $}.2 % "First argument is ' '"
      in
        {Boot_System.printRepr SplitArguments false true}
        case SplitArguments of Aggregate|Options then
          case Aggregate of "threads" then
            case Options of nil then
              {Boot_System.printVS
                "\t=>Active threads count: "#
                {Int.toString
                  {Boot_Introspection.getActiveThreadsCount $} $
                }#
                "\n\t=>Passive threads count: "#
                {Int.toString
                  {Boot_Introspection.getPassiveThreadsCount $} $
                }#
                "\n\t=>Threads count: "#
                {Int.toString
                  {Boot_Introspection.getThreadsCount $} $
                }
                false true}
            [] ["active"] then
              {Boot_System.printVS "\t=>Active threads count: "#
                {Int.toString
                  {Boot_Introspection.getActiveThreadsCount $} $
                }
                false true}
            [] ["passive"] then
              {Boot_System.printVS "\t=>Passive threads count: "#
                {Int.toString
                  {Boot_Introspection.getPassiveThreadsCount $} $
                }
                false true}
            [] ["all"] then
              {Boot_System.printVS "\t=>Threads count: "#
                {Int.toString
                  {Boot_Introspection.getThreadsCount $} $
                }
                false true}
            else
              {Boot_System.printVS MARKER#": invalid options for aggregate '"#Aggregate#"'" true true}
            end
          else
            {Boot_System.printVS MARKER#": invalid aggregate '"#Aggregate#"'" true true}
          end
        end

        {DefaultLoop}

      %%%%%%%% preemptible
      [] "preemptible" then
        Activated = {Boot_System.inputVS $}
      in
        case Activated of "true" then
          {Loop false 0}
        [] "false" then
          {Loop true 0}
        else
          {Boot_System.printVS MARKER#": preemptible command takes one boolean argument true or false" true true}
          {DefaultLoop}
        end
        
      %%%%%%%% status
      [] "status" then
        Status = {GetVMStatus $}
      in
        {Boot_System.printVS "VM status:\n"#
          "\tThreads:\n"#
          "\t\t- "#Status.threads.active#" active\n"#
          "\t\t- "#Status.threads.passive#" passive\n"#
          "\t\t- "#Status.threads.total#" in total\n"#
          "\tVariables:\n"#
          "\t\t- "#Status.variables.bound#" bound\n"#
          "\t\t- "#Status.variables.unbound#" unbound\n"#
          "\t\t- "#Status.variables.total#" in total\n"
          false true}

        {DefaultLoop}
      
      %%%%%%%% continue
      [] "continue" then
        Argument = {Boot_System.inputVS $}
      in
        try
          NextNContinue = {String.toInt Argument $}
        in
          {Boot_System.printVS "Continue: "#NextNContinue false true}
          {Boot_Thread.setPreemptible This true}
          {Loop
            NonPreemptible
            NextNContinue}
        catch _ then
          {Boot_System.printVS MARKER#": continue takes one integer argument" true true}
          {DefaultLoop}
        end

      %%%%%%%% continue
      [] "noperations" then
        Argument = {Boot_System.inputVS $}
      in
        try
          NOperations = {String.toInt Argument $}
        in
          {Boot_System.printVS "NOperations: "#NOperations false true}
          {Boot_VirtualMachine.setNOperationsWithoutSystemThreads NOperations}
          {Boot_Thread.requestPreemption This}
          {DefaultLoop}
        catch _ then
          {Boot_System.printVS MARKER#": noperations takes one integer argument" true true}
          {DefaultLoop}
        end

      %%%%%%%% unknown command
      else
        {Boot_System.printVS MARKER#": unknown command '"#Command#"', try help to get more infos" true true}
        {DefaultLoop}
      end
    else
      /*
        As every system threads run once before running only one other thread,
        to let execute N threads before running the debugger with have to
        request preemption N times. Once the counter comes back to 0 it means
        N thread have been scheduled N times however the number of executed
        instructions is not determined with this action
      */
      {Boot_Thread.requestPreemption This}
      {Loop NonPreemptible NContinues - 1}
    end
  end
in
  {Loop
    true
    0}
end
