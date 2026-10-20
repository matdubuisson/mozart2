local
  proc {DisplayOptions}
    {DisplayFrame "Reset command without arguments" [
      "'empty'" "Simple reset to the normal execution mode"
    ]}
  end
in
  case Arguments of nil then
    {Boot_Scheduler.reset}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end