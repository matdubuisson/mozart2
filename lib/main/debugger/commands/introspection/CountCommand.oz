local
  % Aggregate options

  proc {DisplayOptions}
    {DisplayFrame "Count command options" [
      "vm <aggregates>" "display counters about the virtual machine"
      "threads <aggregates>" "display counters about the threads"
      "variables <aggregates>" "display counters about the variables"
      "nodes <aggregates>" "display counters about the nodes"
    ]}
  end

  % VM aggregates

  proc {DisplayVMOptions}
    {DisplayFrame "Count threads aggregates" [
      "schedules (<nextAggregates>)" "display the total schedules counter"
      "operations (<nextAggregates>)" "display the total executed operations counter"
      "systemSchedules (<nextAggregates>)" "display the total schedules counter"
      "systemOperations (<nextAggregates>)" "display the total executed operations counter"
      "gcSchedules (<nextAggregates>)" "display the total number of garbage collections done"
      "all" "display all VM counters"
    ]}
  end

  proc {GetVMCountsFrame Arguments ?Frame}
    proc {GetSchedulesCount Frame NewFrame}
      Frame = "Schedules count"|{Boot_Introspection.getSchedulesCount $}|NewFrame
    end

    proc {GetOperationsCount Frame NewFrame}
      Frame = "Operations count"|{Boot_Introspection.getOperationsCount $}|NewFrame
    end

    proc {GetSystemSchedulesCount Frame NewFrame}
      Frame = "System schedules count"|{Boot_Introspection.getSystemSchedulesCount $}|NewFrame
    end

    proc {GetSystemOperationsCount Frame NewFrame}
      Frame = "System operations count"|{Boot_Introspection.getSystemOperationsCount $}|NewFrame
    end

    proc {GetGCExecutionsCount Frame NewFrame}
      Frame = "GC schedules count"|{Boot_Introspection.getGCSchedulesCount $}|NewFrame
    end
  in
    case Arguments of nil then
      Frame = nil
    [] Argument|NextArguments then
      case Argument of "schedules" then NewFrame in
        {GetSchedulesCount Frame NewFrame}
        {GetVMCountsFrame NextArguments NewFrame}
      [] "operations" then NewFrame in
        {GetOperationsCount Frame NewFrame}
        {GetVMCountsFrame NextArguments NewFrame}
      [] "systemSchedules" then NewFrame in
        {GetSchedulesCount Frame NewFrame}
        {GetVMCountsFrame NextArguments NewFrame}
      [] "systemOperations" then NewFrame in
        {GetOperationsCount Frame NewFrame}
        {GetVMCountsFrame NextArguments NewFrame}
      [] "gcSchedules" then NewFrame in
        {GetGCExecutionsCount Frame NewFrame}
        {GetVMCountsFrame NextArguments NewFrame}
      [] "all" then A B C D E in
        {GetSchedulesCount Frame A}
        {GetOperationsCount A B}
        {GetSystemSchedulesCount B C}
        {GetSystemOperationsCount C D}
        {GetGCExecutionsCount D nil}
      else
        Frame = nil
        {PrintError "Unexpected vm counter '"#Argument#"'"#TRYHELP}
      end
    end
  end

  % Threads aggregates

  proc {DisplayThreadsOptions}
    {DisplayFrame "Count threads aggregates" [
      "active" "display active threads counter"
      "passive" "display passive threads counter"
      "total" "display total threads counter"
      "all" "display all threads counters"
    ]}
  end
  
  proc {GetThreadsCountsFrame Arguments ?Frame}
    proc {GetActiveThreadsCount ?Frame NewFrame}
      Frame = "Active threads count"|{Boot_Introspection.getActiveThreadsCount $}|NewFrame
    end

    proc {GetPassiveThreadsCount ?Frame NewFrame}
      Frame = "Passive threads count"|{Boot_Introspection.getPassiveThreadsCount $}|NewFrame
    end

    proc {GetThreadsCount ?Frame NewFrame}
      Frame = "Threads count"|{Boot_Introspection.getThreadsCount $}|NewFrame
    end
  in
    case Arguments of nil then
      Frame = nil
    [] Argument|NextArguments then
      case Argument of "active" then NewFrame in
        {GetActiveThreadsCount Frame NewFrame}
        {GetThreadsCountsFrame NextArguments NewFrame}
      [] "passive" then NewFrame in
        {GetActiveThreadsCount Frame NewFrame}
        {GetThreadsCountsFrame NextArguments NewFrame}
      [] "total" then NewFrame in
        {GetActiveThreadsCount Frame NewFrame}
        {GetThreadsCountsFrame NextArguments NewFrame}
      [] "all" then A B in
        {GetActiveThreadsCount Frame A}
        {GetPassiveThreadsCount A B}
        {GetThreadsCount B nil}
      else
        Frame = nil
        {PrintError "Unexpected threads counter '"#Argument#"'"#TRYHELP}
      end
    end
  end

  % Variables aggregates

  proc {DisplayVariablesOptions}
    {DisplayFrame "Count variables aggregates" [
      "bound" "display the number of bound variables"
      "unbound" "display the number of unbound variables"
      "total" "display the number of variables"
      "all" "display every counter related to variables"
    ]}
  end

  proc {GetVariablesCountsFrame Arguments ?Frame}
    proc {GetBoundVariablesCount ?Frame NewFrame}
      Frame = "Bound variables"|{Boot_Introspection.getBoundVariablesCount $}|NewFrame
    end

    proc {GetUnboundVariablesCount ?Frame NewFrame}
      Frame = "Unbound variables"|{Boot_Introspection.getUnBoundVariablesCount $}|NewFrame
    end

    proc {GetVariablesCount ?Frame NewFrame}
      Frame = "Variables"|{Boot_Introspection.getVariablesCount $}|NewFrame
    end
  in
    case Arguments of nil then
      Frame = nil
    [] Argument|NextArguments then
      case Argument of "bound" then NewFrame in
        {GetBoundVariablesCount Frame NewFrame}
        {GetVariablesCountsFrame NextArguments NewFrame}
      [] "unbound" then NewFrame in
        {GetUnboundVariablesCount Frame NewFrame}
        {GetVariablesCountsFrame NextArguments NewFrame}
      [] "total" then NewFrame in
        {GetVariablesCount Frame NewFrame}
        {GetVariablesCountsFrame NextArguments NewFrame}
      [] "all" then A B in
        {GetBoundVariablesCount Frame A}
        {GetUnboundVariablesCount A B}
        {GetVariablesCount B nil}
      else
        Frame = nil
        {PrintError "Unexpected variables counter '"#Argument#"'"#TRYHELP}
      end
    end
  end

  % Nodes aggregates

  proc {DisplayNodesOptions}
    {DisplayFrame "Count nodes aggregates" [
      "variable" "display the counter of nodes behaving as variables"
      "value" "display the counter of nodes behaving as values"
      "structural" "display the counter of nodes behaving as structures"
      "token" "display the counter of nodes behaving as tokens"
      "stable" "display the stable nodes counter"
      "unstable" "display the unstable nodes counter"
      "x" "display the counter of nodes stored inside X registers"
      "y" "display the counter of nodes stored inside Y registers"
      "g" "display the counter of nodes stored inside G registers"
      "k" "display the counter of nodes stored inside K registers"
      "stackDepth" "display the depth of YGK registers, for several threads it is a sum of all"
      "total" "display the total count of nodes from anywhere and of any type"
      "all" "display every counter related to nodes"
    ]}
  end

  proc {GetNodesCountsFrame Arguments ?Frame}
    proc {GetVariableNodesCount Frame NewFrame}
      Frame = "Variable nodes count"|{Boot_Introspection.getVariableNodesCount $}|NewFrame
    end

    proc {GetValueNodesCount Frame NewFrame}
      Frame = "Value nodes count"|{Boot_Introspection.getValueNodesCount $}|NewFrame
    end

    proc {GetStructuralNodesCount Frame NewFrame}
      Frame = "Structural nodes count"|{Boot_Introspection.getStructuralNodesCount $}|NewFrame
    end

    proc {GetTokenNodesCount Frame NewFrame}
      Frame = "Token nodes count"|{Boot_Introspection.getTokenNodesCount $}|NewFrame
    end

    proc {GetStableNodesCount Frame NewFrame}
      Frame = "Stable nodes count"|{Boot_Introspection.getStableNodesCount $}|NewFrame
    end

    proc {GetUnstableNodesCount Frame NewFrame}
      Frame = "Unstable nodes count"|{Boot_Introspection.getUnstableNodesCount $}|NewFrame
    end

    proc {GetXNodesCount Frame NewFrame}
      Frame = "X nodes count"|{Boot_Introspection.getXNodesCount $}|NewFrame
    end

    proc {GetYNodesCount Frame NewFrame}
      Frame = "Y nodes count"|{Boot_Introspection.getYNodesCount $}|NewFrame
    end

    proc {GetGNodesCount Frame NewFrame}
      Frame = "G nodes count"|{Boot_Introspection.getGNodesCount $}|NewFrame
    end

    proc {GetKNodesCount Frame NewFrame}
      Frame = "K nodes count"|{Boot_Introspection.getKNodesCount $}|NewFrame
    end

    proc {GetStackDepth Frame NewFrame}
      Frame = "Stack depth"|{Boot_Introspection.getStackDepth $}|NewFrame
    end

    proc {GetNodesCount Frame NewFrame}
      Frame = "Nodes count"|{Boot_Introspection.getNodesCount $}|NewFrame
    end
  in
    case Arguments of nil then
      Frame = nil
    [] Argument|NextArguments then
      case Argument of "variable" then NewFrame in
        {GetVariableNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "value" then NewFrame in
        {GetValueNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "structural" then NewFrame in
        {GetStructuralNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "token" then NewFrame in
        {GetTokenNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "stable" then NewFrame in
        {GetStableNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "unstable" then NewFrame in
        {GetUnstableNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "x" then NewFrame in
        {GetXNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "y" then NewFrame in
        {GetYNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "g" then NewFrame in
        {GetGNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "k" then NewFrame in
        {GetKNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "stackDepth" then NewFrame in
        {GetStackDepth Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "total" then NewFrame in
        {GetNodesCount Frame NewFrame}
        {GetNodesCountsFrame NextArguments NewFrame}
      [] "all" then A B C D E F G H I J K in
        {GetVariableNodesCount Frame A}
        {GetValueNodesCount A B}
        {GetStructuralNodesCount B C}
        {GetTokenNodesCount C D}
        {GetStableNodesCount D E}
        {GetUnstableNodesCount E F}
        {GetXNodesCount F G}
        {GetYNodesCount G H}
        {GetGNodesCount H I}
        {GetKNodesCount I J}
        {GetStackDepth J K}
        {GetNodesCount K nil}
      else
        Frame = nil
        {PrintError "Unexpected nodes counter '"#Argument#"'"#TRYHELP}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "vm" then
      case NextArguments of nil then {DisplayVMOptions}
      [] "help"|_ then {DisplayVMOptions}
      else
        {DisplayFrame "VM counters" {GetVMCountsFrame NextArguments $}}
      end
    [] "threads" then
      case NextArguments of nil then {DisplayThreadsOptions}
      [] "help"|_ then {DisplayThreadsOptions}
      else
        {DisplayFrame "Threads counters" {GetThreadsCountsFrame NextArguments $}}
      end
    [] "variables" then
      case NextArguments of nil then {DisplayVariablesOptions}
      [] "help"|_ then {DisplayVariablesOptions}
      else
        {DisplayFrame "Variables counters" {GetVariablesCountsFrame NextArguments $}}
      end
    [] "nodes" then
      case NextArguments of nil then {DisplayNodesOptions}
      [] "help"|_ then {DisplayNodesOptions}
      else
        {DisplayFrame "Nodes counters" {GetNodesCountsFrame NextArguments $}}
      end
    else
      {PrintError "Unexpected count option '"#Argument#"'"#TRYHELP}
    end
  end
end