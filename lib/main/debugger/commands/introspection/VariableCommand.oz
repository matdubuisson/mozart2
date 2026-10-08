local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["<id0> .... <idN>"]
      [
        "display the state of one or several variable(s) specified by their id"
      ]}
  end

  proc {Execute I Id}
    if {ValidId Id $} then
      Variable = {GetVariableFromId Id $}
    in
      if Variable \= none then
        {DisplayVariable "" Variable}
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