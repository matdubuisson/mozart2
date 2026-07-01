local
  proc {DisplayOptions}
    {PrintInfo "status (optional: <id0> .... <idN>)\tdisplays variables' statuses"}
    {PrintInfo "pendings (optional: <id0> .... <idN>)\tdisplays pending threads on the variable"}
  end

  proc {HandleStatusCase Variable}
    case Variable of variable(
      id: Id
      isNeeded: IsNeeded
      threadId: ThreadId
      type: Type
    ) then
      {PrintInfo
        {Int.toString Id $}#"\t"#
        {Int.toString ThreadId $}#"\t"#
        {Atom.toString Type $}#"\t"#
        if Type == optVariable then "false" else "true" end#"\t"#
        {Bool.toString IsNeeded $}#"\t"#
        "none"
      }
    [] variable(
      id: Id
      isBound: IsBound
      isNeeded: IsNeeded
      pendings: Pendings
      type: Type
      threadId: ThreadId
    ) then
      {PrintInfo
        {Int.toString Id $}#"\t"#
        {Int.toString ThreadId $}#"\t"#
        {Atom.toString Type $}#"\t"#
        {Bool.toString IsBound $}#"\t"#
        {Bool.toString IsNeeded $}#"\t"#
        {Int.toString
          {List.length Pendings $} $}
      }
    end
  end

  proc {HandlePendingsCase Variable}
    case Variable of variable(
      id: Id
      isNeeded: _
      type: Type
      threadId: ThreadId
    ) then
      {PrintInfo
        {Int.toString Id $}#"\t"#
        {Int.toString ThreadId $}#"\t"#
        {Atom.toString Type $}#"\t"#
        "none"}
    [] variable(
      id: Id
      isBound: _
      isNeeded: _
      pendings: Pendings
      type: Type
      threadId: ThreadId
    ) then
      proc {DisplayPendings I Max Pendings}
        case Pendings of nil then {PrintLn ""}
        [] Pending|NextPendings then
          if I > 0 then {Print ", "} end
          {Print {Int.toString Pending $}}
          if I < Max then            
            {DisplayPendings I+1 Max NextPendings}
          else
            {PrintLn ""}
          end
        end
      end
    in
      {PrintOtherPrefix "INFO"}
      {Print
        {Int.toString Id $}#"\t"#
        {Int.toString ThreadId $}#"\t"#
        {Atom.toString Type $}#"\t"}
      if Pendings == nil then
        {PrintLn "none"}
      else
        {DisplayPendings 0 100 Pendings}
      end
    end
  end

  proc {DisplayVariables Variables Aggregate}
    case Variables of nil then skip
    [] Variable|NextVariables then
      case Aggregate of status then
        {HandleStatusCase Variable}
      [] pendings then
        {HandlePendingsCase Variable}
      end
      {DisplayVariables NextVariables Aggregate}
    end
  end

  proc {HandleOption Arguments Aggregate}
    proc {ForEachThreadId Arguments Aggregate}
      case Arguments of nil then skip
      [] Argument|NextArguments then
        ThreadId = {ExtractInput int Argument none $}
      in
        if ThreadId == none then
          {PrintError "Unexpected provided thread id '"#Argument#
            "' that is not an integer"}
        else
          Thread = {Boot_Introspection.getThread ThreadId $}
          Variables = {Boot_Introspection.getThreadVariables Thread $}
        in
          {DisplayVariables Variables Aggregate}
          {ForEachThreadId NextArguments Aggregate}
        end
      end
    end

  in
    if Arguments == nil then
      {DisplayVariables
        {Boot_Introspection.getVariables $} Aggregate}
    else
      {ForEachThreadId Arguments Aggregate}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "status" then
      {PrintInfo "Id\tThreadId\tType\tBound\tNeeded\tNPendings"}
      {HandleOption NextArguments status}
    [] "pendings" then
      {PrintInfo "Id\tThreadId\tType\tPendings"}
      {HandleOption NextArguments pendings}
    else
      {PrintError "Unexpected variables aggregate '"#Argument#"'"#TRYHELP}
    end
  end
end