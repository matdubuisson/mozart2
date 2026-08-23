local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {DisplayNameDescriptions
      [
        "state"
        "statistics"
        "nodes"
      ]
      [
        "display the state for each thread"
        "display the statistics for each thread"
        "display the nodes for each thread"
      ]}
  end

  proc {HandleStateOption From To Conditions}
    States = {Boot_Introspection.getAllThreadStates From To $}
  in
    {DisplayCSV
      [
        "Id" "KindId" "GenerationId" % Ids
        "Priority" "Type" % Importance
        "Runnable" "Terminated" "Dead" "Preempted" "Preemptible" % State
      ]
      {FilterInputsUsingFilteringParameters
        States Conditions $}
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
    Conditions = {ExtractFilteringParameters Arguments $}
  in
    case Option of state then
      {HandleStateOption 0 100 Conditions}
    [] statistics then
      {HandleStatisticsOption 0 100 Conditions}
    [] nodes then
      {HandleNodesOption 0 100 Conditions}
    end
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