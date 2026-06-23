local
  proc {DisplayOptions}
    {PrintInfo "x <id>\tdisplay content of the x register of the specified thread"}
    {PrintInfo "y <id> <depth=0>\tdisplay content of the y register of the specified thread at the specified depth"}
    {PrintInfo "g <id> <depth=0>\tdisplay content of the g register of the specified thread at the specified depth"}
    {PrintInfo "k <id> <depth=0>\tdisplay content of the k register of the specified thread at the specified depth"}
  end

  proc {DisplayNode Node}
    proc {GetPrintableByteString BString ?Result}
      Length = {ByteString.length BString $}
      proc {ForI I Acc ?Result}
        if I >= Length then Result = Acc
        else
          Value = {ByteString.get BString I $}
        in
          {ForI I + 1 Value + Acc Result}
        end
      end
    in
      Result = {ForI 0 0 $}
    end
  in
    case Node of node(
      name: Name
      bindingPriority: BindingPriority
      structuralBehavior: StructuralBehavior
      copyable: Copyable
      transient: Transient
      feature: Feature
      uuid: Uuid
      value: Value
    ) then
      {PrintLog Name#
        "\t"#BindingPriority#
        "\t"#StructuralBehavior#
        "\t"#{Bool.toString Copyable $}#
        "\t"#{Bool.toString Transient $}#
        "\t"#{Bool.toString Feature $}#
        "\t"#{Int.toString {GetPrintableByteString Uuid $} $}#
        "\t"#Value}
    end
  end

  proc {DisplayNodes Nodes}
    case Nodes of nil then skip
    [] Node|NextNodes then
      {DisplayNode Node}
      {DisplayNodes NextNodes}
    end
  end

  proc {DisplayRegister Type Thread Depth}
    Size
    Nodes
  in
    case Type of x then
      Size = {Boot_Introspection.getThreadXNodesRegisterSize Thread $}
      Nodes = {Boot_Introspection.getThreadXNodeAggregates
        Thread 0 Size state}
    [] y then
      Size = {Boot_Introspection.getThreadYNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadYNodeAggregates
        Thread Depth 0 Size state}
    [] g then
      Size = {Boot_Introspection.getThreadGNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadGNodeAggregates
        Thread Depth 0 Size state}    
    [] k then
      Size = {Boot_Introspection.getThreadKNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadKNodeAggregates
        Thread Depth 0 Size state}
    end

    local
      NodesCounts = {Boot_Introspection.getThreadNodesCounts Thread $}
      NodesCount = NodesCounts.nodesCount
      Percent = (Size * 100) div NodesCount
      DotPercent = (Size * 10000) mod NodesCount
    in
      {PrintLog "Size: "#Size#"/"#NodesCount#" nodes ("#Percent#"."#DotPercent#"%)"}
    end

    {PrintLog "Name"#
      "\tBP"#
      "\tSB"#
      "\tCopy"#
      "\tTrans"#
      "\tFeat"#
      "\tUuid"#
      "\tValue"}

    {DisplayNodes Nodes}
  end

  proc {HandleOption Type Arguments}
    case Arguments of nil then
      {PrintError "Missing thread id argument"#TRYHELP}
    [] ThreadIdString|NextArguments then
      ThreadId = {ExtractInput int ThreadIdString none $}
    in
      if ThreadId == none then
        {PrintError "Provided first parameter thread id '"#ThreadIdString#"' is not an integer"}
      else
        Depth
      in
        case Type of x then Depth = 0
        else
          case NextArguments of nil then Depth = 0
          [] DepthString|NextArguments then
            Depth = {ExtractInput int DepthString none $}

            if Depth == none then
              {PrintError "Provided second parameter stack depth '"#DepthString#"' is not an integer"}
            end
          end
        end
        
        if Depth \= none then
          Thread = {Boot_Introspection.getThread ThreadId $}
          MaxDepth = {Boot_Introspection.getThreadStackDepth Thread $}
        in
          if Depth >= MaxDepth then
            {PrintError "Invalid depth "#Depth#", maximal depth is "#MaxDepth - 1}
          else
            {DisplayRegister Type Thread Depth}
          end
        end
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "x" then
      {HandleOption x NextArguments}
    [] "y" then
      {HandleOption y NextArguments}
    [] "g" then
      {HandleOption g NextArguments}
    [] "k" then
      {HandleOption k NextArguments}
    else
      {PrintError "Unexpected register type '"#Argument#
        "' (must be either x, y, g or k)"#TRYHELP}
    end
  end
end