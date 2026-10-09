local
  proc {DisplayOptions}
    {DisplayFrame "VM command options" [
      "state" "display the state of the Virtual Machine"
      "statistics" "display statistics related to the Virtual Machine"
      "nodes" "display the state nodes proportions related to the Virtual Machine"
    ]}
  end

  proc {HandleStateOption}
    ThreadsCount = {Boot_Introspection.getThreadsCount $}
    VariablesCount = {Boot_Introspection.getVariablesCount $}
    NodesCount = {Boot_Introspection.getNodesCount $}
  in
    {DisplayFrame "Virtual Machine state" [
      "Threads count" ThreadsCount
      "Variables count" VariablesCount
      "Nodes count" NodesCount
    ]}
  end

  proc {HandleStatisticsOption}
    SchedulesCount = {Boot_Introspection.getSchedulesCount $}
    OperationsCount = {Boot_Introspection.getOperationsCount $}
    SystemSchedulesCount = {Boot_Introspection.getSystemSchedulesCount $}
    SystemOperationsCount = {Boot_Introspection.getSystemOperationsCount $}
    GCSchedulesCount = {Boot_Introspection.getGCSchedulesCount $}
  in
    {DisplayFrame "Virtual Machine statistics" [
      "Schedules count" SchedulesCount
      "Operations count" OperationsCount
      "SystemSchedules count" SystemSchedulesCount
      "SystemOperations count" SystemOperationsCount
      "GCSchedules count" GCSchedulesCount
    ]}
  end

  proc {HandleNodesOption}
    VariableNodesCount = {Boot_Introspection.getVariableNodesCount $}
    ValueNodesCount = {Boot_Introspection.getValueNodesCount $}
    StructuralNodesCount = {Boot_Introspection.getStructuralNodesCount $}
    TokenNodesCount = {Boot_Introspection.getTokenNodesCount $}

    StableNodesCount = {Boot_Introspection.getStableNodesCount $}
    UnstableNodesCount = {Boot_Introspection.getUnstableNodesCount $}

    XNodesCount = {Boot_Introspection.getXNodesCount $}
    YNodesCount = {Boot_Introspection.getYNodesCount $}
    GNodesCount = {Boot_Introspection.getGNodesCount $}
    KNodesCount = {Boot_Introspection.getKNodesCount $}

    TotalStackDepth = {Boot_Introspection.getStackDepth $}
    NodesCount = {Boot_Introspection.getNodesCount $}
  in
    {DisplayFrame "Virtual Machine nodes" [
      {MakeStringWithPercent "Variable nodes count" VariableNodesCount NodesCount} VariableNodesCount
      {MakeStringWithPercent "Value nodes count" ValueNodesCount NodesCount} ValueNodesCount
      {MakeStringWithPercent "Structural nodes count" StructuralNodesCount NodesCount} StructuralNodesCount
      {MakeStringWithPercent "Token nodes count" TokenNodesCount NodesCount} TokenNodesCount

      {MakeStringWithPercent "Stable nodes count" StableNodesCount NodesCount} StableNodesCount
      {MakeStringWithPercent "Unstable nodes count" UnstableNodesCount NodesCount} UnstableNodesCount

      {MakeStringWithPercent "X nodes count" XNodesCount NodesCount} XNodesCount
      {MakeStringWithPercent "Y nodes count" YNodesCount NodesCount} YNodesCount
      {MakeStringWithPercent "G nodes count" GNodesCount NodesCount} GNodesCount
      {MakeStringWithPercent "K nodes count" KNodesCount NodesCount} KNodesCount

      "Sum of all stack depths" TotalStackDepth
      "Total number of nodes" NodesCount
    ]}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      {HandleStateOption}
    [] "statistics" then
      {HandleStatisticsOption}
    [] "nodes" then
      {HandleNodesOption}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end