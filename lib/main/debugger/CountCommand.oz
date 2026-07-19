local
  proc {PrintCount Message Count}
    String = {Int.toString Count $}
  in
    {PrintLog "\t=> "#Message#": "#String}
  end

  % Aggregate options

  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["threads" "nodes" "variables"]
      [
        "display counters related to threads"
        "display counters related to nodes"
        "display counters related to variables"
      ]}
  end

  % Threads aggregates

  proc {DisplayThreadsOptions}
    {DisplayNameDescriptions
      ["active" "passive" "total" "all"]
      [
        "display active threads counter"
        "display passive threads counter"
        "display total threads counter"
        "display all threads counters"
      ]}
  end

  proc {HandleThreadsOption Arguments}
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
  in
    case Arguments of nil then
      {DisplayThreadsOptions}
    [] Argument|NextArguments then
      case Argument of "help" then 
        {DisplayThreadsOptions}
      [] "active" then
        {PrintActiveThreadsCount}
      [] "passive" then
        {PrintPassiveThreadsCount}
      [] "total" then
        {PrintThreadsCount}
      [] "all" then
        {PrintActiveThreadsCount}
        {PrintPassiveThreadsCount}
        {PrintThreadsCount}
      else
        {PrintError "Unexpected threads counter '"#Argument#"'"#TRYHELP}
      end

      if NextArguments \= nil then
        {HandleThreadsOption NextArguments}
      end
    end
  end

  % Nodes aggregates

  proc {DisplayNodesOptions}
    {DisplayNameDescriptions
      [
        "variable" "value" "structural" "token"
        "stable" "unstable"
        "x" "y" "g" "k"
        "stackDepth" "total" "all"
      ]
      [
        "display the counter of nodes behaving as variables"
        "display the counter of nodes behaving as values"
        "display the counter of nodes behaving as structures"
        "display the counter of nodes behaving as tokens"

        "display the stable nodes counter"
        "display the unstable nodes counter"

        "display the counter of nodes stored inside X registers"
        "display the counter of nodes stored inside Y registers"
        "display the counter of nodes stored inside G registers"
        "display the counter of nodes stored inside K registers"

        "display the depth of YGK registers, for several threads it is a sum of all"
        "display the total count of nodes from anywhere and of any type"
        "display every counter related to nodes"
      ]}
  end

  proc {HandleNodesOption Arguments}
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
  in
    case Arguments of nil then
      {DisplayNodesOptions}
    [] Argument|NextArguments then
      case Argument of "help" then 
        {DisplayNodesOptions}
      [] "variable" then
        {PrintVariableNodesCount}
      [] "value" then
        {PrintValueNodesCount}
      [] "structural" then
        {PrintStructuralNodesCount}
      [] "token" then
        {PrintTokenNodesCount}
      [] "stable" then
        {PrintStableNodesCount}
      [] "unstable" then
        {PrintUnstableNodesCount}
      [] "x" then
        {PrintXNodesCount}
      [] "y" then
        {PrintYNodesCount}
      [] "g" then
        {PrintGNodesCount}
      [] "k" then
        {PrintKNodesCount}
      [] "stackDepth" then
        {PrintStackDepth}
      [] "total" then
        {PrintNodesCount}
      [] "all" then
        {PrintVariableNodesCount}
        {PrintValueNodesCount}
        {PrintStructuralNodesCount}
        {PrintTokenNodesCount}
        {PrintStableNodesCount}
        {PrintUnstableNodesCount}
        {PrintXNodesCount}
        {PrintYNodesCount}
        {PrintGNodesCount}
        {PrintKNodesCount}
        {PrintStackDepth}
        {PrintNodesCount}
      else
        {PrintError "Unexpected nodes counter '"#Argument#"'"#TRYHELP}
      end

      if NextArguments \= nil then
        {HandleNodesOption NextArguments}
      end
    end
  end

  % Variables aggregates

  proc {DisplayVariablesOptions}
    {DisplayNameDescriptions
      ["REMARK:" "bound" "unbound" "total" "all"]
      [
        "All garbage collected variables are not parts of the aggregates"

        "display the number of bound variables"
        "display the number of unbound variables"
        "display the number of variables"
        "display every counter related to variables"
      ]}
  end

  proc {HandleVariablesOption Arguments}
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
  in
    case Arguments of nil then
      {DisplayVariablesOptions}
    [] Argument|NextArguments then
      case Argument of "help" then 
        {DisplayVariablesOptions}
      [] "bound" then
        {PrintBoundVariablesCount}
      [] "unbound" then
        {PrintUnboundVariablesCount}
      [] "total" then
        {PrintVariablesCount}
      [] "all" then
        {PrintBoundVariablesCount}
        {PrintUnboundVariablesCount}
        {PrintVariablesCount}
      else
        {PrintError "Unexpected variables counter '"#Argument#"'"#TRYHELP}
      end

      if NextArguments \= nil then
        {HandleVariablesOption NextArguments}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "threads" then
      {HandleThreadsOption NextArguments}
    [] "nodes" then
      {HandleNodesOption NextArguments}
    [] "variables" then
      {HandleVariablesOption NextArguments}
    else
      {PrintError "Unexpected counter option '"#Argument#"'"#TRYHELP}
    end
  end
end