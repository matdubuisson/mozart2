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

  proc {Loop}
    This = {Boot_Thread.this $}
    Stats = stats(
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
  in
    {Boot_Thread.setPriority This 'high'}

    {Boot_System.printRepr Stats false true}

    {Boot_System.printVS "[DEBUGGER] Enter command: " false false}
    local Command in
      Command = {Boot_System.inputVSLine $}
      {Boot_System.printVS "You entered: "#Command false true}
    end
    
    {Boot_Thread.setPriority This 'low'}
    {Time.delay 500}
    {Loop}
  end
in
  {Loop}
end
