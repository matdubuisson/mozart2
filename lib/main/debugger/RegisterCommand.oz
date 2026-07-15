local
  proc {DisplayOptions}
    {PrintInfo "x <id> (<from> <to>)\tdisplay content of the x register of the specified thread"}
    {PrintInfo "y <id> <depth=0> (<from> <to>)\tdisplay content of the y register of the specified thread at the specified depth"}
    {PrintInfo "g <id> <depth=0> (<from> <to>)\tdisplay content of the g register of the specified thread at the specified depth"}
    {PrintInfo "k <id> <depth=0> (<from> <to>)\tdisplay content of the k register of the specified thread at the specified depth"}
  end

  proc {DisplayRegister Type Thread Depth From To}
    Size
    Nodes
  in
    local N in
      N = To - From
      {PrintInfo {Int.toString N $}#" selected nodes from register "#
        {Atom.toString Type $}#" of thread "#
        {Boot_Thread.getId Thread $}#" at depth "#
        {Int.toString Depth $}#" indexed from "#
        {Int.toString From $}#" to "#
        {Int.toString To $}}
    end

    case Type of x then
      Size = {Boot_Introspection.getThreadXNodesRegisterSize Thread $}
      Nodes = {Boot_Introspection.getThreadXNodes
        Thread From To}
    [] y then
      Size = {Boot_Introspection.getThreadYNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadYNodes
        Thread Depth From To}
    [] g then
      Size = {Boot_Introspection.getThreadGNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadGNodes
        Thread Depth From To}    
    [] k then
      Size = {Boot_Introspection.getThreadKNodesRegisterSize Thread Depth $}
      Nodes = {Boot_Introspection.getThreadKNodes
        Thread Depth From To}
    end

    local
      NodesCounts = {Boot_Introspection.getThreadNodesCounts Thread $}
      NodesCount = NodesCounts.nodesCount
      Percent = (Size * 100) div NodesCount
      DotPercent = (Size * 10000) mod NodesCount
    in
      {PrintLog "Size: "#Size#"/"#NodesCount#" nodes ("#Percent#"."#DotPercent#"%)"}
    end

    {MaskedDisplayCSV
      ["Name" "BindingPriority" "StructuralBehavior" "Copyable" "Transient" "Feature" "UUID" "Value"]
      Nodes 20 FormatNode
      [true true true true true true true false]}
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
        Depth From To
      in
        case Type of x then
          Depth = 0
          {ExtractFromTo NextArguments 0 100 From To}
        else
          case NextArguments of nil then
            Depth = 0
            From = 0
            To = 100
          [] DepthString|NextArguments then
            Depth = {ExtractInput int DepthString none $}

            if Depth == none then
              {PrintError "Provided second parameter stack depth '"#DepthString#"' is not an integer"}
            end

            {ExtractFromTo NextArguments 0 100 From To}
          end
        end
        
        if Depth \= none then
          Thread = {Boot_Introspection.getThread ThreadId $}
          MaxDepth = {Boot_Introspection.getThreadStackDepth Thread $}
        in
          if Depth >= MaxDepth then
            {PrintError "Invalid depth "#Depth#", maximal depth is "#MaxDepth}
          else
            {DisplayRegister Type Thread Depth From To}
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