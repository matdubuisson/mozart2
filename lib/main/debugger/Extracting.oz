proc {ExtractIdentities Arguments ?Identities}
  case Arguments of nil then Identities = nil
  [] Argument|NextArguments then
    NewIdentities
  in
    if {String.isInt Argument $} then
      Identities = {String.toInt Argument $}|NewIdentities
    else
      Id = {Boot_Identity.getIdFromName Argument $}
    in
      if Id \= none then
        Identities = Id|NewIdentities
      else
        Identities = NewIdentities
        {PrintError "Name '"#Argument#"' was referenced to no id"}
      end
    end
    {ExtractIdentities NextArguments NewIdentities}
  end
end

local
  proc {ExtractExpression Arguments ?NextArguments ?Result}
    case Arguments of Left|Operation|Right|Rest then
      IsRightInt = {String.isInt Right $}
    in
      if {String.isAtom Left $} andthen {Not {String.isInt Left $} $}
        andthen (IsRightInt orelse {String.isAtom Right $})
      then
        NewLeft = {String.toAtom Left $}
        NewRight = if IsRightInt then {String.toInt Right $}
          else {String.toAtom Right $} end
      in
        case Operation of "==" then
          NextArguments = Rest
          Result = '=='(NewLeft NewRight)
        [] "\\=" then
          NextArguments = Rest
          Result = '\\='(NewLeft NewRight)
        [] ">" then
          NextArguments = Rest
          Result = '>'(NewLeft NewRight)
        [] "<" then
          NextArguments = Rest
          Result = '<'(NewLeft NewRight)
        [] ">=" then
          NextArguments = Rest
          Result = '>='(NewLeft NewRight)
        [] "=<" then
          NextArguments = Rest
          Result = '=<'(NewLeft NewRight)
        [] "has" then
          NextArguments = Rest
          Result = 'has'(NewLeft NewRight)
        else
          NextArguments = nil
          Result = none
          {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#
            "' cannot be evaluated as an expression because '"#Operation#"' is not a valid operation "#
            "and it should be something like: '==', '\\=', '>', '<', '>=', '>=', 'has'"}
        end
      else
        NextArguments = nil
        Result = none
        {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#
          "' cannot be evaluated as an expression because left and right operands"#
          "must be lower case strings or int only for right operand"}
      end
    else
      NextArguments = nil
      Result = none
      {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#
        "' cannot be evaluated as an expression of the shape 'left op right'"}
    end
  end

  proc {ExtractBooleanPrime PreviousExpression Arguments ?NextArguments ?Result}
    case Arguments of nil then
      NextArguments = nil
      Result = PreviousExpression
    [] ")"|_ then
      NextArguments = Arguments
      Result = PreviousExpression
    [] "andthen"|Next2Arguments then
      Next3Arguments
      Boolean = {ExtractBoolean Next2Arguments Next3Arguments $}
    in
      if Boolean == none then
        NextArguments = nil
        Result = none
      else
        NextArguments = Next3Arguments
        Result = 'andthen'(PreviousExpression Boolean)
      end
    [] "orelse"|Next2Arguments then
      Next3Arguments
      Boolean = {ExtractBoolean Next2Arguments Next3Arguments $}
    in
      if Boolean == none then
        NextArguments = nil
        Result = none
      else
        NextArguments = Next3Arguments
        Result = 'orelse'(PreviousExpression Boolean)
      end
    else
      {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#"' cannot be evaluated as a boolean"}
    end
  end
  
  proc {ExtractBoolean Arguments ?NextArguments ?Result}
    case Arguments of "("|Next2Arguments then
      Next3Arguments
      Boolean = {ExtractBoolean Next2Arguments Next3Arguments $}
    in
      if Boolean == none then
        NextArguments = nil
        Result = none
      elsecase Next3Arguments of ")"|Next4Arguments then
        % SubBoolean = 'parenthesis'(Boolean)
        SubBoolean = Boolean
      in
        % In case of error both Result and NextArguments are properly set
        Result = {ExtractBooleanPrime SubBoolean Next4Arguments NextArguments $}
      else
        NextArguments = nil
        Result = none
        {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#
          "' cannot be evaluated as a clause because it has never been closed with a ')' parenthesis'"}
      end
    else
      Next2Arguments
      Expression = {ExtractExpression Arguments Next2Arguments $}
    in
      if Expression == none then
        NextArguments = nil
        Result = none
      else
        % In case of error both Result and NextArguments are properly set
        Result = {ExtractBooleanPrime Expression Next2Arguments NextArguments $}
      end
    end
  end
in
  proc {ExtractConditions Arguments ?Conditions}
    case Arguments of nil then Conditions = true
    else Conditions = {ExtractBoolean Arguments _ $} end
  end
end

local
  proc {CheckOperation Left Right Operation Arity RecordValue ?Result}
    if {List.member Left Arity $} then
      Result = {Operation RecordValue.Left Right $}
      % if {Int.is Right $} then
      %   Result = {Operation RecordValue.Left Right $}
      % elseif {List.member Right Arity $} then
      %   Result = {Operation RecordValue.Left RecordValue.right $}
      % else
      %   Result = none
      %   %{PrintInvalidFeatureError Right Arity}
      % end
    else
      Result = none
      {PrintInvalidFeatureError Left Arity}
    end
  end

  CheckClause

  proc {CheckBoolean Left Right Operation Arity RecordValue ?Result}
    LeftResult = {CheckClause Left Arity RecordValue $}
  in
    if LeftResult == none then Result = none
    else
      RightResult = {CheckClause Right Arity RecordValue $}
    in
      if RightResult == none then Result = none
      else
        Result = {Operation LeftResult RightResult $}
      end
    end
  end

  proc {CheckClause Condition Arity RecordValue ?Result}
    case Condition of '=='(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A == B) end
        Arity RecordValue $}
    [] '\\='(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A \= B) end
        Arity RecordValue $}
    [] '>'(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A > B) end
        Arity RecordValue $}
    [] '<'(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A < B) end
        Arity RecordValue $}
    [] '>='(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A >= B) end
        Arity RecordValue $}
    [] '=<'(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R} R = (A =< B) end
        Arity RecordValue $}

    [] 'has'(Left Right) then
      Result = {CheckOperation Left Right
        proc {$ A B ?R}
          case A of nil then R = false
          [] _|_ then R = {List.member B A $}
          else R = false end
        end
        Arity RecordValue $}

    [] 'andthen'(Left Right) then
      Result = {CheckBoolean Left Right
        proc {$ A B ?R} R = (A andthen B) end
        Arity RecordValue $}
    [] 'orelse'(Left Right) then
      Result = {CheckBoolean Left Right
        proc {$ A B ?R} R = (A orelse B) end
        Arity RecordValue $}
    end
  end
in
  proc {CheckConditions Condition Value ?Result}
    if {Record.is Value $} then
      Arity = {Record.arity Value $}
    in
      case Condition of true then Result = true
      else Result = {CheckClause Condition Arity Value $} end
    else Result = false end
  end
end