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
  Boot_Journal = {Boot.getInternal 'Journal'}
  Boot_Scheduler = {Boot.getInternal 'Scheduler'}

  TRYHELP = ", try help to get more details"

  % All printers to display runtime data, infos and errors
  \insert Printing

  \insert Error

  \insert Displaying
  
  \insert Interface

  This = {Boot_Thread.this $}
  ThisId = {Boot_Thread.getId This $}

  ModeCell = {Cell.new true $}
 
  proc {UpdateState State AlarmsCell Alarms ?AlarmRaised ?NewState}
    Journal = {Boot_Journal.getJournal $}
  in
    case Journal of journal(
      runnables: runnablesJournal(
        inserted: InsertedRunnables
        removed: RemovedRunnables
        updated: UpdatedRunnables
        collected: CollectedRunnables
      )

      % variables: variablesJournal(
      %   optVariables: optVariablesJournal(
      %     created: CreatedOptVariables
      %     collected: CollectedOptVariables
      %     needed: NeededOptVariables
      %     bound: BoundOptVariables
      %     waited: WaitedOptVariables
      %   )
      %   variables: variablesJournal(
      %     created: CreatedVariables
      %     collected: CollectedVariables
      %     needed: NeededVariables
      %     bound: BoundVariables
      %     waited: WaitedVariables
      %   )
      %   readOnlyVariables: readOnlyVariables(
      %     created: CreatedReadOnlyVariables
      %     collected: CollectedReadOnlyVariables
      %     needed: NeededReadOnlyVariables
      %     bound: BoundReadOnlyVariables
      %     waited: WaitedReadOnlyVariables
      %   )
      % )

      variables: variablesJournal(
        optVariables: OptVariablesJournal
        variables: VariablesJournal
        readOnlyVariables: ReadOnlyVariablesJournal
      )

    ) then
      \insert Alarm
    in
      AlarmRaised = {ParseAlarms Alarms $}
    end

    % if AlarmRaised then
    %   {PrintWarning "Alarm raised"}
    % end

    NewState = State
  end

  proc {ProcessCommand AlarmsCell Alarms}
    /*
      It ensures the debugger will not be preempted during its analysis
      and so risking to produce an inconsistent result. However it is
      responsible to release the VM often to let other threads
      enough running time
    */
    if {Boot_Thread.isPreemptible This $} then
      {Boot_Thread.setPreemptible This false}
    end
    
    {PrintPrefix}
    
    local
      Input = {Boot_System.inputVSLine $}    
    in
      if Input \= "" then
        Inputs = {String.tokens Input 32 $}
        Command|Arguments = Inputs
      in
        case Command of "count" then
          \insert CountCommand
        [] "thread" then
          \insert ThreadCommand
        [] "threads" then
          \insert ThreadsCommand
        [] "depth" then
          \insert DepthCommand
        [] "register" then
          \insert RegisterCommand
        [] "registers" then
          \insert RegistersCommand
        [] "variable" then
          \insert VariableCommand
        [] "variables" then
          \insert VariablesCommand
        [] "status" then
          \insert StatusCommand
        [] "run" then
          \insert RunCommand
        [] "continue" then
          {Cell.assign ModeCell false}
          {Boot_Thread.preempt This}
        [] "reset" then
          {Boot_Scheduler.reset}
        [] "alarm" then
          \insert AlarmCommand
        else
          {PrintError "Unknown command '"#Command#"'"#TRYHELP}
        end
      end
    end
  end

  proc {Loop State AlarmsCell}
    Alarms = {Cell.access AlarmsCell $}
    IsNormalExecutionMode = ({Boot_Scheduler.getExecutionMode $} == normal)
    IsActiveMode = {Cell.access ModeCell $}

    AlarmRaised
    NewState
  in
    % if {Boot_Scheduler.isGCReady $} then
    %   {PrintWarning "Garbage collector is about to run"}
    % end

    % if {Boot_Scheduler.isGCDone $} then
    %   {PrintWarning "Garbage collector just ran"}
    % end

    {UpdateState State AlarmsCell Alarms AlarmRaised NewState}

    if AlarmRaised orelse (IsNormalExecutionMode andthen IsActiveMode) then
      {Cell.assign ModeCell true}
      {ProcessCommand AlarmsCell Alarms}
    else
      {Boot_Thread.preempt This}
    end

    {Loop NewState AlarmsCell}
  end
in
  {Loop state() {Cell.new nil $}}
end
