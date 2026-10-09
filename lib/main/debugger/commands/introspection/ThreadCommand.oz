local
  proc {DisplayOptions}
    {DisplayFrame "Thread command options" [
      "state <identities>" "display the state of the specified thread(s)"
      "statistics <identities>" "display statistics related to the thread(s)"
      "nodes <identities>" "display the state nodes proportions related to thread(s)"
      "all <identities>" "display all available options just listed before"
    ]}
  end

  proc {DisplayState Id Thread}
    State = {Boot_Introspection.getThreadState Thread $}
  in
    case State of state(
      id: _
      kindId: KindId
      generationId: GenerationId
      priority: Priority

      runnable: Runnable
      terminated: Terminated
      dead: Dead
      preempted: Preempted
      preemptible: Preemptible
    ) then
      {DisplayFrame "Thread state" [
        "Id" Id
        "Kind Id" KindId
        "Generation Id" GenerationId
        "Priority" Priority

        "Is runnable" {Bool.toString Runnable $}
        "Is terminated" {Bool.toString Terminated $}
        "Is dead" {Bool.toString Dead $}
        "Is preempted" {Bool.toString Preempted $}
        "Is preemptible" {Bool.toString Preemptible $}
      ]}
    end
  end

  proc {DisplayStatistics Id Thread}
    Statistics = {Boot_Introspection.getThreadStatistics Thread $}
  in
    case Statistics of statistics(
      runsCount: RunsCount
      resumesCount: ResumesCount
      suspendsCount: SuspendsCount
      suspendsOnVarCount: SuspendsOnVarCount
      operationsCount: OperationsCount
      bindsCount: BindsCount
    ) then
      TotalCount = RunsCount + ResumesCount + SuspendsCount
        + SuspendsOnVarCount + OperationsCount + BindsCount
    in
      {DisplayFrame "Thread state" [
        "Id" Id

        {MakeStringWithPercent "Runs count" RunsCount TotalCount $} RunsCount
        {MakeStringWithPercent "Resumes count" ResumesCount TotalCount $} ResumesCount
        {MakeStringWithPercent "Suspends count" SuspendsCount TotalCount $} SuspendsCount
        {MakeStringWithPercent "Suspends on var count" SuspendsOnVarCount TotalCount $} SuspendsOnVarCount
        {MakeStringWithPercent "Operations count" OperationsCount TotalCount $} OperationsCount
        {MakeStringWithPercent "Binds count" BindsCount TotalCount $} BindsCount
      ]}
    end
  end

  proc {DisplayNodes Id Thread}
    Nodes = {Boot_Introspection.getThreadNodesCounts Thread $}
  in
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
      {DisplayFrame "Thread state" [
        "Id" Id

        {MakeStringWithPercent "Variable nodes count" VariableNodesCount NodesCount $} VariableNodesCount
        {MakeStringWithPercent "Value nodes count" ValueNodesCount NodesCount $} ValueNodesCount
        {MakeStringWithPercent "Structural nodes count" StructuralNodesCount NodesCount $} StructuralNodesCount
        {MakeStringWithPercent "Token nodes count" TokenNodesCount NodesCount $} TokenNodesCount
        
        {MakeStringWithPercent "Stable nodes count" StableNodesCount NodesCount $} StableNodesCount
        {MakeStringWithPercent "Unstable nodes count" UnstableNodesCount NodesCount $} UnstableNodesCount
        
        {MakeStringWithPercent "X nodes count" XNodesCount NodesCount $} XNodesCount
        {MakeStringWithPercent "Y nodes count" YNodesCount NodesCount $} YNodesCount
        {MakeStringWithPercent "G nodes count" GNodesCount NodesCount $} GNodesCount
        {MakeStringWithPercent "K nodes count" KNodesCount NodesCount $} KNodesCount
        
        "Stack depth" StackDepth
        "Nodes count" NodesCount
      ]}
    end
  end

  proc {HandleOption Option Identities}
    case Identities of nil then skip
    [] Id|NextIdentities then
      Thread = {GetThreadFromId Id $}
    in
      if Thread \= none then
        case Option of state then
          {DisplayState Id Thread}
        [] statistics then
          {DisplayStatistics Id Thread}
        [] nodes then
          {DisplayNodes Id Thread}
        [] all then
          {DisplayState Id Thread}
          {DisplayStatistics Id Thread}
          {DisplayNodes Id Thread}
        end
      else
        {PrintThreadNotFoundError Id}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      Identities = {ExtractIdentities NextArguments $}
    in
      {HandleOption state Identities}
    [] "statistics" then
      Identities = {ExtractIdentities NextArguments $}
    in
      {HandleOption statistics Identities}
    [] "nodes" then
      Identities = {ExtractIdentities NextArguments $}
    in
      {HandleOption nodes Identities}
    else
      {PrintInvalidOptionError Argument}
    end
  end
end