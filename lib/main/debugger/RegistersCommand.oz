local
  Options = [
    option("id <value>" "Taking one integer representing a thread id")
    option("ids <value0 .... valueN>" "Taking a chain of ids to display several threads")
  ]

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

  proc {FindAndDisplayThreadRegisters IdString}
    Id
  in
    try
      Id = {String.toInt IdString $}
    catch _ then
      Id = none
    end

    if Id == none then
      {PrintError "Bad provided id '"#IdString#"'"}
    else
      Th = {Boot_Introspection.getThread Id $}
    in
      if Th == none then
        {PrintError "Thread "#Id#" does not exist"}
      else
        {DisplayThreadRegisters Th Id}
      end
    end
  end
in
  case Arguments of Option|NextArguments then
    case Option of "id" then
      case NextArguments of IdString|_ then
        {FindAndDisplayThreadRegisters IdString}
      else
        {PrintError "Option id takes an integer value"}
      end
    [] "ids" then
      proc {ForEachIdString IdStrings}
        case IdStrings of nil then skip
        [] IdString|NextIdStrings then
          {FindAndDisplayThreadRegisters IdString}
          {ForEachIdString NextIdStrings}
        end
      end
    in
      {ForEachIdString NextArguments}
    [] "help" then
      {PrintOptions "threads" Options true}
    else
      {PrintOptions "threads" Options true}
    end
  else
    {PrintOptions "thread" Options false}
  end
end