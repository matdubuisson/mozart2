local
  proc {DisplayOptions}
    {PrintInfo "active"}
    {PrintInfo "passive"}
  end

  proc {DisplayAggregateOptions}
    {PrintInfo "threads"}
    {PrintInfo "variables"}
  end

  proc {DisplayThreadsAggregateOptions}
    {PrintInfo "runnable"}
    {PrintInfo "suspended"}
    {PrintInfo "terminated"}
    {PrintInfo "dead"}
  end

  proc {DisplayVariablesAggregateOptions}
    {PrintInfo "referenced"}
    {PrintInfo "waited"}
    {PrintInfo "bound"}
    {PrintInfo "needed"}
  end

  proc {DisplaySpecificAggregateOptions Aggregate}
    case Aggregate of threads then
      {DisplayThreadsAggregateOptions}
    [] variables then 
      {DisplayVariablesAggregateOptions}
    end
  end

  proc {GetThreadsCondition Argument ?Result}
    case Argument of "runnable" then Result = runnable
    [] "suspended" then Result = suspended
    [] "terminated" then Result = terminated
    [] "dead" then Result = dead
    else
      {PrintUnexpectedOptionError Argument}
    end
  end

  proc {GetVariablesCondition Argument ?Result}
    case Argument of "referenced" then Result = referenced
    [] "waited" then Result = waited
    [] "bound" then Result = bound
    [] "needed" then Result = needed
    else
      {PrintUnexpectedOptionError Argument}
    end
  end

  proc {GetSpecificCondition Aggregate Argument ?Result}
    case Aggregate of threads then
      Result = {GetThreadsCondition Argument $}
    [] variables then
      Result = {GetVariablesCondition Argument $}
    end
  end

  proc {GetConditions Aggregate Arguments ?Result}
    case Arguments of nil then Result = nil
    [] Argument|NextArguments then
      Condition = {GetSpecificCondition Aggregate Argument $}
      NextResult
    in
      Result = Condition|NextResult
      {GetConditions Aggregate NextArguments NextResult}
    end
  end

  proc {Handle Type Aggregate Conditions}
    NewAlarm = alarm(
      type: Type
      aggregate: Aggregate
      conditions: Conditions
    )

    NewAlarms = NewAlarm|Alarms
  in
    {Cell.assign AlarmsCell NewAlarms}
  end

  proc {HandleAggregate Type Aggregate Arguments}
    case Arguments of nil then
      {DisplaySpecificAggregateOptions Aggregate}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplaySpecificAggregateOptions Aggregate}
      else
        Conditions = {GetConditions Aggregate Arguments $}
      in
        {Handle Type Aggregate Conditions}
      end
    end
  end

  % alarm passive/active variables waited/referenced/bound/needed <id0> .... <idN>
  % alarm passive/active threads runnable/suspended/terminated/dead <id0> .... <idN>

  proc {HandleType Type Arguments}
    case Arguments of nil then
      {DisplayAggregateOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayAggregateOptions}
      [] "threads" then
        {HandleAggregate Type threads NextArguments}
      [] "variables" then
        {HandleAggregate Type variables NextArguments}
      else
        {PrintUnexpectedOptionError Argument}
      end
    end
  end

  proc {ListAlarms Alarms}
    proc {ConditionsToString Conditions Accumulated ?Result}
      case Conditions of nil then Result = Accumulated
      [] Condition|NextConditions then
        {ConditionsToString NextConditions Accumulated#" "#
          {Atom.toString Condition $} Result}
      end
    end
  in
    case Alarms of nil then skip
    [] Alarm|NextAlarms then
      case Alarm of alarm(
        type: Type
        aggregate: Aggregate
        conditions: Conditions
      ) then
        {PrintInfo "\t- "#Type#" "#Aggregate#
          {ConditionsToString Conditions "" $}}
      end

      {ListAlarms NextAlarms}
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "active" then
      {HandleType active NextArguments}
    [] "passive" then
      {HandleType passive NextArguments}
    [] "list" then
      {PrintInfo "Alarms:"}
      {ListAlarms Alarms}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end
