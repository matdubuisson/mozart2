local
  proc {DisplayOptions}
    {DisplayFrame "DebugInfo command options" [
      "abstractions <condition>" "displaying procedure descritpions including procedure name and all parameters' names"
      "variables <condition>" "showing mapping of ids to list of source names"
    ]}
  end

  proc {DisplayAbstractions}
    SourceNames = {Boot_DebugInfoManager.getSourceNames $}
    ThreadIds = {Record.arity SourceNames $}

    proc {ForEachStackFrame Frames}
      case Frames of nil then skip
      [] Frame|NextFrames then
        case Frame of sourceNamesMaps(
          abstractionsMap: AbstractionsMap
          variablesMap: _
        ) then
          ObjectIds = {Record.arity AbstractionsMap $}
        in
          {DisplayCSV
            [
              "Id      " "Type  "
              flexible("Source Name                     ")
              flexible("Filename                        ")
              "Line    " "Column  "
              flexible("Parameters' Source Names")
            ]
            ObjectIds
            proc {$ ObjectId ?Tuple} Tuple = ObjectId#AbstractionsMap.ObjectId end
            proc {$ Tuple ?Result} Result = true end
            proc {$ Tuple ?Result}
              case Tuple of ObjectId#(Head|Tail) then
                case Head of sourceName(
                  type: Type
                  name: Name
                  filename: Filename
                  line: Line
                  column: Column
                ) then
                  Result = [
                    {Int.toString ObjectId $}
                    {Atom.toString Type $}
                    {Atom.toString Name $}
                    {Atom.toString Filename $}
                    {Int.toString Line $}
                    {Int.toString Column $}
                    ""
                  ]
                end
              end
            end}
          {Boot_System.printRepr AbstractionsMap false true}
        end

        {ForEachStackFrame NextFrames}
      end
    end
    
    proc {ForEachThreadId ThreadIds}
      case ThreadIds of nil then skip
      [] ThreadId|NextThreadIds then
        Stack = SourceNames.ThreadId
      in
        {ForEachStackFrame Stack}
        {ForEachThreadId NextThreadIds}
      end
    end
  in
    {ForEachThreadId ThreadIds}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "abstractions" then
      {DisplayAbstractions}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end