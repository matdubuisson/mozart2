local
  proc {DisplayOptions}
    {PrintInfo "<id0> .... <idN>\tdisplay the state of one or several thread(s) specified by their id"}
  end

  proc {ForEach I Arguments Ids}
    case Arguments#Ids of nil#nil then skip
    [] (Argument|NextArguments)#(Id|NextIds) then
      if Id == none then
        {PrintIndexedWrongArgumentError I "id" "integer" Argument}
      else
        Thread = {Boot_Introspection.getThread Id $}
      in
        if Thread == none then
          {PrintThreadNotFoundError Id}
        else
          {DisplayThread "" Thread}
        end
      end

      {ForEach I + 1 NextArguments NextIds}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] "help"|_ then
    {DisplayOptions}
  else
    Ids = {ExtractInputs int Arguments none $}
  in
    {ForEach 0 Arguments Ids}
  end
end