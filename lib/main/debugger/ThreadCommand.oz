local
  proc {DisplayOptions}
    {PrintInfo "<id0> .... <idN>\tdisplay the state of one or several thread(s) specified by their id"}
  end

  proc {DisplayThread Thread Id}
    State = {Boot_Introspection.getThreadState Thread $}
  in
    case State of state(nodes:Nodes statistics:Statistics status:Status) then
      case Status of status(
        id: Id
        type: Type
        priority: Priority
        kindId: KindId
        generationId: GenerationId
        runnable: Runnable
        terminated: Terminated
        dead: Dead
        preempted: Preempted
        preemptible: Preemptible
      ) then
        {PrintLog "Thread "#Id#":"}
        {PrintLog "\tType: "#Type}
        {PrintLog "\tPriority: "#Priority}
        {PrintLog "\tKind id: "#KindId}
        {PrintLog "\tGeneration id: "#GenerationId}

        {PrintLog "\tStatus: "#
          if Dead then "dead" else "alive" end#
          " "#
          if Terminated then "terminated" elseif Runnable then "runnable"
            else "non-runnable" end}

        {PrintLog "\tPreemption: "#
          if Preempted then "preempted" else "running" end#" "#
          if Preemptible then "preemptible" else "non-preemptible" end}
      end

      case Statistics of statistics(
        runsCount: RunsCount
        resumesCount: ResumesCount
        suspendsCount: SuspendsCount
        suspendsOnVarCount: SuspendsOnVarCount
        operationsCount: OperationsCount
        bindsCount: BindsCount
      ) then        
        {PrintLog "\tStatistics:"}
        {PrintLog "\t\tRuns: "#RunsCount}
        {PrintLog "\t\tResumes: "#ResumesCount}
        {PrintLog "\t\tSuspends: "#SuspendsCount}
        {PrintLog "\t\tSuspendsOnVar: "#SuspendsOnVarCount}
        {PrintLog "\t\tOperations: "#OperationsCount}
        {PrintLog "\t\tBinds: "#BindsCount}
      end

      case Nodes of nodes(
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
        {PrintLog "\tNodes:"}
        {PrintLog "\t\tVariable nodes: "#VariableNodesCount}
        {PrintLog "\t\tValue nodes: "#ValueNodesCount}
        {PrintLog "\t\tStructural nodes: "#StructuralNodesCount}
        {PrintLog "\t\tToken nodes: "#TokenNodesCount}

        {PrintLog "\t\tStable nodes: "#StableNodesCount}
        {PrintLog "\t\tUnstable nodes: "#UnstableNodesCount}

        {PrintLog "\t\tX nodes: "#XNodesCount}
        {PrintLog "\t\tY nodes: "#YNodesCount}
        {PrintLog "\t\tG nodes: "#GNodesCount}
        {PrintLog "\t\tK nodes: "#KNodesCount}
        {PrintLog "\t\tStack depth: "#StackDepth}
        {PrintLog "\t\tTotal nodes: "#NodesCount}
      end
    end
  end

  proc {FindAndDisplayThread IdString}
    Id
  in
    try
      Id = {String.toInt IdString $}
    catch _ then
      Id = none
    end

    if Id == none then
      {PrintError "Argument '"#IdString#"' is not an id"}
    else
      Th = {Boot_Introspection.getThread Id $}
    in
      if Th == none then
        {PrintError "Thread "#Id#" does not exist"}
      else
        {DisplayThread Th Id}
      end
    end
  end

  proc {ForEach Arguments}
    case Arguments of nil then skip
    [] Argument|NextArguments then
      {FindAndDisplayThread Argument}
      {ForEach NextArguments}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  else
    {ForEach Arguments}
  end
end