%%%
%%% Authors:
%%%   
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
        total: {Boot_Introspection.getTotalThreadsCount $}
      )
      variables: variables(
        bound: {Boot_Introspection.getBoundVariablesCount $}
        unbound: {Boot_Introspection.getUnBoundVariablesCount $}
        total: {Boot_Introspection.getTotalVariablesCount $}
      )
    )
  end

  proc {GetThreadStatus Thread ?Status}
    Status = 'thread'(
      id: {Boot_Thread.getID Thread $}
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
      
      if Status.preemptible andthen NonPreemptible then
        {Boot_Thread.setPreemptible This false}
      end
      
      local
        NextThread = {Boot_Introspection.getNextScheduledThread false $}
      in
        {Boot_System.printVS MARKER#": "#
          "Next scheduled thread is: "#
          {Int.toString {Boot_Thread.getID NextThread $} $}
        false true}
      end

      {Boot_System.printVS MARKER#
        "(id: "#{Int.toString Status.id $}#
        ", "#
        "priority: "#{Atom.toString Status.priority $}#
        ", "#
        "preemptible: "#{Bool.toString Status.preemptible $}#
        "): "
        false false}

      Command = {Boot_System.inputVS $}

      %%%%%%%% help
      case Command of "help" then
        {Boot_System.printVS "Useful help doc will be here" false true}
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
      {Boot_Thread.requestPreemption This}
      {Loop NonPreemptible NContinues - 1}
    end
  end
in
  {Loop
    true
    0}
end
