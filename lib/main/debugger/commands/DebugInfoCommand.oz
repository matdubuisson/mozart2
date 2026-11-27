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

  proc {DisplayVariables Conditions}
    VariablesMap = {Boot_DebugInfoManager.getVariables $}
    Values = {Record.foldLInd VariablesMap
      proc {$ Id AccumulatedList SourceNames ?Result}
        NewSourceNames = {List.map SourceNames
          proc {$ Element ?Result}
            Result = {Record.adjoin Element sourceName(id: Id) $}
          end
          $}
      in
        Result = {List.append AccumulatedList NewSourceNames $}
      end
      nil $}
  in
    {DisplayCSV
      [
        "Id      " "KindId  " "GenerationId"
        
        "Type    "
        flexible("Source Name                     ")
        flexible("Filename                        ")
        "Line    " "Column  "
      ]
      Values
      proc {$ Value ?Result} Result = Value end
      proc {$ Value ?Result}
        Result = {CheckConditions Conditions Value $}
      end
      proc {$ Value ?Result}
        case Value of sourceName(
          id: Id
          kindId: KindId
          generationId: GenerationId

          type: Type
          name: Name
          filename: Filename
          line: Line
          column: Column
        ) then
          Result = [
            {Int.toString Id $}
            {Int.toString KindId $}
            {Int.toString GenerationId $}

            {Atom.toString Type $}
            {Atom.toString Name $}
            {Atom.toString Filename $}
            {Int.toString Line $}
            {Int.toString Column $}
          ]
        end
      end}
  end

  proc {DisplayResolved Arguments}
    Name
    Ids
  in
    case Arguments of [N] then
      Name = {ExtractInput atom N none $}

      if Name == none then
        {PrintError "Provided name "#Name#" is not a string"}
        Ids = nil
      else
        Ids = {Boot_DebugInfoManager.resolveName Name $}
      end
    [] [N F L C] then
      Filename = {ExtractInput atom F none $}
      Line = {ExtractInput int L none $}
      Column = {ExtractInput int C none $}
    in
      Name = {ExtractInput atom N none $}

      if Name == none then
        {PrintError "Provided name "#Name#" is not a string"}
        Ids = nil
      elseif Filename == none then
        {PrintError "Provided filename "#Filename#" is not a string"}
        Ids = nil
      elseif Line == none then
        {PrintError "Provided line "#Line#" is not a string"}
        Ids = nil
      elseif Column == none then
        {PrintError "Provided column "#Column#" is not a string"}
        Ids = nil
      else
        Ids = {Boot_DebugInfoManager.resolveCompleteName Name Filename Line Column $}
      end
    end

    if Name \= none then
      if Ids == nil then {PrintInfo "Name "#Name#" has no associated ids"}
      else
        {PrintInfo "Name "#Name#" is associated to these ids: "#{ListToString Ids $}}
      end
    end
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
    [] "variables" then
      Conditions = {ExtractConditions NextArguments $}
    in
      if Conditions \= none then
        {DisplayVariables Conditions}
      end
    [] "resolve" then
      N = {List.length NextArguments $}
    in
      if N \= 1 andthen N \= 4 then
        {DisplayOptions}
      elseif N == 1 then
        {DisplayResolved NextArguments}
      end
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end