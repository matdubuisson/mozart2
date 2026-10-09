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
  Boot_Identity = {Boot.getInternal 'Identity'}
  Boot_Introspection = {Boot.getInternal 'Introspection'}
  Boot_EventManager = {Boot.getInternal 'EventManager'}
  Boot_Scheduler = {Boot.getInternal 'Scheduler'}

  TAB = "\t"
  TRYHELP = ", try help to get more details"

  % All printers to display runtime data, infos and errors
  \insert Utils

  \insert Printing

  \insert Formatting

  \insert Displaying

  \insert Extracting
  
  \insert Filtering

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
          \insert ./commands/introspection/CountCommand
        [] "vm" then
          \insert ./commands/introspection/VMCommand
        [] "this" then
          {PrintInfo "Debugger thread id: "#ThisId}
        [] "thread" then
          \insert ./commands/introspection/ThreadCommand
        [] "threads" then
          \insert ./commands/introspection/ThreadsCommand
        [] "depth" then
          \insert ./commands/introspection/DepthCommand
        [] "register" then
          \insert ./commands/introspection/RegisterCommand
        [] "registers" then
          \insert ./commands/introspection/RegistersCommand
        [] "variable" then
          \insert ./commands/introspection/VariableCommand
        [] "variables" then
          \insert ./commands/introspection/VariablesCommand
        [] "run" then
          \insert ./commands/execution/RunCommand
        [] "continue" then
          {Cell.assign ModeCell false}
          {Boot_Thread.preempt This}
        [] "reset" then
          {Boot_Scheduler.reset}
        % [] "alarm" then
        %   \insert AlarmCommand
        [] "nodes" then
          \insert ./commands/introspection/NodesCommand
        [] "lists" then
          \insert ./commands/introspection/ListsCommand
        [] "gc" then
          \insert ./commands/GCCommand
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
      CollectedThreadIds = {Boot_Introspection.getGarbageCollectedThreads $}
    in
      {PrintWarning "GC ready"}
      {Boot_System.printRepr CollectedThreadIds false true}
    end

    % if {Boot_Scheduler.isGCDone $} then
    %   {PrintWarning "GC done"}
    % end

    % if {Boot_Thread.isPreemptible This $} then
    %   {Boot_Thread.setPreemptible This false}
    % end

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
    %     Todos = {Boot_Introspection.getGarbageCollectorTodos $}
    %   in
    %     {Boot_System.printRepr Todos false true}
    %   end

    %   % local
    %   %   Variables = {Boot_Introspection.getAllVariables $}

    %   %   proc {FormatStateCase Variable ?Result}
    %   %     case Variable of variable(
    %   %       id: Id
    %   %       kindId: KindId
    %   %       generationId: GenerationId
    %   %       type: Type
    %   %       isBound: IsBound
    %   %       isNeeded: IsNeeded
    %   %       pendings: Pendings
    %   %       candidates: Candidates
    %   %       value: _
    %   %     ) then
    %   %       Result = [
    %   %         {Int.toString Id $}
    %   %         {Int.toString KindId $}
    %   %         {Int.toString GenerationId $}
    %   %         {Atom.toString Type $}
    %   %         {Bool.toString IsBound $}
    %   %         {Bool.toString IsNeeded $}
    %   %         {Int.toString
    %   %           {List.length Pendings $} $}
    %   %         {Int.toString
    %   %           {List.length Candidates $} $}
    %   %       ]
    %   %     end
    %   %   end

    %   % in
    %   %   {DisplayCSV
    %   %     ["Id" "KindId" "GenerationId" "Type" "IsBound" "IsNeeded" "NPendings" "NCandidates"]
    %   %     Variables
    %   %     10
    %   %     FormatStateCase
    %   %   }
    %   % end

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

  {Loop}
end














































































