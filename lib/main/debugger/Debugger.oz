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
  Boot at 'x-oz://boot/Boot'
define
  Boot_System = {Boot.getInternal 'System'}
  proc {Test}
    {Boot_System.printVS "Hello again from debugger" false true}
    {Time.delay 500}
    {Test}
  end
in
  {Test}
end
