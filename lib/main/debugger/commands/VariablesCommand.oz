local
  proc {DisplayOptions}
    {DisplayNameDescriptions
      [
        "state (optional: <id0> .... <idN>)"
        "pendings (optional: <id0> .... <idN>)"
        "candidates (optional: <id0> .... <idN>)"
        "value (optional: <id0> .... <idN>)"
      ]
      [
        "displays variables' states"
        "displays pending threads on the variable"
        "displays candidate threads to bind the variable"
        "displays variables' values"
      ]
    }
  end

  proc {FormatStateCase Variable ?Result}
    case Variable of variable(
      id: Id
      type: Type
      isBound: IsBound
      isNeeded: IsNeeded
      pendings: Pendings
      candidates: Candidates
      value: _
    ) then
      Result = [
        {Int.toString Id $}
        {Atom.toString Type $}
        {Bool.toString IsBound $}
        {Bool.toString IsNeeded $}
        {Int.toString
          {List.length Pendings $} $}
        {Int.toString
          {List.length Candidates $} $}
      ]
    end
  end

  proc {FormatPendingsCase Variable ?Result}
    case Variable of variable(
      id: Id
      type: _
      isBound: IsBound
      isNeeded: IsNeeded
      pendings: Pendings
      candidates: _
      value: _
    ) then
      Result = [
        {Int.toString Id $}
        {Bool.toString IsBound $}
        {Bool.toString IsNeeded $}
        {IdsToString Pendings $}
      ]
    end
  end

  proc {FormatCandidatesCase Variable ?Result}
    case Variable of variable(
      id: Id
      type: _
      isBound: IsBound
      isNeeded: IsNeeded
      pendings: _
      candidates: Candidates
      value: _
    ) then
      Result = [
        {Int.toString Id $}
        {Bool.toString IsBound $}
        {Bool.toString IsNeeded $}
        {IdsToString Candidates $}
      ]
    end
  end

  proc {FormatValueCase Variable ?Result}
    case Variable of variable(
      id: Id
      type: Type
      isBound: IsBound
      isNeeded: IsNeeded
      pendings: _
      candidates: _
      value: Value
    ) then
      Result = [
        {Int.toString Id $}
        {Atom.toString Type $}
        {Bool.toString IsBound $}
        {Bool.toString IsNeeded $}
        Value
      ]
    end
  end

  proc {HandleCase Aggregate Variables}
    case Aggregate of state then
      {DisplayCSV
        ["Id" "Type" "IsBound" "IsNeeded" "NPendings" "NCandidates"]
        Variables
        10
        FormatStateCase
      }
    [] pendings then
      {MaskedDisplayCSV
        ["Id" "IsBound" "IsNeeded" "Pendings"]
        Variables
        10
        FormatPendingsCase
        [true true true false]
      }
    [] candidates then
      {MaskedDisplayCSV
        ["Id" "IsBound" "IsNeeded" "Candidates"]
        Variables
        10
        FormatCandidatesCase
        [true true true false]
      }
    [] value then
      {MaskedDisplayCSV
        ["Id" "Type" "IsBound" "IsNeeded" "Value"]
        Variables
        10
        FormatValueCase
        [true true true true false]
      }
    end
  end

  proc {HandleOption Arguments Aggregate}
    proc {Execute I Id}
      if {ValidId Id $} then
        Thread = {GetThreadFromId Id $}
      in
        if Thread \= none then
          Variables = {Boot_Introspection.getThreadVariables Thread $}
        in
          {HandleCase Aggregate Variables}
        end
      else {PrintInvalidIdError I} end
    end
  in
    if Arguments == nil then
      Variables = {Boot_Introspection.getAllVariables $}
    in
      {HandleCase Aggregate Variables}
    else
      Ids = {ExtractInputs int Arguments none $}
    in
      {ForEachI Ids Execute}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      {HandleOption NextArguments state}
    [] "pendings" then
      {HandleOption NextArguments pendings}
    [] "candidates" then
      {HandleOption NextArguments candidates}
    [] "value" then
      {HandleOption NextArguments value}
    else
      {PrintError "Unexpected variables aggregate '"#Argument#"'"#TRYHELP}
    end
  end
end