local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      [
        "state"
        "statistics"
        "nodes"
      ]
      [
        "display the state"
        "display the statistics"
        "display the nodes proportions"
      ]}
  end

  proc {HandleStateOption}
    SchedulesCounter = {Boot_Introspection.getSchedulesCounter $}
    OperationsCounter = {Boot_Introspection.getOperationsCounter $}
    SystemSchedulesCounter = {Boot_Introspection.getSystemSchedulesCounter $}
    SystemOperationsCounter = {Boot_Introspection.getSystemOperationsCounter $}
    GCSchedulesCount = {Boot_Introspection.getGCSchedulesCount $}

    ThreadsCount = {Boot_Introspection.getThreadsCount $}
    VariablesCount = {Boot_Introspection.getVariablesCount $}
    NodesCount = {Boot_Introspection.getNodesCount $}
  in
  	{PrintInfo "Virtual machine status:"}
	
    {PrintInfo "\tSchedules counter: "#
      {Int.toString SchedulesCounter $}}
    {PrintInfo "\tOperations counter: "#
      {Int.toString OperationsCounter $}}

    {PrintInfo "\tSystem schedules counter: "#
      {Int.toString SystemSchedulesCounter $}}
    {PrintInfo "\tSystem operations counter: "#
      {Int.toString SystemOperationsCounter $}}

    {PrintInfo "\tNon-system schedules counter: "#
      {Int.toString (SchedulesCounter - SystemSchedulesCounter) $}}
    {PrintInfo "\tNon-system operations counter: "#
      {Int.toString (OperationsCounter - SystemOperationsCounter) $}}

    {PrintInfo "\tThreads count: "#
      {Int.toString ThreadsCount $}}
  end

  proc {HandleStatisticsOption}
    skip
  end

  proc {HandleNodesOption}
    skip
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