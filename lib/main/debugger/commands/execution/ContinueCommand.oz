local
  proc {DisplayOptions}
    {DisplayFrame "Continue command without arguments" [
      "'empty'" "Continue the program execution by preempting the debugger thread once"
    ]}
  end
in
  case Arguments of nil then
    {Boot_Scheduler.runUntilTrackingTriggered}
    {Boot_Thread.preempt This}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end