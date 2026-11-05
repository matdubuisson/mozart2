local
  proc {DisplayOptions}
    {DisplayFrame "Graph command options" [
      "t2v" "display the bipartite dataflow graph from threads to variables"
      "v2t" "display the bipartite dataflow graph from variables to threads"
    ]}
  end

  %% By key or link or neighbor it means the unique id provided to threads or variables
  %% Key stands for source thread id, neighbor for destination thread id and link for a variable id
  %% linking two threads source and destination

  proc {DisplayGraphAux PrimaryGraph SecondaryGraph DisplayKey DisplayKeyValue}
    Keys = {Record.arity PrimaryGraph $}

    proc {ForEach Keys}
      case Keys of nil then skip
      [] Key|NextKeys then
        Values = PrimaryGraph.Key

        proc {ForEachEach Values}
          case Values of nil then skip
          [] Value|NextValues then
            KeysSubset = SecondaryGraph.Value
          in
            {DisplayKeyValue Key Value KeysSubset}
            {ForEachEach NextValues}
          end
        end
      in
        {DisplayKey Key}
        {ForEachEach Values}
        {ForEach NextKeys}
      end
    end
  in
    {ForEach Keys}
  end

  proc {DisplayGraph IsT2V}
    Graph = {Boot_Introspection.getReachabilityGraph $}
  in
    case Graph of graph(
      threadToVariables: T2VGraph
      threadsStatus: T2Status
      variableToThreads: V2TGraph
      variablesStatus: V2Status
    ) then
      if IsT2V then
        {DisplayGraphAux T2VGraph V2TGraph
          proc {$ Key}
            {Print "Thread "#Key}

            case T2Status.Key of state(
              id: _
              kindId: _
              generationId: _

              abstraction: _
              priority: _

              runnable: Runnable
              terminated: Terminated
              dead: Dead
              preempted: Preempted
              preemptible: Preemptible
            ) then
              if Runnable then {Print " (runnable)"} end
              if Terminated then {Print " (terminated)"} end
              if Dead then {Print " (dead)"} end
              if Preempted then {Print " (preempted)"} end
              if Preemptible then {Print " (preemptible)"} end
            end
            
            {PrintLn ""}
          end
          proc {$ Key Value KeysSubset}
            {PrintLn TAB#"- Variable "#Value#": ["#
              {ListToString KeysSubset $}#"]"}
          end}
      else
        {DisplayGraphAux V2TGraph T2VGraph
          proc {$ Key}
            {Print "Variable "#Key}

            case V2Status.Key of state(
              isNeeded: IsNeeded
              isBound: IsBound
            ) then
              if IsNeeded then {Print " (needed)"} end
              if IsBound then {Print " (bound)"} end
            end

            {PrintLn ""}
          end
          proc {$ Key Value KeysSubset}
            {PrintLn TAB#"- Thread "#Value#": ["#
              {ListToString KeysSubset $}#"]"}
          end}
      end
    end
  end

  proc {DisplayReachability}
    proc {DisplayReachables Reachables}
      case Reachables of nil then skip
      [] pair(Key Path)|NextReachables then
        {Print "Reacheable thread "#Key#": "}
        {PrintReachabilityPath Path}
        {DisplayReachables NextReachables}
      end
    end

    Marks Reachables
  in
    {ComputeReachability Marks Reachables} % From Reachability.oz
    {PrintInfo "There are "#{List.length Reachables $}#" reachable threads"}
    {DisplayCSV [
        "Key     "
        flexible("Path                                                                ")
      ]
      Reachables
      proc {$ Pair ?Result} Result = Pair end
      proc {$ Pair ?Result} Result = true end
      proc {$ Pair ?Result}
        case Pair of pair(Key Path) then
          Result = [
            {Int.toString Key $}
            {ReachabilityPathToString Path $}
          ]
        end
      end}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "t2v" then
      {DisplayGraph true}
    [] "v2t" then
      {DisplayGraph false}
    [] "reachability" then
      {DisplayReachability}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end