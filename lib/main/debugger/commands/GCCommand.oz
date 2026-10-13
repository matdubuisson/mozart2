local
  proc {DisplayOptions}
    {DisplayFrame "GC command controls" [
      "on" "Turn on the garbage collector"
      "off" "Turn off the garbage collector but be aware the VM has a risk to accumulate too much memory and therefore to crash however very usefull when we need to debug data out of scope"
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