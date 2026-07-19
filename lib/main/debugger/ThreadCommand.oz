local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      ["<id0> .... <idN>"]
      [
        "display the state of one or several thread(s) specified by their id"
      ]}
  end

  proc {Execute I Id}
    if {ValidId Id $} then
      Thread = {GetThreadFromId Id $}
    in
      if Thread \= none then
        {DisplayThread "" Thread}
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