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
  Boot_Time = {Boot.getInternal 'Time'}
  Boot_Introspection = {Boot.getInternal 'Introspection'}
  Boot_EventManager = {Boot.getInternal 'EventManager'}
  Boot_Scheduler = {Boot.getInternal 'Scheduler'}

  TAB = "\t"
  TRYHELP = ", try help to get more details"

  % All printers to display runtime data, infos and errors
  \insert Printing

  \insert Error

  \insert Displaying
  
  \insert Extract

  \insert Format

  \insert Util

  This = {Boot_Thread.this $}
  ThisId = {Boot_Thread.getId This $}

  ModeCell = {Cell.new true $}

  proc {ProcessCommand}
    /*
      It ensures the debugger will not be preempted during its analysis
      and so risking to produce an inconsistent result. However it is
      responsible to release the VM often to let other threads
      enough running time
    */    
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
        % [] "alarm" then
        %   \insert AlarmCommand
        [] "nodes" then
          \insert NodesCommand
        [] "lists" then
          \insert ListsCommand
        [] "gc" then
          \insert GCCommand
        else
          {PrintError "Unknown command '"#Command#"'"#TRYHELP}
        end
      end
    end
  end

  proc {Loop}
    NormalExecutionMode = ({Boot_Scheduler.getExecutionMode $} == normal)
    AlarmRaised = {Boot_EventManager.isTrackingTriggered $}
  in
    if {Boot_Scheduler.isGCReady $} then
      {PrintWarning "GC ready"}
    end

    if {Boot_Scheduler.isGCDone $} then
      {PrintWarning "GC done"}
    end

    if {Boot_Thread.isPreemptible This $} then
      {Boot_Thread.setPreemptible This false}
    end

    if NormalExecutionMode orelse AlarmRaised then
      {ProcessCommand}
    else
      {Boot_Thread.preempt This}
    end

    % if 
    %   {Boot_Scheduler.isGCReady $} == false
    %   andthen {Boot_Scheduler.isGCDone $} == false
    %   andthen {Boot_EventManager.isTrackingTriggered $} then

    %   local
    %     Variables = {Boot_Introspection.getAllVariables $}

    %     proc {FormatStateCase Variable ?Result}
    %       case Variable of variable(
    %         id: Id
    %         kindId: KindId
    %         generationId: GenerationId
    %         type: Type
    %         isBound: IsBound
    %         isNeeded: IsNeeded
    %         pendings: Pendings
    %         candidates: Candidates
    %         value: _
    %       ) then
    %         Result = [
    %           {Int.toString Id $}
    %           {Int.toString KindId $}
    %           {Int.toString GenerationId $}
    %           {Atom.toString Type $}
    %           {Bool.toString IsBound $}
    %           {Bool.toString IsNeeded $}
    %           {Int.toString
    %             {List.length Pendings $} $}
    %           {Int.toString
    %             {List.length Candidates $} $}
    %         ]
    %       end
    %     end

    %   in
    %     {DisplayCSV
    %       ["Id" "KindId" "GenerationId" "Type" "IsBound" "IsNeeded" "NPendings" "NCandidates"]
    %       Variables
    %       10
    %       FormatStateCase
    %     }
    %   end

    %   local
    %     % PingPong :
    %     % Lists = {Boot_Introspection.getLists [100000] $}
    %     Lists = {Boot_Introspection.getLists [11111111 22222222 33333333 44444444 55555555] $}
    %   in
    %     if Lists \= nil then
    %       {MaskedDisplayCSV
    %         ["Id" "KindId" "GenerationId" "Hash" "Owners" "List"]
    %         Lists 10 FormatList
    %         [true true true true false false]}
    %       {PrintWarning "Make a stop...."}
    %       {Boot_System.inputEnter}
    %     else {PrintWarning "No lists matching conditions found"} end
    %   end
    % end

    {Boot_Thread.preempt This}
    {Loop}
  end
in
  %{Boot_EventManager.track variable bound 123456 nil} % [123456 654321]
  %{Boot_EventManager.track variable bound 654321 nil}
  
  {Loop}
end



























