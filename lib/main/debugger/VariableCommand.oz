local
  proc {DisplayOptions}
    {PrintInfo "<id0> .... <idN>\tdisplay the state of one or several variable(s) specified by their id"}
  end

  proc {ExtractVariableIds Arguments ?Error ?Result}
    case Arguments of nil then
      Error = false
      Result = nil
    [] Argument|NextArguments then
      VariableId = {ExtractInput int Argument none $}
    in
      if VariableId == none then
        {PrintError "Unexpected provided variable id '"#Argument#
          "' that is not an integer"}
        Error = true
        Result = nil
      else
        NextResult
      in
        Result = VariableId|NextResult
        {ExtractVariableIds NextArguments Error NextResult}
      end
    end
  end

  proc {DisplayPendings Pendings}
    case Pendings of nil then skip
    [] Id|NextPendings then
      Thread = {Boot_Introspection.getThread Id $}
    in
      % if Thread == none then
      %   {PrintWarning "Thread "#{Int.toString Id $}#" does not exist anymore"}
      % else
        {PrintInfo "\t\tThread "#Id#":"}
        {PrintInfo "\t\t\tPriority: "#
          {Atom.toString {Boot_Thread.getPriority Thread $} $}}
        {PrintInfo "\t\t\tRunnable: "#
          {Bool.toString {Boot_Thread.isRunnable Thread $} $}}
        {PrintInfo "\t\t\tTerminated: "#
          {Bool.toString {Boot_Thread.isTerminated Thread $} $}}
        {PrintInfo "\t\t\tDead: "#
          {Bool.toString {Boot_Thread.isDead Thread $} $}}
        {PrintInfo "\t\t\tPreempted: "#
          {Bool.toString {Boot_Thread.isPreempted Thread $} $}}
        {PrintInfo "\t\t\tPreemptible: "#
          {Bool.toString {Boot_Thread.isPreemptible Thread $} $}}
      % end

      {DisplayPendings NextPendings}
    end
  end

  proc {DisplayVariables Variables VariablesIds}
    case Variables of nil then skip
    [] Variable|NextVariables then
      if {List.member Variable.id VariablesIds $} then
        case Variable of variable(
          id: Id
          isNeeded: IsNeeded
          type: Type
          threadId: ThreadId
        ) then
          {PrintInfo "Variable "#{Int.toString Id $}#":"}
          {PrintInfo "\tOwner thread id: "#{Int.toString ThreadId $}}
          {PrintInfo "\tType: "#Type}
          {PrintInfo "\tIsBound: "#if Type == optVariable then "false" else "true" end}
          {PrintInfo "\tIsNeeded: "#{Bool.toString IsNeeded $}}
          {PrintInfo "\tPendings: none"}
        [] variable(
          id: Id
          isBound: IsBound
          isNeeded: IsNeeded
          pendings: Pendings
          type: Type
          threadId: ThreadId
        ) then
          {PrintInfo "Variable "#{Int.toString Id $}#":"}
          {PrintInfo "\tOwner thread id: "#{Int.toString ThreadId $}}
          {PrintInfo "\tType: "#Type}
          {PrintInfo "\tIsBound: "#{Bool.toString IsBound $}}
          {PrintInfo "\tIsNeeded: "#{Bool.toString IsNeeded $}}

          if {List.length Pendings $} == 0 then
            {PrintInfo "\tPendings: none"}
          else
            {PrintInfo "\tPendings:"}
            {Boot_System.printVS Pendings false true}
            {DisplayPendings Pendings}
          end
        end
      end
      
      {DisplayVariables NextVariables VariablesIds}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|_ then
    case Argument of "help" then
      {DisplayOptions}
    else
      Error
      VariablesIds = {ExtractVariableIds Arguments Error $}
    in
      if Error == false then
        Variables = {Boot_Introspection.getVariables $}
      in
        {DisplayVariables Variables VariablesIds}
      end
    end
  end
end