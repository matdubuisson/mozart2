local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["variable" "structural" "token" "value" "all"]
      [
        "display all nodes with variable behavior variable"
        "display all nodes with structural behavior structural"
        "display all nodes with token behavior token"
        "display all nodes with value behavior value"
        "display all nodes with any structural behavior"
      ]}
  end

  proc {HandleOption Type Arguments}
    From To
    Nodes
  in
    {ExtractFromTo Arguments 0 100 From To}

    local
      Nodes = {Boot_Introspection.getNodes Type From To}
    in
      {MaskedDisplayCSV
        ["Id" "Name" "BindingPriority" "StructuralBehavior" "Copyable" "Transient" "Feature" "UUID" "Value"]
        Nodes 20 FormatNode
        [true true true true true true true true false]}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "all" then
      {HandleOption all NextArguments}
    [] "variable" then
      {HandleOption variable NextArguments}
    [] "structural" then
      {HandleOption structural NextArguments}
    [] "token" then
      {HandleOption token NextArguments}
    [] "value" then
      {HandleOption value NextArguments}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end
