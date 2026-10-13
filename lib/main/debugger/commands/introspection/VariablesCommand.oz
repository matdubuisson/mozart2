local
  proc {DisplayOptions}
    {DisplayFrame "Variables command options" [
      "state <condition>" "displays variables' states"
      "pendings <condition>" "displays pending threads on the variable"
      "candidates <condition>" "displays candidate threads to bind the variable"
    ]}
  end

  proc {HandleStateOption VariableIds VariablesMap Conditions}
    {DisplayCSV
      [
        "Id      " "KindId  " "GenerationId"
        "Type    " "Bound " "Needed"
        "Candidates Count" "Pendings Count"
        flexible("Value")
      ]
      VariableIds
      proc {$ Id ?Record} Record = VariablesMap.Id end
      proc {$ Variable ?Result} Result = {CheckConditions Conditions Variable $} end
      proc {$ Variable ?Result}
        case Variable of variable(
          id: Id
          kindId: KindId
          generationId: GenerationId
          type: Type
          isBound: IsBound
          isNeeded: IsNeeded
          candidates: Candidates
          pendings: Pendings
          value: Value
        ) then
          Result = [
            {Int.toString Id $}
            {Int.toString KindId $}
            {Int.toString GenerationId $}
            {Atom.toString Type $}
            {Bool.toString IsBound $}
            {Bool.toString IsNeeded $}
            {Int.toString
              {List.length Candidates $} $}
            {Int.toString
              {List.length Pendings $} $}
            {Atom.toString Value $}
          ]
        end
      end}
  end

  proc {HandleCandidatesOption VariableIds VariablesMap Conditions}
    {DisplayCSV
      [
        "Id      " "Bound " "Needed"
        "Candidates Count" "Pendings Count"
        flexible("Candidates")
      ]
      VariableIds
      proc {$ Id ?Record} Record = VariablesMap.Id end
      proc {$ Variable ?Result} Result = {CheckConditions Conditions Variable $} end
      proc {$ Variable ?Result}
        case Variable of variable(
          id: Id
          isBound: IsBound
          isNeeded: IsNeeded
          candidates: Candidates
          pendings: Pendings
          ...
        ) then
          Result = [
            {Int.toString Id $}
            {Bool.toString IsBound $}
            {Bool.toString IsNeeded $}
            {Int.toString
              {List.length Candidates $} $}
            {Int.toString
              {List.length Pendings $} $}
            {ListToString Candidates $}
          ]
        end
      end}
  end

  proc {HandlePendingsOption VariableIds VariablesMap Conditions}
    {DisplayCSV
      [
        "Id      " "Bound " "Needed"
        "Candidates Count" "Pendings Count"
        flexible("Pendings")
      ]
      VariableIds
      proc {$ Id ?Record} Record = VariablesMap.Id end
      proc {$ Variable ?Result} Result = {CheckConditions Conditions Variable $} end
      proc {$ Variable ?Result}
        case Variable of variable(
          id: Id
          isBound: IsBound
          isNeeded: IsNeeded
          candidates: Candidates
          pendings: Pendings
          ...
        ) then
          Result = [
            {Int.toString Id $}
            {Bool.toString IsBound $}
            {Bool.toString IsNeeded $}
            {Int.toString
              {List.length Candidates $} $}
            {Int.toString
              {List.length Pendings $} $}
            {ListToString Pendings $}
          ]
        end
      end}
  end

  proc {HandleOption Option Arguments}
    Conditions = {ExtractConditions Arguments $}
  in
    if Conditions \= none then
      VariablesMap = {Boot_Introspection.getVariables $}
    in
      % {Boot_System.printRepr VariablesMap false true}

      case Option of state then
        {HandleStateOption
          {Record.arity VariablesMap $} VariablesMap Conditions}
      [] candidates then
        {HandleCandidatesOption
          {Record.arity VariablesMap $} VariablesMap Conditions}
      [] pendings then
        {HandlePendingsOption
          {Record.arity VariablesMap $} VariablesMap Conditions}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "state" then
      {HandleOption state NextArguments}
    [] "candidates" then
      {HandleOption candidates NextArguments}
    [] "pendings" then
      {HandleOption pendings NextArguments}
    else
      {PrintError "Unexpected variables aggregate '"#Argument#"'"#TRYHELP}
    end
  end
end