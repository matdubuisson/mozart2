local
  proc {DisplayOptions}
    {DisplayFrame "DebugInfo command options" [
      "abstractions <condition>" "displaying procedure descritpions including procedure name and all parameters' names"
      "variables <condition>" "showing mapping of ids to list of source names"
    ]}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end