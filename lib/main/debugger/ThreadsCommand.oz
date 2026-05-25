local
  Options = [
    option("status" "The thread statuses")
    option("statistics" "The thread statuses")
  ]

  proc {ForEachThread Threads Execute}
    case Threads of nil then skip
    [] Thread|NextThreads then
      {Execute Thread}
      {ForEachThread NextThreads Execute}
    end
  end
in
  case Arguments of Option|NextArguments then
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
    [] "help" then
      {PrintOptions "threads" Options false}
    else
      {PrintOptions "threads" Options true}
    end
  else
    {PrintOptions "threads" Options false}
  end
end