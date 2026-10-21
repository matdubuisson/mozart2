% WARNING this command is responsible to manage the return action itself !!

local
  proc {DisplayOptions}
    {DisplayFrame "Run command options" [
      "schedules <count>" "Run x schedules of non-system threads (debugger thread excluded)"
      "operations <count>" "Run x operations of any non-system thread leading to an arbitrary number of schedules (debugger thread excluded)"
      "until" "Run until the next interruption either being a breakpoint or a triggered tracking rule"
      "steps <identities>" "Run step by step on streams"
    ]}
    
    {DefaultReturn}
  end

  proc {HandleRunN Thing RunN Arguments}
    case Arguments of nil then
      {PrintError "Missing "#Thing#" count"}
      {DefaultReturn}
    [] CountString|NextArguments then
      Count = {ExtractInput int CountString none $}
    in
      if Count == none then
        {PrintError "Provided first parameter "#Thing#" count '"#CountString#"' is not an integer"}
        {DefaultReturn}
      else
        {RunN Count}
        {PreemptionReturn}
      end
    end
  end

  proc {HandleUntil}
    {Boot_Scheduler.runUntilTrackingTriggered}
    {DefaultReturn}
  end

  proc {HandleStepByStep Arguments}
    Ids = {ExtractIdentities Arguments $}
  in
    if {List.is Ids $} then
      {Boot_EventManager.track variable bound none Ids}
      {Boot_Scheduler.runUntilTrackingTriggered}

      % Custom return here
      PreemptionDone = false
      NewWatchedListIds = Ids
    else
      {DefaultReturn}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "schedules" then
      {HandleRunN "schedules" Boot_Scheduler.runNSchedules NextArguments}
    [] "operations" then
      {HandleRunN "operations" Boot_Scheduler.runNOperations NextArguments}
    [] "until" then
      {HandleUntil}
    [] "steps" then
      {HandleStepByStep NextArguments}
    else
      {PrintError "Unexpected run option '"#Argument#"'"#TRYHELP}
      {DefaultReturn}
    end
  end
end