local
  proc {DisplayMainOptions}
    skip
  end

  proc {HandleOption Type Arguments}
    From To
    Nodes
  in
    {ExtractFromTo Arguments 0 100 From To}
    Nodes = {Boot_Introspection.getNodes Type From To}

    {MaskedDisplayCSV
      ["Name" "BindingPriority" "StructuralBehavior" "Copyable" "Transient" "Feature" "UUID" "Value"]
      Nodes 20 FormatNode
      [true true true true true true true false]}
  end
in
  case Arguments of nil then
    {DisplayMainOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayMainOptions}
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
