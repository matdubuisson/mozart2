local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {PrintInfo "status\tdisplay the status for each thread"}
    {PrintInfo "identity\tdisplay the identity for each thread"}
    {PrintInfo "statistics\tdisplay the statistics for each thread"}
    {PrintInfo "nodes\tdisplay the nodes for each thread"}
  end

  proc {DisplayCommonOptions}
    {PrintInfo "min <int>\tspecifies from which thread id to start iterating"}
      {PrintInfo "max <int>\tspecifies from which thread id to stop iterating"}
  end

  proc {HandleOption Aggregate DisplayOptions DisplayAggregate Arguments Min Max}
    case Arguments of nil then
      {DisplayAggregate Min Max}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayOptions}
      [] "min" then
        case NextArguments of nil then
          {PrintError "Missing integer provided to min argument"}
        [] MinString|NextNextArguments then
          NewMin = {ExtractInput int MinString 0 $}
        in
          if NewMin == none then
            {PrintError "Provided parameter to min argument '"#MinString#"' is not an integer"} 
          else
            {HandleOption Aggregate DisplayOptions DisplayAggregate NextNextArguments NewMin Max}
          end
        end
      [] "max" then
        case NextArguments of nil then
          {PrintError "Missing integer provided to max argument"}
        [] MaxString|NextNextArguments then
          NewMax = {ExtractInput int MaxString 100 $}
        in
          if NewMax == none then
            {PrintError "Provided parameter to max argument '"#MaxString#"' is not an integer"} 
          else
            {HandleOption Aggregate DisplayOptions DisplayAggregate NextNextArguments Min NewMax}
          end
        end
      else
        {PrintError "Unexpected threads "#Aggregate#" aggregate argument '"#Argument#"'"#TRYHELP}
      end
    end   
  end

  % Status threads aggregate

  proc {DisplayStatusOptions}
    {DisplayCommonOptions}
  end

  proc {DisplayStatusAggregate Min Max}
    Threads = {Boot_Introspection.getThreads Min Max $}
  in
    {PrintLog "\tID \tTYPE \tRUNNABLE \tTERMINATED \tDEAD \tPREEMPTED \tPREEMPTIBLE \tPRIORITY"}
    {ForEachThread Threads proc {$ Thread}
      Status = {Boot_Introspection.getThreadStatus Thread $}
    in
      case Status of status(
        id: Id
        type: Type
        priority: Priority
        kindId: _
        generationId: _
        runnable: Runnable
        terminated: Terminated
        dead: Dead
        preempted: Preempted
        preemptible: Preemptible
      ) then
        {PrintLog "\t"#Id#" "#
          "\t"#Type#" "#
          "\t"#{Bool.toString Runnable $}#" "#
          "\t\t"#{Bool.toString Terminated $}#" "#
          "\t\t"#{Bool.toString Dead $}#" "#
          "\t"#{Bool.toString Preempted $}#" "#
          "\t\t"#{Bool.toString Preemptible $}#" "#
          "\t\t"#Priority}
      end
    end}
  end

  % Identity threads aggregate

  proc {DisplayIdentityOptions}
    {DisplayCommonOptions}
  end

  proc {DisplayIdentityAggregate Min Max}
    Threads = {Boot_Introspection.getThreads Min Max $}
  in
    {PrintLog "\tID \tTYPE \tKINDID \tGENID \tPRIORITY"}
    {ForEachThread Threads proc {$ Thread}
        Identity = {Boot_Introspection.getThreadIdentity Thread $}
      in
        case Identity of identity(
          id: Id
          kindId: KindId
          generationId: GenerationId
          type: Type
          priority: Priority
        ) then
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#KindId#" "#
            "\t"#GenerationId#" "#
            "\t"#Priority}
        end
      end}
  end

  % Threads statistics aggregate

  proc {DisplayStatisticsOptions}
    {DisplayCommonOptions}
  end

  proc {DisplayStatisticsAggregate Min Max}
    Threads = {Boot_Introspection.getThreads Min Max $}
  in
    {PrintLog "\tID \tTYPE \tRUNS \tRESUMES \tSUSPENDS \tSUSPENDSONVAR \tOPERATIONS \tBINDS"}
    {ForEachThread Threads proc {$ Thread}
        Identity = {Boot_Introspection.getThreadIdentity Thread $}
        Statistics = {Boot_Introspection.getThreadStatistics Thread $}
      in
        case Identity#Statistics of identity(
          id: Id
          kindId: _
          generationId: _
          type: Type
          priority: _
        )#statistics(
          runsCount: RunsCount
          resumesCount: ResumesCount
          suspendsCount: SuspendsCount
          suspendsOnVarCount: SuspendsOnVarCount
          operationsCount: OperationsCount
          bindsCount: BindsCount
        ) then
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#RunsCount#" "#
            "\t"#ResumesCount#" "#
            "\t\t"#SuspendsCount#" "#
            "\t\t"#SuspendsOnVarCount#" "#
            "\t\t"#OperationsCount#" "#
            "\t\t"#BindsCount}
        end
      end}
  end

  % Threads nodes aggregate

  proc {DisplayNodesOptions}
    {DisplayCommonOptions}
  end

  proc {DisplayNodesAggregate Min Max}
    Threads = {Boot_Introspection.getThreads Min Max $}
  in
    {PrintLog "\tID \tTYPE \tVARS \tVALS \tSTRUS \tTOKS \tSTABS \tUSTABS"#
      "\tXNODES \tYNODES \tGNODES \tKNODES \tSDEPTH \tNODES"}
    {ForEachThread Threads proc {$ Thread}
        Identity = {Boot_Introspection.getThreadIdentity Thread $}
        Nodes = {Boot_Introspection.getThreadNodesCounts Thread $}
      in
        case Identity#Nodes of identity(
          id: Id
          kindId: _
          generationId: _
          type: Type
          priority: _
        )#nodes(
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
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#VariableNodesCount#" "#
            "\t"#ValueNodesCount#" "#
            "\t"#StructuralNodesCount#" "#
            "\t"#TokenNodesCount#" "#
            "\t"#StableNodesCount#" "#
            "\t"#UnstableNodesCount#" "#
            "\t"#XNodesCount#" "#
            "\t"#YNodesCount#" "#
            "\t"#GNodesCount#" "#
            "\t"#KNodesCount#" "#
            "\t"#StackDepth#" "#
            "\t"#NodesCount}
        end
      end}
  end

  proc {ForEachThread Threads Execute}
    case Threads of nil then skip
    [] Thread|NextThreads then
      {Execute Thread}
      {ForEachThread NextThreads Execute}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "status" then
      {HandleOption "status" DisplayStatusOptions DisplayStatusAggregate
        NextArguments 0 100}
    [] "identity" then
      {HandleOption "identity" DisplayIdentityOptions DisplayIdentityAggregate
        NextArguments 0 100}
    [] "statistics" then
      {HandleOption "statistics" DisplayStatisticsOptions DisplayStatisticsAggregate
        NextArguments 0 100}
    [] "nodes" then
      {HandleOption "nodes" DisplayNodesOptions DisplayNodesAggregate
        NextArguments 0 100}
    else
      {PrintError "Unexpected threads aggregate '"#Argument#"'"#TRYHELP}
    end
  end
end