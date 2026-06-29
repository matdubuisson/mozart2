local
  proc {DisplayOptions}
    {PrintInfo "<id0> .... <idN>\tdisplay the stack depth of one or several thread(s) specified by their id"}
  end

  proc {FindAndDisplayThread IdString}
    Id
  in
    try
      Id = {String.toInt IdString $}
    catch _ then
      Id = none
    end

    if Id == none then
      {PrintError "Argument '"#IdString#"' is not an id"}
    else
      Th = {Boot_Introspection.getThread Id $}
    in
      if Th == none then
        {PrintError "Thread "#Id#" does not exist"}
      else
        {PrintInfo "Thread "#Id#" stack depth: "#
          {Boot_Introspection.getThreadStackDepth Th $}}
      end
    end
  end

  proc {ForEach Arguments}
    case Arguments of nil then skip
    [] Argument|NextArguments then
      {FindAndDisplayThread Argument}
      {ForEach NextArguments}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  else
    {ForEach Arguments}
  end
end