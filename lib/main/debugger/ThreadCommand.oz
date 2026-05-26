local
  Options = [
    option("id <value>" "Taking one integer representing a thread id")
    option("ids <value0 .... valueN>" "Taking a chain of ids to display several threads")
  ]

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
        {PrintLog "\t\t=> Runs: "#RunsCount}
        {PrintLog "\t\t=> Resumes: "#ResumesCount}
        {PrintLog "\t\t=> Suspends: "#SuspendsCount}
        {PrintLog "\t\t=> SuspendsOnVar: "#SuspendsOnVarCount}
        {PrintLog "\t\t=> Operations: "#OperationsCount}
        {PrintLog "\t\t=> Binds: "#BindsCount}
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
        {PrintLog "\t\t=> Variable nodes: "#VariableNodesCount}
        {PrintLog "\t\t=> Value nodes: "#ValueNodesCount}
        {PrintLog "\t\t=> Structural nodes: "#StructuralNodesCount}
        {PrintLog "\t\t=> Token nodes: "#TokenNodesCount}

        {PrintLog "\t\t=> Stable nodes: "#StableNodesCount}
        {PrintLog "\t\t=> Unstable nodes: "#UnstableNodesCount}

        {PrintLog "\t\t=> X nodes: "#XNodesCount}
        {PrintLog "\t\t=> Y nodes: "#YNodesCount}
        {PrintLog "\t\t=> G nodes: "#GNodesCount}
        {PrintLog "\t\t=> K nodes: "#KNodesCount}

        {PrintLog "\t\t=> Stack depth: "#StackDepth}
        {PrintLog "\t\t=> Total nodes: "#NodesCount}
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
      {PrintError "Bad provided id '"#IdString#"'"}
    else
      Th = {Boot_Introspection.getThread Id $}
    in
      if Th == none then
        {PrintError "Thread "#Id#" is not found"}
      else
        {DisplayThread Th Id}
      end
    end
  end
in
  case Arguments of Option|NextArguments then
    case Option of "id" then
      case NextArguments of IdString|_ then
        {FindAndDisplayThread IdString}
      else
        {PrintError "Option id takes an integer value"}
      end
    [] "ids" then
      proc {ForEachIdString IdStrings}
        case IdStrings of nil then skip
        [] IdString|NextIdStrings then
          {FindAndDisplayThread IdString}
          {ForEachIdString NextIdStrings}
        end
      end
    in
      {ForEachIdString NextArguments}
    [] "help" then
      {PrintOptions "threads" Options true}
    else
      {PrintOptions "threads" Options true}
    end
  else
    {PrintOptions "thread" Options false}
  end
end