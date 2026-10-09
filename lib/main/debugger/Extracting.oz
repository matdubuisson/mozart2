local
  X = unit
in
  proc {ExtractIdentities Arguments ?Identities}
    case Arguments of nil then Identities = nil
    [] Argument|NextArguments then
      NewIdentities
    in
      if {String.isInt Argument $} then
        Identities = {String.toInt Argument $}|NewIdentities
      else
        Id = {Boot_Identity.getIdFromName Argument $}
      in
        if Id \= none then
          Identities = Id|NewIdentities
        else
          Identities = NewIdentities
          {PrintError "Name '"#Argument#"' was referenced to no id"}
        end
      end
      {ExtractIdentities NextArguments NewIdentities}
    end
  end
end