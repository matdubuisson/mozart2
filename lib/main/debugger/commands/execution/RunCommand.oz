local
  proc {DisplayOptions}
    {PrintInfo "schedules <x> <includeSystemThreads=false>\tschedules x threads"}
    {PrintInfo "operations <x> <includeSystemThreads=false>\texecutes x threads"}
    {PrintInfo "operationByOperation (alias obo)\tEnters the operation by operation execution mode"}
  end

  \insert RunNCommand
  \insert RunOBOCommand
  \insert RunSBSCommand

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
    [] "operationByOperation" then
      {HandleOperationByOperation NextArguments}
    [] "obo" then
      {HandleOperationByOperation NextArguments}
    [] "stepByStep" then
      {HandleStepByStep NextArguments}
    [] "sbs" then
      {HandleStepByStep NextArguments}
    [] "untilAlarm" then
      {Cell.assign ModeCell false}
      {Boot_Thread.preempt This}
    else
      {PrintError "Unexpected run option '"#Argument#"'"#TRYHELP}
    end
  end
end