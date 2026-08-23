local
  OpEqual = 0
  OpNotEqual = 1
  OpGreater = 2
  OpLower = 3
  OpGreaterOrEqual = 4
  OpLowerOrEqual = 5
  OpHas = 6

  local
    proc {ExtractCondition Arguments ?Condition ?NewNextArguments}
      case Arguments
      of Attribute|Op|StringValue|NextArguments then
        AttributeAtom OpInt Value
      in
        if {String.isAtom Attribute $} then
          AttributeAtom = {String.toAtom Attribute $}
        else
          AttributeAtom = none
          {PrintError "Invalid attribute '"#Attribute#
            "', must be a lowercase atom/string"}
        end

        case Op of "==" then OpInt = OpEqual
        [] "\\=" then OpInt = OpNotEqual
        [] ">" then OpInt = OpGreater
        [] "<" then OpInt = OpLower
        [] ">=" then OpInt = OpGreaterOrEqual
        [] "=<" then OpInt = OpLowerOrEqual
        [] "has" then OpInt = OpHas
        else
          OpInt = none
          {PrintError "Invalid operation '"#Op#"' provided"}
        end

        if {String.isInt StringValue $} then
          Value = {String.toInt StringValue $}
        elseif {String.isFloat StringValue $} then
          Value = {String.toFloat StringValue $}
        elseif {String.isAtom StringValue $} then
          Value = {String.toAtom StringValue $}
        else Value = StringValue end

        if OpInt == none then
          Condition = none
          NewNextArguments = nil
        else
          Condition = condition(
            attribute: AttributeAtom
            operation: OpInt
            value: Value
          )
          {Boot_System.printRepr Condition false true}
          NewNextArguments = NextArguments
        end
      else
        Condition = none
        NewNextArguments = nil
        {PrintError "Cannot extract condition from "#
          {Boot_System.getRepr Arguments ~1 ~1 $}}
      end
    end

    proc {ExtractConditions Arguments ?Conditions}
      case Arguments of nil then Conditions = nil
      [] Argument|NextArguments then
        Condition NextConditions NewNextArguments
      in
        case Argument of "and" then
          Condition = 'and'
          NewNextArguments = NextArguments
        [] "or" then
          Condition = 'or'
          NewNextArguments = NextArguments
        else
          {ExtractCondition Arguments Condition NewNextArguments}
        end

        if Condition == none then Conditions = nil
        else
          Conditions = Condition|NextConditions
          {ExtractConditions NewNextArguments NextConditions}
        end
      end
    end
  in
    proc {ExtractFilteringParameters Arguments ?Conditions}
      {ExtractConditions Arguments Conditions}
      {Boot_System.printRepr Conditions false true}
    end
  end

  local
    proc {MatchCondition Input Condition ?Result}
      % {Boot_System.printRepr input(Input) false true}
      % {Boot_System.printRepr condition(Condition) false true}

      case Condition of condition(
        attribute: Attribute
        operation: Operation
        value: Value
      ) then
        AttributeValue = Input.Attribute
      in      
        if Operation == OpEqual then
          Result = (AttributeValue == Value)
        elseif Operation == OpNotEqual then
          Result = (AttributeValue \= Value)
        elseif Operation == OpGreater then
          Result = (AttributeValue > Value)
        elseif Operation == OpLower then
          Result = (AttributeValue < Value)
        elseif Operation == OpGreaterOrEqual then
          Result = (AttributeValue >= Value)
        elseif Operation == OpLowerOrEqual then
          Result = (AttributeValue =< Value)
        elseif Operation == OpHas then
          Result = {List.member Value AttributeValue $}
        else
          Result = false
          {PrintError "Unknown operation '"#Operation#"'"}
        end
      end
    end

    proc {MatchConditions Input Conditions Flag ?Result}
      case Conditions of nil then Result = Flag
      [] Condition|NextConditions then
        case Condition of condition(...) then
          NewFlag = {MatchCondition Input Condition $}
        in
          {MatchConditions Input NextConditions NewFlag Result}
        [] 'and' then
          if Flag then
            {MatchConditions Input NextConditions true Result}
          else Result = false end
        [] 'or' then
          if Flag then
            proc {GetUntilNextAnd Conditions ?NewConditions}
              case Conditions of nil then NewConditions = nil
              [] Condition|NextConditions then
                case Condition of 'and' then NewConditions = NextConditions
                else {GetUntilNextAnd NextConditions NewConditions} end
              end
            end

            NewNextConditions = {GetUntilNextAnd NextConditions $}
          in
            {MatchConditions Input NewNextConditions Flag Result}
          else
            {MatchConditions Input NextConditions Flag Result}
          end
        end
      end
    end
  in
    proc {FilterInputsUsingFilteringParameters Inputs Conditions ?FilteredInputs}
      case Inputs of nil then FilteredInputs = nil
      [] Input|NextInputs then
        NextFilteredInputs
      in
        if {MatchConditions Input Conditions true $} then
          FilteredInputs = Input|NextFilteredInputs
        else
          NextFilteredInputs = FilteredInputs
        end

        {FilterInputsUsingFilteringParameters
          NextInputs Conditions NextFilteredInputs}
      end
    end
  end

end
