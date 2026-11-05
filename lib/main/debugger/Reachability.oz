proc {ReachabilityPathToString Path ?String}
  proc {Aux Path AccumulatedString}
    case Path of LastThreadKey|nil then
      String = {List.append AccumulatedString
        {Int.toString LastThreadKey $} $}
    [] ThreadKey|VariableKey|NextPath then
      Left =  {List.append
                {Int.toString ThreadKey $}
                "--("
                $}
      Right = {List.append
                {Int.toString VariableKey $}
                ")-->"
                $}
      Together = {List.append Left Right $}
      NewString = {List.append AccumulatedString Together $}
    in
      {Aux NextPath NewString}
    end
  end

  ReversedPath = {List.reverse Path $}
in
  {Aux ReversedPath ""}
end

proc {PrintReachabilityPath Path}
  String = {ReachabilityPathToString Path $}
in
  {PrintLn String}
end

proc {ComputeReachability ?Marks ?Reachables}
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

    proc {BFS Marks Queue QueueSize ?QueueTail ?ResultedMarks ?ReachableKeys}
      if QueueSize == 0 then
        QueueTail = nil
        ResultedMarks = Marks
        ReachableKeys = nil
      elsecase Queue of pair(Key Path)|NextQueue then
        if {Value.condSelect Marks Key false $} then
          NextQueueSize = QueueSize - 1
        in
          {BFS Marks NextQueue NextQueueSize QueueTail ResultedMarks ReachableKeys}
        else
          NewReachableKeys
          Links = T2VGraph.Key
          Count
          NewQueueTail
        in
          ReachableKeys = pair(Key Path)|NewReachableKeys

          % {Print {List.length Path $}#" => "#Key#": "}
          % {PrintLn {ReachabilityPathToString Path}}
          % {Boot_System.printRepr links(Links) false true}

          {ForEachDataflowLink Key Path Links 0 Count QueueTail NewQueueTail}

          local
            NewMarks = {Record.adjoin Marks marks(Key:true) $}
            NextQueueSize = (QueueSize - 1 + Count)
          in
            {BFS NewMarks NextQueue NextQueueSize NewQueueTail ResultedMarks NewReachableKeys}
          end
        end
      end
    end

    Keys = {Record.arity T2VGraph $}
    Roots = {GetRoots Keys $}
    InitialQueue
    InitialQueueTail
    InitialQueueSize
  in
    {InitQueue Roots 0 InitialQueueSize InitialQueue InitialQueueTail}
    {BFS marks() InitialQueue InitialQueueSize InitialQueueTail Marks Reachables}
  end
end