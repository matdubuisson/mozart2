local
  proc {PrintCount Message Count}
    String = {Int.toString Count $}
  in
    {PrintLog "\t=> "#Message#": "#String}
  end

  Options = [
    option("threads" "display counters about threads")
    option("variables" "display counters about variables")
    option("nodes" "display counters about nodes")
  ]
in
  case Arguments of "threads"|SelectedOptionNames then
    proc {PrintActiveThreadsCount}
      {PrintCount
        "Active threads count"
        {Boot_Introspection.getActiveThreadsCount $}}
    end

    proc {PrintPassiveThreadsCount}
      {PrintCount
        "Passive threads count"
        {Boot_Introspection.getPassiveThreadsCount $}}
    end

    proc {PrintThreadsCount}
      {PrintCount
        "Threads count"
        {Boot_Introspection.getThreadsCount $}}
    end

    Options = [
      option("active" "display active threads counter"
        PrintActiveThreadsCount)
      option("passive" "display passive threads counter"
        PrintPassiveThreadsCount)
      option("all" "display threads counter"
        PrintThreadsCount)
    ]
  in
    {ExecuteOptions "threads" Options SelectedOptionNames}

  [] "nodes"|SelectedOptionNames then
    proc {PrintVariableNodesCount}
      {PrintCount "Variable nodes count"
        {Boot_Introspection.getVariableNodesCount $}}
    end

    proc {PrintValueNodesCount}
      {PrintCount "Value nodes count"
        {Boot_Introspection.getValueNodesCount $}}
    end

    proc {PrintStructuralNodesCount}
      {PrintCount "Structural nodes count"
        {Boot_Introspection.getStructuralNodesCount $}}
    end

    proc {PrintTokenNodesCount}
      {PrintCount "Token nodes count"
        {Boot_Introspection.getTokenNodesCount $}}
    end

    proc {PrintStableNodesCount}
      {PrintCount "Stable nodes count"
        {Boot_Introspection.getStableNodesCount $}}
    end

    proc {PrintUnstableNodesCount}
      {PrintCount "Unstable nodes count"
        {Boot_Introspection.getUnstableNodesCount $}}
    end

    proc {PrintXNodesCount}
      {PrintCount "X nodes count"
        {Boot_Introspection.getXNodesCount $}}
    end

    proc {PrintYNodesCount}
      {PrintCount "Y nodes count"
        {Boot_Introspection.getYNodesCount $}}
    end

    proc {PrintGNodesCount}
      {PrintCount "G nodes count"
        {Boot_Introspection.getGNodesCount $}}
    end

    proc {PrintKNodesCount}
      {PrintCount "K nodes count"
        {Boot_Introspection.getKNodesCount $}}
    end

    proc {PrintStackDepth}
      {PrintCount "Stack depth"
        {Boot_Introspection.getStackDepth $}}
    end

    proc {PrintNodesCount}
      {PrintCount "Nodes count"
        {Boot_Introspection.getNodesCount $}}
    end

    Options = [
      option("variableNodesCount" "display the counter of nodes behaving as variables"
        PrintVariableNodesCount)
      option("valueNodesCount" "display the counter of nodes behaving as values"
        PrintValueNodesCount)
      option("structuralNodesCount" "display the counter of nodes behaving as structures"
        PrintStructuralNodesCount)
      option("tokenNodesCount" "display the counter of nodes behaving as tokens"
        PrintTokenNodesCount)

      option("stableNodesCount" "display the stable nodes counter"
        PrintStableNodesCount)
      option("unstableNodesCount" "display the unstable nodes counter"
        PrintUnstableNodesCount)

      option("xNodesCount" "display the counter of nodes stored inside X registers"
        PrintXNodesCount)
      option("yNodesCount" "display the counter of nodes stored inside Y registers"
        PrintYNodesCount)
      option("gNodesCount" "display the counter of nodes stored inside G registers"
        PrintGNodesCount)
      option("kNodesCount" "display the counter of nodes stored inside K registers"
        PrintKNodesCount)

      option("stackDepth" "display the depth of YGK registers, for several threads it is a sum of all"
        PrintStackDepth)

      option("nodesCount" "display the total count of nodes from anywhere and of any type"
        PrintNodesCount)
    ]
  in
    {ExecuteOptions "nodes" Options SelectedOptionNames}

  [] "variables"|SelectedOptionNames then
    proc {PrintBoundVariablesCount}
      {PrintCount "Bound variables"
        {Boot_Introspection.getBoundVariablesCount $}}
    end

    proc {PrintUnboundVariablesCount}
      {PrintCount "Unbound variables"
        {Boot_Introspection.getUnBoundVariablesCount $}}
    end

    proc {PrintVariablesCount}
      {PrintCount "Variables"
        {Boot_Introspection.getVariablesCount $}}
    end

    Options = [
      option("boundVariablesCount" "display the number of bound variables (still garbage collected yet)"
        PrintBoundVariablesCount)
      option("unboundVariablesCount" "display the number of unbound variables (still garbage collected yet)"
        PrintUnboundVariablesCount)
      option("variablesCount" "display the number of variables (still garbage collected yet)"
        PrintVariablesCount)
    ]
  in
    {ExecuteOptions "variables" Options SelectedOptionNames}

  [] "help"|_ then
    {PrintOptions "count" Options false}
  else
    {PrintOptions "count" Options true}
  end
end