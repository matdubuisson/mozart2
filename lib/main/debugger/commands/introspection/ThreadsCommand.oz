local
  % Common configuration for all aggregates

  proc {DisplayOptions}
    {DisplayFrame "Thread command options" [
      "state <condition>" "display the state of a selected set of threads"
      "statistics <condition>" "display statistics related to a selection of threads"
      "nodes <condition>" "display the state nodes proportions related to a selection of threads"
    ]}
  end

  proc {HandleStateOption Threads Conditions}
    {DisplayCSV
      [
        "Id      " "KindId  " "GenerationId" % Ids
        "Priority" % Importance
        "Runnable" "Terminated" "Dead " "Preempted" "Preemptible" % State
      ]
      Threads
      proc {$ Thread ?Record} Record = {Boot_Introspection.getThreadState Thread $} end
      proc {$ State ?Result} Result = {CheckConditions Conditions State $} end
      proc {$ State ?Result}
        case State of state(
          id: Id
          kindId: KindId
          generationId: GenerationId

          priority: Priority

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

            {Bool.toString Runnable $}
            {Bool.toString Terminated $}
            {Bool.toString Dead $}
            {Bool.toString Preempted $}
            {Bool.toString Preemptible $}
          ]
        end
      end}
  end

  proc {HandleStatisticsOption Threads Conditions}
    {DisplayCSV
      [
        "Id      "
        "RunsCount" "ResumesCount"
        "SuspendsCount" "SuspendsOnVarCount"
        "OperationsCount" "BindsCount"
      ]
      Threads
      proc {$ Thread ?Record} Record = {Boot_Introspection.getThreadStatistics Thread $} end
      proc {$ Statistics ?Result} Result = {CheckConditions Conditions Statistics $} end
      proc {$ Statistics ?Result}
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
      end}
  end

  proc {HandleNodesOption Threads Conditions}
    {DisplayCSV
      [
        "Id      "
        "Variables" "Values" "Structures" "Tokens" % Family
        "Stable" "Unstable" % Modifiable
        "X     " "Y     " "G     " "K     " % Type
        "StackDepth" "Total " % How many
      ]
      Threads
      proc {$ Thread ?Record} Record = {Boot_Introspection.getThreadNodesCounts Thread $} end
      proc {$ NodesCounts ?Result} Result = {CheckConditions Conditions NodesCounts $} end
      proc {$ NodesCounts ?Result}
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
      end}
  end

  proc {HandleOption Option Arguments}
    Conditions = {ExtractConditions Arguments $}
  in
    if Conditions \= none then
      Threads = {Boot_Introspection.getThreads 0 10000 $}
    in
      case Option of state then
        {HandleStateOption Threads Conditions}
      [] statistics then
        {HandleStatisticsOption Threads Conditions}
      [] nodes then
        {HandleNodesOption Threads Conditions}
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
      {HandleOption state NextArguments}
    [] "statistics" then
      {HandleOption statistics NextArguments}
    [] "nodes" then
      {HandleOption nodes NextArguments}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end