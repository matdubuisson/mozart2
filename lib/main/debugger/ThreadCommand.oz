local
  Options = [
    option("id <value>" "Taking one integer representing a thread id")
    option("ids <value0 .... valueN>" "Taking a chain of ids to display several threads")
  ]

  proc {DisplayThread Thread Id}
    Type = {Atom.toString
      {Boot_Introspection.getThreadInfo Thread type $} $}
    Priority = {Atom.toString
      {Boot_Introspection.getThreadInfo Thread priority $} $}
    KindId = {Int.toString
      {Boot_Introspection.getThreadInfo Thread kindId $} $}
    GenerationId = {Int.toString
      {Boot_Introspection.getThreadInfo Thread generationId $} $}

    Runnable = {Boot_Introspection.getThreadInfo Thread runnable $}
    Terminated = {Boot_Introspection.getThreadInfo Thread terminated $}
    Dead = {Boot_Introspection.getThreadInfo Thread dead $}
    Preempted = {Boot_Introspection.getThreadInfo Thread preempted $}
    Preemptible = {Boot_Introspection.getThreadInfo Thread preemptible $}
    
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
      if Preempted then "preempted" else "non-preempted" end#
      if Preemptible then "preemptible" else "non-preemptible" end}
    
    {PrintLog "\tStatistics:"}
    {PrintLog "\t\t=> Runs: "#RunsCount}
    {PrintLog "\t\t=> Resumes: "#ResumesCount}
    {PrintLog "\t\t=> Suspends: "#SuspendsCount}
    {PrintLog "\t\t=> SuspendsOnVar: "#SuspendsOnVarCount}
    {PrintLog "\t\t=> Operations: "#OperationsCount}
    {PrintLog "\t\t=> Binds: "#BindsCount}

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