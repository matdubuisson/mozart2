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
  
  \insert Interface

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
  in
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
    % local
    %   NextThread = {Boot_Introspection.getNextScheduledThread false $}
    % in
    %   {PrintLog
    %     "Next scheduled thread is: "#
    %     {Int.toString {Boot_Thread.getId NextThread $} $}}
    % end

    /*
      Terminal commands management and execution of them
    */




    {PrintPrefix}

    local
      Input = {Boot_System.inputVSLine $}
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
      [] "status" then
        \insert StatusCommand
      [] "run" then
        \insert RunCommand
      else
        {PrintError "Unknown command '"#Command#"'"#TRYHELP}
      end
    end

    {DefaultLoop}
  end
in
  {Loop true 0}
end
