local
  X = unit
in
  proc {ExtractArguments Command Arguments ?Which ?What ?Who}
    proc {ReturnError}
      Which = none What = none Who = none
    end
  in
    case Arguments of Wi|Wa|Wo then
      if {List.member Wi WhichList $} then
        if {List.member Wa WhatList $} then
        
        else
          {PrintWrongArgumentError Wa 2 WhatList}
          {ReturnError}
        end
      else
        {PrintWrongArgumentError Wi 1 WhichList}
        {ReturnError}
      end
    else
      {PrintError "Command '"#Command#"'' must be composed with "#
        "at least 3 arguments"#TRYHELP}
      {ReturnError}
    end
  end
end