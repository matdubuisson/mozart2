local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["<id0> .... <idN>"]
      [
        "display the state of one or several thread(s) specified by their id"
      ]}
  end

  proc {DisplayThreadRegisters Id Thread}
    VMNodesCount = {Boot_Introspection.getNodesCount $}
    NodesCount = {Boot_Introspection.getThreadNodesCount Thread $}
    StackDepth = {Boot_Introspection.getThreadStackDepth Thread $}

    proc {PrintRegister Type Depth Size}
      {PrintInfo TAB#Type#
        TAB#Depth#"/"#(StackDepth - 1)#
        TAB#Size#
        TAB#{GetPercentString Size NodesCount $}#
        TAB#{GetPercentString Size VMNodesCount $}}
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

        {PrintStack (Depth + 1)}
      end
    end
  in
    {PrintInfo "Thread "#Id#":"}
    {PrintInfo TAB#"TYPE"#
      TAB#"DEPTH"#
      TAB#"COUNT"#
      TAB#"PERCENT"#
      TAB#"VM-PERCENT"}

    {PrintRegister x 0
      {Boot_Introspection.getThreadXNodesRegisterSize Thread $}}
    {PrintStack 0}
  end

  proc {Execute I Id}
    if {ValidId Id $} then
      Thread = {GetThreadFromId Id $}
    in
      if Thread \= none then
        {DisplayThreadRegisters Id Thread}
      end
    else {PrintInvalidIdError I} end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] "help"|_ then
    {DisplayOptions}
  else
    Ids = {ExtractInputs int Arguments none $}
  in
    {ForEachI Ids Execute}
  end
end