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

  TRYHELP = ", try help to get more details"

  % All printers to display runtime data, infos and errors
  \insert Printing

  \insert Error

  \insert Displaying
  
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
    /*
      It ensures the debugger will not be preempted during its analysis
      and so risking to produce an inconsistent result. However it is
      responsible to release the VM often to let other threads
      enough running time
    */
    if {Boot_Thread.isPreemptible This $} andthen NonPreemptible then
      {Boot_Thread.setPreemptible This false}
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
        else
          {PrintError "Unknown command '"#Command#"'"#TRYHELP}
        end
      end
    end

    {DefaultLoop}
  end
in
  {Loop true 0}
end
