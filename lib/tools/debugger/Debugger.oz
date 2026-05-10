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
  Boot_Debugger           at 'x-oz://boot/Debugger'
  System(show)

export
  'helloFromOZ': HelloFromOZ
  'value': Value

define
  proc {HelloFromOZ}
    {System.show 'helloFromOZ'}
    {Boot_Debugger.hello}
  end

  proc {Value ?R}
    R = 14
  end

end