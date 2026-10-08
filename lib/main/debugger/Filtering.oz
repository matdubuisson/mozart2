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

        case StringValue of "true" then Value = true
        [] "false" then Value = false
        [] "nil" then Value = nil
        else
          if {String.isInt StringValue $} then
            Value = {String.toInt StringValue $}
          elseif {String.isFloat StringValue $} then
            Value = {String.toFloat StringValue $}
          elseif {String.isAtom StringValue $} then
            Value = {String.toAtom StringValue $}
          else Value = StringValue end
        end

        if AttributeAtom == none orelse OpInt == none then
          Condition = none
          NewNextArguments = nil
        else
          Condition = condition(
            attribute: AttributeAtom
            operation: OpInt
            value: Value
          )
          NewNextArguments = NextArguments
        end
      else
        Condition = none
        NewNextArguments = nil
        {PrintError "Cannot extract condition from "#
          {Boot_System.getRepr Arguments ~1 ~1 $}}
      end
    end

    proc {ExtractConditions Arguments From To
      ?FinalFrom ?FinalTo ?Conditions ?Result}
      proc {Return}
        FinalFrom = From
        FinalTo = To
        Conditions = nil
      end
    in
      case Arguments of nil then
        {Return}
        Result = true
      [] Argument|NextArguments then
        proc {Error}
          {Return}
          Result = false
        end

        proc {Normal Condition NextArguments}
          NextConditions
        in
          if Condition == none then {Error}
          else
            Conditions = Condition|NextConditions
            {ExtractConditions NextArguments From To
              FinalFrom FinalTo
              NextConditions Result}
          end
        end
      
        proc {GetBound Which Arguments ?Value ?NextArguments}
          Value = {ExtractSomething Which int Arguments $}
          
          if Value == none then {Error}
          else
            case Arguments of nil then {Error}
            [] _|Tail then
              NextArguments = Tail
            end
          end
        end
      in
        case Argument of "from" then
          Value NextNextArguments
        in
          {GetBound "from" NextArguments Value NextNextArguments}
          {ExtractConditions NextNextArguments Value To
            FinalFrom FinalTo Conditions Result}
        [] "to" then
            Value NextNextArguments
        in
          {GetBound "from" NextArguments Value NextNextArguments}
          {ExtractConditions NextNextArguments From Value
            FinalFrom FinalTo Conditions Result}
        [] "and" then {Normal 'and' NextArguments}
        [] "or" then {Normal 'or' NextArguments}
        else
          Condition NewNextArguments
        in
          {ExtractCondition Arguments Condition NewNextArguments}
          {Normal Condition NewNextArguments}
        end
      end
    end
  in
    proc {ExtractFilteringParameters Arguments ?From ?To ?Conditions ?Result}
      {ExtractConditions Arguments 0 100 From To Conditions Result}
    end
  end

  local
    proc {MatchCondition Input Condition ?Result}
      case Condition of condition(
        attribute: Attribute
        operation: Operation
        value: Value
      ) then
        Arity = {Record.arity Input $}
      in
        % Input.Attribute
        if {List.member Attribute Arity $} then
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
          end
        else
          Label = {Record.label Input $}
          Features = {Boot_System.getRepr Arity ~1 ~1 $}
        in
          Result = error
          {PrintError "Attribute '"#Attribute#"' is not a feature of record '"#
            Label#"', see list of features: "#Features}
        end
      end
    end

    proc {MatchConditions Input Conditions Flag ?Result}
      case Conditions of nil then Result = Flag
      [] Condition|NextConditions then
        case Condition of condition(...) then
          NewFlag = {MatchCondition Input Condition $}
        in
          case NewFlag of error then
            Result = error
          else
            {MatchConditions Input NextConditions NewFlag Result}
          end
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
        Flag = {MatchConditions Input Conditions true $}
      in
        case Flag of error then
          FilteredInputs = nil
        else
          NextFilteredInputs
        in
          if Flag then
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

end
