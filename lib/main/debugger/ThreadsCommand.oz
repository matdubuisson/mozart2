local
  Options = [
    option("status" "The thread statuses")
    option("identity" "The thread identities")
    option("statistics" "The thread stats counts")
    option("nodes" "The thread nodes counts")
  ]

  proc {ForEachThread Threads Execute}
    case Threads of nil then skip
    [] Thread|NextThreads then
      {Execute Thread}
      {ForEachThread NextThreads Execute}
    end
  end
in
  case Arguments of Option|_ then
    Threads
  in
    local
      From = 0
      To = 100
    in
      Threads = {Boot_Introspection.getThreads From To $}
    end

    case Option of "status" then
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
    [] "identity" then
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
    [] "statistics" then
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
    [] "nodes" then
      {PrintLog "\tID \tTYPE \tVARS \tVALS \tSTRUS \tTOKS \tSTABS \tUSTABS"#
        "\tXNODES \tYNODES \tGNODES \tKNODES \tSDEPTH \tNODES"}
      {ForEachThread Threads proc {$ Thread}
          Identity = {Boot_Introspection.getThreadIdentity Thread $}
          Nodes = {Boot_Introspection.getThreadNodesProperties Thread $}
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
    [] "help" then
      {PrintOptions "threads" Options false}
    else
      {PrintOptions "threads" Options true}
    end
  else
    {PrintOptions "threads" Options false}
  end
end