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
        [] "<=" then
          NextArguments = Rest
          Result = '<='(NewLeft NewRight)
        [] "has" then
          NextArguments = Rest
          Result = '=='(NewLeft NewRight)
        else
          NextArguments = nil
          Result = none
          {PrintError "Arguments '"#{Boot_System.getRepr Arguments ~1 ~1 $}#
            "' cannot be evaluated as an expression because '"#Operation#"' is not a valid operation "#
            "and it should be something like: '==', '\\=', '>', '<', '>=', '<=', 'has'"}
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
        SubBoolean = 'parenthesis'(Boolean)
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
    {ExtractBoolean Arguments _ Conditions}
  end
end