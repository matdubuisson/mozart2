local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["on" "off"]
      [
        "Allows the garbage collector to run"
        "Prevents the garbage collector to run, warning with this mode"
      ]}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|_ then
    case Argument of "help" then
      {DisplayOptions}
    [] "on" then
      {Boot_Scheduler.enableGC}
    [] "off" then
      {Boot_Scheduler.disableGC}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end