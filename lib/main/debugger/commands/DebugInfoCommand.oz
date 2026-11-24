local
  proc {DisplayOptions}
    {DisplayFrame "DebugInfo command options" [
      "abstractions <condition>" "displaying procedure descritpions including procedure name and all parameters' names"
      "variables <condition>" "showing mapping of ids to list of source names"
    ]}
  end

  proc {DisplayAbstractions Conditions}
    AbstractionsMap = {Boot_DebugInfoManager.getAbstractions $}
    ObjectIds = {Record.arity AbstractionsMap $}
  in
    {DisplayCSV
      [
        "Id      " "Type    "
        flexible("Source Name                     ")
        flexible("Filename                        ")
        "Line    " "Column  "
        flexible("Parameters' Source Names")
      ]
      ObjectIds
      proc {$ ObjectId ?Tuple} Tuple = ObjectId#AbstractionsMap.ObjectId end
      proc {$ Tuple ?Result}
        case Tuple of ObjectId#(Head|Tail) then
          Value = {Record.adjoin Head value(id: ObjectId) $}
        in
          Result = {CheckConditions Conditions Value $}
        end
      end
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
              {List.foldL Tail
                proc {$ AccumulatedString SourceName ?Result}
                  Name = {Atom.toString SourceName.name $}
                in
                  if AccumulatedString == "" then Result = Name
                  else
                    SubString = {List.append AccumulatedString ", " $}
                  in
                    Result = {List.append SubString Name $}
                  end
                end
                "" $}
            ]
          end
        end
      end}
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "abstractions" then
      Conditions = {ExtractConditions NextArguments $}
    in
      if Conditions \= none then
        {DisplayAbstractions Conditions}
      end
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end