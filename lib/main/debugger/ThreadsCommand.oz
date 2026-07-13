local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {PrintInfo "state\tdisplay the state for each thread"}
    {PrintInfo "statistics\tdisplay the statistics for each thread"}
    {PrintInfo "nodes\tdisplay the nodes for each thread"}
  end

  proc {FormatThreadState State ?Result}
    case State of state(
      id: Id
      kindId: KindId
      generationId: GenerationId

      priority: Priority
      type: Type

      runnable: Runnable
      terminated: Terminated
      dead: Dead
      preempted: Preempted
      preemptible: Preemptible
    ) then
      Result = [
        {Int.toString Id $}
        {Int.toString KindId $}
        {Int.toString GenerationId $}

        {Atom.toString Priority $}
        {Atom.toString Type $}

        {Bool.toString Runnable $}
        {Bool.toString Terminated $}
        {Bool.toString Dead $}
        {Bool.toString Preempted $}
        {Bool.toString Preemptible $}
      ]
    end
  end

  proc {FormatThreadStatistics Statistics ?Result}
    case Statistics of statistics(
      id: Id
      runsCount: RunsCount
      resumesCount: ResumesCount
      suspendsCount: SuspendsCount
      suspendsOnVarCount: SuspendsOnVarCount
      operationsCount: OperationsCount
      bindsCount: BindsCount
    ) then
      Result = [
        {Int.toString Id $}
        {Int.toString RunsCount $}
        {Int.toString ResumesCount $}
        {Int.toString SuspendsCount $}
        {Int.toString SuspendsOnVarCount $}
        {Int.toString OperationsCount $}
        {Int.toString BindsCount $}
      ]
    end
  end

  proc {FormatThreadNodesCounts NodesCounts ?Result}
    case NodesCounts of nodes(
      id: Id
      variableNodesCount: VariableNodesCount
      valueNodesCount: ValueNodesCount
      structuralNodesCount: StructuralNodesCount
      tokenNodesCount: TokenNodesCount
      stableNodesCount: StableNodesCount
      unstableNodesCount: UnstableNodesCount
      xNodesCount: XNodesCount
      yNodesCount: YNodesCount
      gNodesCount: GNodesCount
      kNodesCount: KNodesCount
      stackDepth: StackDepth
      nodesCount: NodesCount
    ) then
      Result = [
        {Int.toString Id $}
        {Int.toString VariableNodesCount $}
        {Int.toString ValueNodesCount $}
        {Int.toString StructuralNodesCount $}
        {Int.toString TokenNodesCount $}
        {Int.toString StableNodesCount $}
        {Int.toString UnstableNodesCount $}
        {Int.toString XNodesCount $}
        {Int.toString YNodesCount $}
        {Int.toString GNodesCount $}
        {Int.toString KNodesCount $}
        {Int.toString StackDepth $}
        {Int.toString NodesCount $}
      ]
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      States = {Boot_Introspection.getAllThreadStates 0 100000 $}
    in
      {DisplayCSV
        ["Id" "KindId" "GenerationId" "Priority" "Type" "Runnable" "Terminated" "Dead" "Preempted" "Preemptible"]
        States
        12
        FormatThreadState}
    [] "statistics" then
      {DisplayCSV
        ["Id" "RunsCount" "ResumesCount" "SuspendsCount" "SuspendsOnVarCount" "OperationsCount" "BindsCount"]
        {Boot_Introspection.getAllThreadStatistics 0 100000 $}
        12
        FormatThreadStatistics}
    [] "nodes" then
      {DisplayCSV
        ["Id" "Variables" "Values" "Structures" "Tokens" "Stable" "Unstable" "X" "Y" "G" "K" "StackDepth" "Total"]
        {Boot_Introspection.getAllThreadNodesCounts 0 100000 $}
        12
        FormatThreadNodesCounts}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end