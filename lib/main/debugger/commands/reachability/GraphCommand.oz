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
    Graph = {Boot_Introspection.getReachabilityGraph $}
  in
    case Graph of graph(
      threadToVariables: T2VGraph
      threadsStatus: T2Status
      variableToThreads: V2TGraph
      variablesStatus: V2Status
    ) then
      proc {GetRoots Keys ?Lst}
        case Keys of nil then Lst = nil
        [] Key|NextKeys then
          NewLst
        in
          if T2Status.Key.runnable then Lst = Key|NewLst
          else Lst = NewLst end
          {GetRoots NextKeys NewLst}
        end
      end

      proc {InitQueue Roots AccumulatedCount ?Count ?Queue QueueTail}
        case Roots of nil then
          Count = AccumulatedCount
          Queue = QueueTail
        [] Root|NextRoots then
          NewAccumulatedCount = AccumulatedCount + 1
          NewQueue
        in
          Queue = pair(Root Root|nil)|NewQueue
          {InitQueue NextRoots NewAccumulatedCount Count NewQueue QueueTail}
        end
      end

      proc {PrintPath Path}
        case Path of LastThreadKey|nil then {PrintLn ""#LastThreadKey}
        [] ThreadKey|VariableKey|NextPath then
          {Print ThreadKey#"--("#VariableKey#")-->"}
          {PrintPath NextPath}
        end
      end

      proc {ForEachNeighbor NewPath Neighbors AccumulatedCount ?Count ?Tail EndTail}
        case Neighbors of nil then
          Count = AccumulatedCount
          Tail = EndTail
        [] Neighbor|NextNeighbors then
          Pair = pair(Neighbor Neighbor|NewPath)
          NewAccumulatedCount = AccumulatedCount + 1
          NewTail
        in
          % {Boot_System.printRepr Pair false true}
          
          Tail = Pair|NewTail
          {ForEachNeighbor NewPath NextNeighbors NewAccumulatedCount Count NewTail EndTail}
        end
      end

      proc {ForEachDataflowLink Key Path Links AccumulatedCount ?Count ?Tail EndTail}
        case Links of nil then
          Count = AccumulatedCount
          Tail = EndTail
        [] Link|NextLinks then
          NewPath = Link|Path
          Neighbors = V2TGraph.Link
          IterationCount
          NewTail
        in
          % {Boot_System.printRepr neighbors(Neighbors) false true}

          {ForEachNeighbor NewPath Neighbors 0 IterationCount Tail NewTail}

          local NewAccumulatedCount = (AccumulatedCount + IterationCount) in
            {ForEachDataflowLink Key Path NextLinks NewAccumulatedCount Count NewTail EndTail}
          end
        end
      end

      proc {BFS Marks Queue QueueSize ?QueueTail}
        if QueueSize == 0 then QueueTail = nil
        elsecase Queue of pair(Key Path)|NextQueue then
          if {Value.condSelect Marks Key false $} then
            NextQueueSize = QueueSize - 1
          in
            {BFS Marks NextQueue NextQueueSize QueueTail}
          else
            Links = T2VGraph.Key
            Count
            NewQueueTail
          in
            {Print "Found "#Key#" at "}
            {PrintPath {List.reverse Path $}}

            % {Boot_System.printRepr links(Links) false true}

            {ForEachDataflowLink Key Path Links 0 Count QueueTail NewQueueTail}

            local
              NewMarks = {Record.adjoin Marks marks(Key:true) $}
              NextQueueSize = (QueueSize - 1 + Count)
            in
              {BFS NewMarks NextQueue NextQueueSize NewQueueTail}
            end
          end
        end
      end

      Keys = {Record.arity T2VGraph $}
      Roots = {GetRoots Keys $}
    in
      {PrintLn "Roots: "#{ListToString Roots $}}

      local
        InitialQueue
        InitialQueueTail
        InitialQueueSize
      in
        {InitQueue Roots 0 InitialQueueSize InitialQueue InitialQueueTail}
        {BFS marks() InitialQueue InitialQueueSize InitialQueueTail}
      end
    end
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