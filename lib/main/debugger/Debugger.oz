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

  proc {GetThreadStatus Thread ?Status}
    Status = 'status'(
      id: {Boot_Introspection.getThreadInfo Thread id $}
      type: {Boot_Introspection.getThreadInfo Thread type $}
      runnable: {Boot_Introspection.getThreadInfo Thread runnable $}
      terminated: {Boot_Introspection.getThreadInfo Thread terminated $}
      dead: {Boot_Introspection.getThreadInfo Thread dead $}
      preempted: {Boot_Introspection.getThreadInfo Thread preempted $}
      preemptible: {Boot_Introspection.getThreadInfo Thread preemptible $}
      priority: {Boot_Introspection.getThreadInfo Thread priority $}
    )
  end

  TRYHELP = ", try help to get more details"

  % All printers to display runtime data, infos and errors
  PrintLog
  PrintInfo
  PrintWarning
  PrintError
  PrintOther
  \insert Printing
  
  proc {ExtractInput Type Argument DefaultValue ?Result}
    proc {Convert Type F Argument ?Value}
      try
        Value = {F Argument $}
      catch _ then
        Value = DefaultValue
      end
    end
  in
    case Type of atom then Result = {String.toAtom Argument $}
    [] string then Result = Argument
    [] bool then
      Result = {String.toAtom Argument $}
    [] int then
      Result = {Convert Type String.toInt Argument $}
    [] float then
      Result = {Convert Type String.toFloat Argument $}
    end
  end
  
  proc {Loop
    NonPreemptible
    NContinues}

    This = {Boot_Thread.this $}
    ThisId = {Boot_Thread.getId This $}

    proc {DefaultLoop}
      {Loop
        NonPreemptible
        NContinues}
    end

    proc {PrintOptions Aggregate Options IsError}
      Print = if IsError then PrintError else PrintLog end

      proc {PrintOptions Options}
        case Options of nil then skip
        [] Option|NextOptions then
          case Option of option(Name Description) then
            {Print "\t=> "#Name#": "#Description}
          [] option(Name Description _) then
            {Print "\t=> "#Name#": "#Description}
          else
            {Print "\t=> "#Option}
          end
          {PrintOptions NextOptions}
        end
      end
    in
      {Print Aggregate#" has "#{List.length Options $}#" options :"}
      {PrintOptions Options}
    end

    proc {ExecuteOptions Aggregate Options SelectedOptionNames}
      proc {ExecuteAllOptions Options}
        case Options of nil then skip
        [] Option|NextOptions then
          case Option of option(_ _ Execute) then {Execute} end
          {ExecuteAllOptions NextOptions}
        end
      end

      proc {SelectOption Options SelectedOptionName ?Result}
        case Options of nil then Result = none
        [] Option|NextOptions then
          case Option of option(Name _ _) then
            if Name == SelectedOptionName then
              Result = Option
            else
              {SelectOption NextOptions SelectedOptionName Result}
            end
          end
        end
      end

      proc {Loop SelectedOptionNames}
        case SelectedOptionNames of nil then skip
        [] SelectedOptionName|NextSelectedOptionNames then
          case SelectedOptionName of "help" then
            {PrintOptions Aggregate Options false}
          else
            Option = {SelectOption Options SelectedOptionName $}
          in
            case Option of none then
              {PrintError "invalid option '"#SelectedOptionName#"'"}
              {PrintOptions Aggregate Options false}
            [] option(_ _ Execute) then {Execute} end
          end

          {Loop NextSelectedOptionNames}
        end
      end
    in
      case SelectedOptionNames of nil then
        {ExecuteAllOptions Options}
      else
        {Loop SelectedOptionNames}
      end
    end

    proc {GetIntValueFromArgument Inputs Argument DefaultValue ?IntValue}
      case Inputs of nil then IntValue = DefaultValue
      [] Input|NextInputs then
        if Input == Argument then
          case NextInputs of nil then
            {PrintError "missing following integer value for argument '"#
              Argument#"'"}
            [] Value|_ then
              try
                IntValue = {String.toInt Value $}
              catch _ then
                IntValue = DefaultValue
                {PrintError "invalid following integer value '"#
                  Value#"' for argument '"#Argument#"'"}
              end
            end
        else
          {GetIntValueFromArgument
            NextInputs Argument DefaultValue IntValue}
        end
      end
    end
  in
    if NContinues == 0 then
      local
        Status = {GetThreadStatus This $}
      in
        % Shows the debugger's thread status
        % {Boot_System.printRepr Status false true}
        
        /*
          It ensures the debugger will not be preempted during its analysis
          and so risking to produce an inconsistent result. However it is
          responsible to release the VM often to let other threads
          enough running time
        */
        if Status.preemptible andthen NonPreemptible then
          {Boot_Thread.setPreemptible This false}
        end
      end
      
      % Shows the next running thread along with the next byte code instruction
      local
        NextThread = {Boot_Introspection.getNextScheduledThread false $}
      in
        {PrintLog
          "Next scheduled thread is: "#
          {Int.toString {Boot_Thread.getId NextThread $} $}}
      end

      /*
        Terminal commands management and execution of them
      */
      local
        Input = {Boot_System.inputVSLine $}
        Inputs = {String.tokens Input 32 $}
        Command|Arguments = Inputs

        proc {GetArgument Arguments TargetedArgument Type DefaultValue ?Result}
          case Arguments of nil then Result = DefaultValue
          [] Argument|NextArguments then
            if Argument == TargetedArgument then
              case Type of flag then
                Result = {Not DefaultValue}
              else
                case NextArguments of Value|_ then
                  try
                    case Type of bool then
                      Result = {String.toAtom Value $}
                    [] atom then
                      Result = {String.toAtom Value $}
                    [] string then
                      Result = Value
                    [] int then
                      Result = {String.toInt Value $}
                    [] float then
                      Result = {String.toFloat Value $}
                    else
                      Result = Value
                    end
                  catch _ then
                    Value = DefaultValue
                    {PrintError "Inconsistent '"#Type#"' value provided for argument '"#Argument#"'"}
                  end
                else
                  Value = DefaultValue
                  {PrintError "No value was provided for argument '"#Argument#"'"}
                end
              end
            else
              {GetArgument NextArguments TargetedArgument Type DefaultValue Result}
            end
          end
        end
      in



        
        case Command of "count" then
          \insert CountCommand
        [] "thread" then
          \insert ThreadCommand
        [] "threads" then
          \insert ThreadsCommand
        [] "register" then
          \insert RegisterCommand
        [] "registers" then
          \insert RegistersCommand
        else
          {PrintError "unknown command '"#Command#"', try help to get more infos"}
        end
      end

      {DefaultLoop}

      %   %%%%%%%% continue
      
      %   [] "continue" then
      %     Argument = {Boot_System.inputVS $}
      %   in
      %     try
      %       NextNContinue = {String.toInt Argument $}
      %     in
      %       {PrintLog "Continue: "#NextNContinue}
      %       {Boot_Thread.setPreemptible This true}
      %       {Loop NonPreemptible NextNContinue}
      %     catch _ then
      %       {PrintError "continue takes one integer argument"}
      %       {DefaultLoop}
      %     end

      %   %%%%%%%% continue
      %   [] "noperations" then
      %     Argument = {Boot_System.inputVS $}
      %   in
      %     try
      %       NOperations = {String.toInt Argument $}
      %     in
      %       {PrintLog "NOperations: "#NOperations}
      %       {Boot_VirtualMachine.setNOperationsWithoutSystemThreads NOperations}
      %       {Boot_Thread.requestPreemption This}
      %       {DefaultLoop}
      %     catch _ then
      %       {PrintError "noperations takes one integer argument"}
      %       {DefaultLoop}
      %     end

      %   %%%%%%%% unknown command
      % end
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
  {Loop true 0}
end
