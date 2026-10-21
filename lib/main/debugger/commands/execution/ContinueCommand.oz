local
  proc {DisplayOptions}
    {DisplayFrame "Continue command without arguments" [
      "'empty'" "Continue the program execution by preempting the debugger thread once"
    ]}

    {DefaultReturn}
  end
in
  case Arguments of nil then
    {Boot_Scheduler.runUntilTrackingTriggered}
    {PreemptionReturn}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      {PrintUnexpectedOptionError Argument}
      {DefaultReturn}
    end
  end
end