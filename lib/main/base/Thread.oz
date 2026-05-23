%%%
%%% Authors:
%%%   Michael Mehl (mehl@dfki.de)
%%%   Martin Mueller (mmueller@ps.uni-sb.de)
%%%   Christian Schulte <schulte@ps.uni-sb.de>
%%%
%%% Copyright:
%%%   Michael Mehl, 1997
%%%   Martin Mueller, 1997
%%%   Christian Schulte, 1997
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


%%
%% Module
%%
local
   % Thread getters
   This                 = Boot_Thread.this
   GetThread            = Boot_Thread.getThread

   % Ids getters
   GetId                = Boot_Thread.getId
   GetKindId            = Boot_Thread.getKindId
   GetGenerationId      = Boot_Thread.getGenerationId

   % Priority control
   GetPriority          = Boot_Thread.getPriority
   SetPriority          = Boot_Thread.setPriority

   % State getters
   IsRunnable           = Boot_Thread.isRunnable
   IsTerminated         = Boot_Thread.isRunnable
   IsDead               = Boot_Thread.isRunnable
   State                = Boot_Thread.state
   GetState             = Boot_Thread.getState

   % Exception injection
   InjectException      = Boot_Thread.injectException

   % Preemption control
   IsPreempted          = Boot_Thread.isPreempted
   IsPreemptible        = Boot_Thread.isPreemptible
   SetPreemptible       = Boot_Thread.setPreemptible

   Resume               = Boot_Thread.resume
   Suspend              = Boot_Thread.suspend
   Preempt              = Boot_Thread.preempt
   Kill                 = Boot_Thread.kill
in
   Thread = 'thread'(is:                  IsThread
                     this:                This
                     getThread:           GetThread

                     getId:               GetId
                     getKindId:           GetKindId
                     getGenerationId:     GetGenerationId

                     getPriority:         GetPriority
                     setPriority:         SetPriority
                     
                     isRunnable:          IsRunnable
                     isTerminated:        IsTerminated
                     isDead:              IsDead
                     state:               State
                     getState:            GetState

                     injectException:     InjectException

                     isPreempted:         IsPreempted
                     isPreemptible:       IsPreemptible
                     setPreemptible:      SetPreemptible

                     resume:              Resume
                     suspend:             Suspend
                     preempt:             Preempt
                     kill:                Kill
                     terminate:           proc {$ T}
                                                {Thread.injectException T
                                                   {Exception.system
                                                   kernel(terminate)}}
                                          end
   )

end
