local
  proc {DisplayOptions}
    {PrintInfo "<id0> .... <idN>\tdisplay the state of one or several thread(s) specified by their id"}
  end

  proc {DisplayThreadRegisters Thread Id}
    VMNodesCount = {Boot_Introspection.getNodesCount $}
    NodesCount = {Boot_Introspection.getThreadNodesCount Thread $}
    StackDepth = {Boot_Introspection.getThreadStackDepth Thread $}

    proc {GetPercentVersus Count Scale ?Result}
      Percent = (Count * 100) div Scale
      DotPercent = (Count * 10000) mod Scale
    in
      Result = Percent#"."#DotPercent#"%"
    end

    proc {PrintRegister Type Depth Size}
      {PrintLog "\t"#Type#
        "\t"#Depth#"/"#(StackDepth - 1)#
        "\t"#Size#
        "\t"#{GetPercentVersus Size NodesCount}#
        "\t"#{GetPercentVersus Size VMNodesCount}}
    end

    proc {PrintStack Depth}
      if Depth < StackDepth then
        YRegisterSize = {Boot_Introspection.getThreadYNodesRegisterSize
          Thread Depth $}
        GRegisterSize = {Boot_Introspection.getThreadGNodesRegisterSize
          Thread Depth $}
        KRegisterSize = {Boot_Introspection.getThreadKNodesRegisterSize
          Thread Depth $}
      in
        {PrintRegister y Depth YRegisterSize}
        {PrintRegister g Depth GRegisterSize}
        {PrintRegister k Depth KRegisterSize}

        {PrintStack Depth + 1}
      end
    end
  in
    {PrintLog "Thread "#Id#":"}
    {PrintLog "\tTYPE\tDEPTH\tCOUNT\tPERCENT\tVM-PERCENT"}

    {PrintRegister x 0
      {Boot_Introspection.getThreadXNodesRegisterSize Thread $}}
    {PrintStack 0}
  end

  proc {ForEach Arguments}
    case Arguments of nil then skip
    [] Argument|NextArguments then
      ThreadId = {ExtractInput int Argument none $}
    in
      if ThreadId == none then
        {PrintError "Provided thread id '"#Argument#"' is not an integer"}
      else
        Thread = {Boot_Introspection.getThread ThreadId $}
      in
        if Thread == none then
          {PrintError "Thread "#ThreadId#" does not exist"}
        else
          {DisplayThreadRegisters Thread ThreadId}
          {ForEach NextArguments}
        end
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  else
    {ForEach Arguments}
  end
end