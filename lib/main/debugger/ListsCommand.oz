local
  proc {DisplayMainOptions}
    {PrintInfo "lists help info + details here"}
  end

  proc {HandleOption}
    Lists = {Boot_Introspection.getLists $}
  in
    {MaskedDisplayCSV
      ["Id" "Hash" "Repr"]
      Lists 10 FormatList
      [true true false]}
  end
in
  case Arguments of nil then
    {DisplayMainOptions}
  [] "help"|_ then
    {DisplayMainOptions}
  else
    {HandleOption}
  end
end