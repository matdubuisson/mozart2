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