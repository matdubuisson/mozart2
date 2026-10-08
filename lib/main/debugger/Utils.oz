proc {ExtractString Argument ?String}
  if {Not {IsDet Argument}} then
    String = "_"
  elseif {Bool.is Argument $} then
    String = {Bool.toString Argument $}
  elseif {Int.is Argument $} then
    String = {Int.toString Argument $}
  elseif {Float.is Argument $} then
    String = {Float.toString Argument $}
  elseif {Atom.is Argument $} then
    String = {Atom.toString Argument $}
  elseif {String.is Argument $} then
    String = Argument
  else
    String = none
    {PrintError "Invalid data provided for string extraction"}
  end
end

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

proc {ExtractInputsWithError Type Arguments DefaultValue ?Error ?Result}
  case Arguments of nil then
    Error = false
    Result = nil
  [] Argument|NextArguments then
    Value = {ExtractInput Type Argument DefaultValue $}
    NextResult
  in
    Result = Value|NextResult

    if Value == DefaultValue then
      Error = true
      NextResult = nil
    else
      {ExtractInputsWithError Type NextArguments DefaultValue Error NextResult}
    end
  end
end

% proc {ExtractOneInput Type Arguments DefaultValue ?Result}
%   case Arguments of nil then Result = DefaultValue
%   [] Argument|NextArguments then
%     Result = {ExtractInput Type Argument DefaultValue $}
%   end
% end

proc {ExtractSomething Something Type Arguments ?Result}
  case Arguments of nil then
    Result = none
    {PrintError "Argument '"#Something#
      "' takes a "#Type#" as parameter but nothing was provided"}
  [] Argument|NextArguments then
    Result = {ExtractInput Type Argument none $}
    
    if Result == none then
      {PrintError "Invalid parameter '"#Argument#"' provided to '"#
        Something#"' argument"}
    end
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

proc {ForEachI List Execute}
  proc {Loop I List}
    case List of nil then skip
    [] Head|Tail then
      {Execute I Head}
      {Loop (I + 1) Tail}
    end
  end
in
  {Loop 0 List}
end

proc {For2EachI List List2 Execute}
  proc {Loop I List List2}
    case List#List2 of nil#nil then skip
    [] (Head|Tail)#(Head2|Tail2) then
      {Execute I Head Head2}
      {Loop (I + 1) Tail Tail2}
    end
  end
in
  {Loop 0 List List2}
end

proc {ValidId Id ?Result}
  Result = (Id \= none andthen {Int.is Id $})
end

proc {GetThreadFromId Id ?Result}
  Thread = {Boot_Introspection.getThread Id $}
in
  if Thread == none then
    Result = none
    {PrintThreadNotFoundError Id}
  else
    Result = Thread
  end
end

proc {GetVariableFromId Id ?Result}
  Variable = {Boot_Introspection.getVariable Id $}
in
  if Variable == none then
    Result = none
    {PrintVariableNotFoundError Id}
  else
    Result = Variable
  end
end

proc {GetPercent Count Scale ?Percent ?DotPercent}
  Percent = (Count * 100) div Scale
  DotPercent = (Count * 10000) mod Scale
end

proc {GetPercentString Count Scale ?String}
  Percent DotPercent
in
  {GetPercent Count Scale Percent DotPercent}
  String = Percent#"."#DotPercent#"%"
end

proc {ListToString ThisList ?String}
  String = ""
end

proc {ToString Something ?String}
  if {Bool.is Something $} then String = {Bool.toString Something $}
  elseif {Int.is Something $} then String = {Int.toString Something $}
  elseif {Float.is Something $} then String = {Float.toString Something $}
  elseif {Atom.is Something $} then String = {Atom.toString Something $}
  elseif {String.is Something $} then String = Something
  elseif {List.is Something $} then String = {ListToString Something $}
  else
    String = "<error>"
    {PrintError "Not type found for "#Something}
  end
end