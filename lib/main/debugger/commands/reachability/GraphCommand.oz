local
  proc {DisplayOptions}
    {DisplayFrame "Graph command options" [
      "t2v" "display the bipartite dataflow graph from threads to variables"
      "v2t" "display the bipartite dataflow graph from variables to threads"
    ]}
  end

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
      variableToThreads: V2TGraph
    ) then
      if IsT2V then
        {DisplayGraphAux T2VGraph V2TGraph
          proc {$ Key}
            {PrintLn "Thread "#Key}  
          end
          proc {$ Key Value KeysSubset}
            {PrintLn TAB#"- Variable "#Value#": ["#
              {ListToString KeysSubset $}#"]"}
          end}
      else
        {DisplayGraphAux V2TGraph T2VGraph
          proc {$ Key}
            {PrintLn "Variable "#Key}  
          end
          proc {$ Key Value KeysSubset}
            {PrintLn TAB#"- Thread "#Value#": ["#
              {ListToString KeysSubset $}#"]"}
          end}
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
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end