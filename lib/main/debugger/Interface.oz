proc {ExtractInput Type Argument DefaultValue ?Result}
  proc {Convert Type F Argument ?Value}
    try
      Value = {F Argument $}
    catch _ then
      Value = DefaultValue
    end
  end
in
  case Type of atom then
    Result = {String.toAtom Argument $}
  [] string then
    Result = Argument
  [] bool then
    Result = {Convert Type proc {$ V ?R}
      case V of "true" then R = true
      [] "false" then R = false
      else R = DefaultValue end
    end Argument $}
  [] int then
    Result = {Convert Type String.toInt Argument $}
  [] float then
    Result = {Convert Type String.toFloat Argument $}
  end
end

proc {ExtractInputs Type Arguments DefaultValue ?Result}
  case Arguments of nil then Result = nil
  [] Argument|NextArguments then
    Value = {ExtractInput Type Argument DefaultValue $}
    NextResult
  in
    Result = Value|NextResult
    {ExtractInputs Type NextArguments DefaultValue NextResult}
  end
end

proc {ExtractFromTo Arguments DefaultFrom DefaultTo ?From ?To}
  case Arguments of nil then
    From = DefaultFrom
    To = DefaultTo
  [] Argument|NextArguments then
    F = {ExtractInput int Argument none $}
  in
    if F == none then
      From = DefaultFrom
      {PrintError "Invalid 'from' index '"#Argument#"', it must be an integer"}
    else
      From = F
    end

    case NextArguments of nil then To = DefaultTo
    [] NextArgument|_ then
      T = {ExtractInput int NextArgument none $}
    in
      if T == none then
        To = DefaultTo
        {PrintError "Invalid 'to' index '"#NextArgument#"', it must be an integer"}
      else
        To = T
      end
    end
  end
end