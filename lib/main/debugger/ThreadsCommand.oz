local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["state" "statistics" "nodes"]
      [
        "display the state for each thread"
        "display the statistics for each thread"
        "display the nodes for each thread"
      ]}
  end

  proc {HandleStateOption From To}
    States = {Boot_Introspection.getAllThreadStates From To $}
  in
    {DisplayCSV
      ["Id" "KindId" "GenerationId" "Priority" "Type" "Runnable" "Terminated" "Dead" "Preempted" "Preemptible"]
      States
      12
      FormatThreadState}
  end

  proc {HandleStatisticsOption From To}
    Statistics = {Boot_Introspection.getAllThreadStatistics From To $}
  in
    {DisplayCSV
      ["Id" "RunsCount" "ResumesCount" "SuspendsCount" "SuspendsOnVarCount" "OperationsCount" "BindsCount"]
      Statistics
      12
      FormatThreadStatistics}
  end

  proc {HandleNodesOption From To}
    Nodes = {Boot_Introspection.getAllThreadNodesCounts From To $}
  in
    {DisplayCSV
      ["Id" "Variables" "Values" "Structures" "Tokens" "Stable" "Unstable" "X" "Y" "G" "K" "StackDepth" "Total"]
      Nodes
      12
      FormatThreadNodesCounts}
  end

  proc {HandleOption Option Arguments}
    From To
  in
    {ExtractFromTo Arguments 0 100 From To}

    case Option of state then
      {HandleStateOption From To}
    [] statistics then
      {HandleStatisticsOption From To}
    [] nodes then
      {HandleNodesOption From To}
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