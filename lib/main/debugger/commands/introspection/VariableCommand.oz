local
  proc {DisplayOptions}
    {DisplayFrame "Variable command options" [
      "<identity> state" "display the state of the specified variable"
      "TODO <identity> candidates" "display the candidates of the specified variable"
      "TODO <identity> pendings" "display the pendings of the specified variable"
    ]}
  end
  
  proc {DisplayState Id Variable}
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
      {DisplayFrame "Variable state" [
        "Id" Id
        "Kind Id" KindId
        "Generation Id" GenerationId

        "Type" Type

        "Is bound" {Bool.toString IsBound $}
        "Is needed" {Bool.toString IsNeeded $}

        "Candidates Count" {List.length Candidates $}
        "Candidates" {ListToString Candidates $}
        "Pending count" {List.length Candidates $}
        "Pendings" {ListToString Pendings $}

        "Value" {Atom.toString Value $}
      ]}
    end
  end

  proc {HandleOption Id Variable Arguments}
    case Arguments of nil then
      {DisplayOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayOptions}
      [] "state" then
        {DisplayState Id Variable}
      % [] "candidates" then
      %   {DisplayStatistics Id Variable}
      % [] "pendings" then
      %   {DisplayNodes Id Variable}
      else
        {PrintInvalidOptionError Argument}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      Id = {ExtractIdentity Argument $}
    in
      if Id == none then
        {PrintInvalidIdError Argument}
      else
        Variable = {Boot_Introspection.getVariable Id $}
      in
        if Variable == none then
          {PrintVariableNotFoundError Id}
        else
          {HandleOption Id Variable NextArguments}
          skip
        end
      end
    end
  end
end