proc {PrintWrongArgumentError What Type Argument}
  {PrintError "Provided "#What#" '"#Argument#"' is not an "#Type#TRYHELP}
end

proc {PrintUnexpectedOptionError Argument}
  {PrintError "Unexpected option '"#Argument#"' provided"#TRYHELP}
end

proc {PrintIndexedWrongArgumentError Index What Type Argument}
  {PrintError "Provided "#Index#"th "#What#" '"#Argument#"' is not an "#Type#TRYHELP}
end

proc {PrintThreadNotFoundError Id}
  {PrintError "Thread "#Id#" does not exist"}
end
