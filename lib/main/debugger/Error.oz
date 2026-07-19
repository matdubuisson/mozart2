proc {PrintInvalidIdError Index}
  {PrintError "Invalid provided "#{Int.toString Index $}#"th id"}
end

proc {PrintMissingArgumentError What Type}
  {PrintError "Provided "#What#" requires a parameter of type "#Type#TRYHELP}
end

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
  {PrintError "Thread "#Id#" does not exist or has been garbage collected"}
end

proc {PrintVariableNotFoundError Id}
  {PrintError "Variable "#Id#" does not exist or has been garbage collected"}
end
