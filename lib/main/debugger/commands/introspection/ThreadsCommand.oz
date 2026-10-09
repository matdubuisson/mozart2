local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {DisplayFrame "Thread command options" [
      "state <condition>" "display the state of a selected set of threads"
      "statistics <condition>" "display statistics related to a selection of threads"
      "nodes <condition>" "display the state nodes proportions related to a selection of threads"
    ]}
  end

  proc {HandleStateOption From To Conditions}
    States = {Boot_Introspection.getAllThreadStates From To $}
    Inputs = {FilterInputsUsingFilteringParameters
      States Conditions $}
  in
    {DisplayCSV
      [
        "Id" "KindId" "GenerationId" % Ids
        "Priority" "Type" % Importance
        "Runnable" "Terminated" "Dead" "Preempted" "Preemptible" % State
      ]
      Inputs
      12
      FormatThreadState}
  end

  proc {HandleStatisticsOption From To Conditions}
    Statistics = {Boot_Introspection.getAllThreadStatistics From To $}
  in
    {DisplayCSV
      [
        "Id"
        "RunsCount" "ResumesCount"
        "SuspendsCount" "SuspendsOnVarCount"
        "OperationsCount" "BindsCount"
      ]
      {FilterInputsUsingFilteringParameters
        Statistics Conditions $}
      12
      FormatThreadStatistics}
  end

  proc {HandleNodesOption From To Conditions}
    Nodes = {Boot_Introspection.getAllThreadNodesCounts From To $}
  in
    {DisplayCSV
      [
        "Id"
        "Variables" "Values" "Structures" "Tokens" % Family
        "Stable" "Unstable" % Modifiable
        "X" "Y" "G" "K" % Type
        "StackDepth" "Total" % How many
      ]
      {FilterInputsUsingFilteringParameters
        Nodes Conditions $}
      12
      FormatThreadNodesCounts}
  end

  proc {HandleOption Option Arguments}
    Conditions = {ExtractConditions Arguments $}
  in
    {Boot_System.printRepr Conditions false true}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      {HandleOption state NextArguments}
    [] "statistics" then
      {HandleOption statistics NextArguments}
    [] "nodes" then
      {HandleOption nodes NextArguments}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end