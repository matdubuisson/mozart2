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

  \insert Extracting
  
  \insert Displaying

  This = {Boot_Thread.this $}
  ThisId = {Boot_Thread.getId This $}

  ModeCell = {Cell.new true $}

  proc {ProcessCommand WatchedListIds ?NewWatchedListIds}
    proc {DefaultReturn}
      NewWatchedListIds = WatchedListIds
    end

    proc {ContinueReturn Arguments}
      \insert ./commands/execution/ContinueCommand
      {DefaultReturn}
    end
  in
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
      if Input == "" then % Considered as a continue
        {ContinueReturn nil}
      else
        Inputs = {String.tokens Input 32 $}
        Command|Arguments = Inputs
      in
        % Everything concerning the introspection of the VM
        case Command of "this" then
          {PrintInfo "Debugger thread id: "#ThisId}
          {DefaultReturn}
        [] "count" then
          \insert ./commands/introspection/CountCommand
          {DefaultReturn}
        [] "vm" then
          \insert ./commands/introspection/VMCommand
          {DefaultReturn}
        [] "thread" then
          \insert ./commands/introspection/ThreadCommand
          {DefaultReturn}
        [] "threads" then
          \insert ./commands/introspection/ThreadsCommand
          {DefaultReturn}
        [] "variable" then
          \insert ./commands/introspection/VariableCommand
          {DefaultReturn}
        [] "variables" then
          \insert ./commands/introspection/VariablesCommand
          {DefaultReturn}

        % Journal and event management
        [] "journal" then
          \insert ./commands/JournalCommand
          {DefaultReturn}
        [] "track" then
          \insert ./commands/tracking/TrackCommand
          {DefaultReturn}
        [] "untrack" then
          \insert ./commands/tracking/UnTrackCommand
          {DefaultReturn}
        [] "tracks" then
          \insert ./commands/tracking/TracksCommand
          {DefaultReturn}

        % Program execution control
        [] "run" then
          \insert ./commands/execution/RunCommand
        [] "reset" then
          \insert ./commands/execution/ResetCommand
          {DefaultReturn}
        [] "continue" then
          {ContinueReturn Arguments}
        
        % Everything concerning the control of the VM
        [] "gc" then
          \insert ./commands/GCCommand
          {DefaultReturn}
        else
          {PrintError "Unknown command '"#Command#"'"#TRYHELP}
          {DefaultReturn}
        end
      end
    end
  end

  proc {Loop WatchedListIds}
    ExecutionMode = {Boot_Scheduler.getExecutionMode $}
    IsTrackingTriggered = {Boot_EventManager.isTrackingTriggered $}

    proc {CommandReturn}
      NewWatchedListIds
    in
      {ProcessCommand WatchedListIds NewWatchedListIds}
      {Loop NewWatchedListIds}
    end

    proc {DefaultReturn}
      {Boot_Thread.preempt This}
      {Loop WatchedListIds}
    end
  in
    {PrintInfo "Execution mode: "#ExecutionMode}
    {PrintInfo "Is tracking triggered: "#{Bool.toString IsTrackingTriggered $}}
    {PrintInfo "Watched list ids: "#{ListToString WatchedListIds $}}

    case ExecutionMode of normal then {CommandReturn}
    [] limitedSchedules then {DefaultReturn}
    [] limitedOperations then {DefaultReturn}
    [] untilTrackingTriggered then
      if IsTrackingTriggered then
        {PrintWarning "Tracking triggered"}

        if WatchedListIds \= nil then
          Lists = {Boot_Introspection.getLists WatchedListIds $}
        in
          if Lists \= nil then
            {DisplayCSV
              [
                "Id        " "KindId    " "GenerationId"
                "Hash    " "Owners                " flexible("List")
              ]
              Lists
              proc {$ Entry ?Record} Record = Entry end
              proc {$ Entry ?Result} Result = true end
              proc {$ Lst ?Result}
                case Lst of list(
                  id: Id
                  kindId: KindId
                  generationId: GenerationId
                  hash: Hash
                  owners: Owners
                  list: List
                ) then
                  Result = [
                    {Int.toString Id $}
                    {Int.toString KindId $}
                    {Int.toString GenerationId $}
                    {Int.toString Hash $}
                    {ListToString Owners $}
                    {Atom.toString {Boot_System.getRepr List ~1 ~1 $} $}
                  ]
                end
              end}
          else {PrintWarning "No lists matching watch list found"} end
        end

        {CommandReturn}
      else
        {DefaultReturn}
      end
    end
  end
in
  if {Boot_Thread.isPreemptible This $} then
    {Boot_Thread.setPreemptible This false}
  end

  {Loop nil}
end




























































































































