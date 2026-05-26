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
          Id = {Int.toString
            {Boot_Introspection.getThreadInfo Thread id $} $}
          Type = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread type $} $}
          Runnable = {Bool.toString
            {Boot_Introspection.getThreadInfo Thread runnable $} $}
          Terminated = {Bool.toString
            {Boot_Introspection.getThreadInfo Thread terminated $} $}
          Dead = {Bool.toString
            {Boot_Introspection.getThreadInfo Thread dead $} $}
          Preempted = {Bool.toString
            {Boot_Introspection.getThreadInfo Thread preempted $} $}
          Preemptible = {Bool.toString
            {Boot_Introspection.getThreadInfo Thread preemptible $} $}
          Priority = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread priority $} $}
        in
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#Runnable#" "#
            "\t\t"#Terminated#" "#
            "\t\t"#Dead#" "#
            "\t"#Preempted#" "#
            "\t\t"#Preemptible#" "#
            "\t\t"#Priority}
        end}
    [] "identity" then
      {PrintLog "\tID \tTYPE \tKINDID \tGENID \tPRIORITY"}
      {ForEachThread Threads proc {$ Thread}
          Id = {Int.toString
            {Boot_Introspection.getThreadInfo Thread id $} $}
          Type = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread type $} $}
          KindId = {Int.toString
            {Boot_Introspection.getThreadInfo Thread kindId $} $}
          GenerationId = {Int.toString
            {Boot_Introspection.getThreadInfo Thread generationId $} $}
          Priority = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread priority $} $}
        in
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#KindId#" "#
            "\t"#GenerationId#" "#
            "\t"#Priority}
        end}
    [] "statistics" then
      {PrintLog "\tID \tTYPE \tRUNS \tRESUMES \tSUSPENDS \tSUSPENDSONVAR \tOPERATIONS \tBINDS"}
      {ForEachThread Threads proc {$ Thread}
          Id = {Int.toString
            {Boot_Introspection.getThreadInfo Thread id $} $}
          Type = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread type $} $}
          RunsCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread runsCount $} $}
          ResumesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread resumesCount $} $}
          SuspendsCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread suspendsCount $} $}
          SuspendsOnVarCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread suspendsOnVarCount $} $}
          OperationsCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread operationsCount $} $}
          BindsCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread bindsCount $} $}
        in
          {PrintLog "\t"#Id#" "#
            "\t"#Type#" "#
            "\t"#RunsCount#" "#
            "\t"#ResumesCount#" "#
            "\t\t"#SuspendsCount#" "#
            "\t\t"#SuspendsOnVarCount#" "#
            "\t\t"#OperationsCount#" "#
            "\t\t"#BindsCount}
        end}
    [] "nodes" then
      {PrintLog "\tID \tTYPE \tVARS \tVALS \tSTRUS \tTOKS \tSTABS \tUSTABS"#
        "\tXNODES \tYNODES \tGNODES \tKNODES \tSDEPTH \tNODES"}
      {ForEachThread Threads proc {$ Thread}
          Id = {Int.toString
            {Boot_Introspection.getThreadInfo Thread id $} $}
          Type = {Atom.toString
            {Boot_Introspection.getThreadInfo Thread type $} $}
          
          VariableNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread variableNodesCount $} $}
          ValueNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread valueNodesCount $} $}
          StructuralNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread structuralNodesCount $} $}
          TokenNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread tokenNodesCount $} $}
          
          StableNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread stableNodesCount $} $}
          UnstableNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread unstableNodesCount $} $}
          
          XNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread xNodesCount $} $}
          YNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread yNodesCount $} $}
          GNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread gNodesCount $} $}
          KNodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread kNodesCount $} $}
          
          StackDepth = {Int.toString
            {Boot_Introspection.getThreadInfo Thread stackDepth $} $}
          NodesCount = {Int.toString
            {Boot_Introspection.getThreadInfo Thread nodesCount $} $}
        in
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