local
  proc {DisplayMainOptions}
    {DisplayOptions
      ["active" "passive"]
      [
        "when raised this alarm triggers a warning message and immediately stops the current program execution giving"
        "when raised this alarm triggers only a warning message"
      ]
    }
  end

  proc {DisplayAggregateOptions}
    {DisplayOptions
      ["threads" "variables"]
      [
        "create an alarm about thread events"
        "create an alarm about variable events"
      ]
    }
  end

  proc {DisplayThreadsAggregateOptions}
    {DisplayOptions
      [
        "inserted"
        "removed"
        "updated"
        "collected"
        
        "id <int>"
        "kindId <int>"
        "generationId <int>"

        "priority <low|medium|high|system>"
        "type <runnable|thread>"

        "runnable"
        "preempted"
        "preemptible"
        "terminated"
        "dead"
      ]
      [
        "detects a new inserted thread"
        "detects a removed thread from the alive threads list"
        "detects a thread changing its state (runnable, preempted, preemptible, terminated, dead, priority)"
        "detects a garbage collected thread"

        "filters the signaled thread with the provided id"
        "filters the signaled thread with the provided kindId"
        "filters the signaled thread with the provided generationId"

        "filters the signaled thread with the provided priority"
        "filters the signaled thread with the provided type"

        "selects runnable threads"
        "selects preempted threads"
        "selects preemptible threads"
        "selects terminated threads"
        "selects dead threads"
      ]
    }
  end

  proc {DisplayVariablesAggregateOptions}
    {DisplayOptions
      [
        "created"
        "needed"
        "bound"
        "collected"
        "waited"

        "id <int>"
        "type <runnable|thread>"
      ]
      [
        "detects a new created variable"
        "detects a variable becoming needed"
        "detects a variable becoming bound"
        "detects a garbage collected variable"
        "detects a new wait on the variable and provides the waiter in question"

        "filters the signaled variable with the provided id"
        "filters the signaled variable with the provided type"
      ]
    }
  end

  proc {DisplaySpecificAggregateOptions Aggregate}
    case Aggregate of threads then
      {DisplayThreadsAggregateOptions}
    [] variables then 
      {DisplayVariablesAggregateOptions}
    end
  end

  proc {Get NextArguments ArgumentType ArgumentRecord ArgumentName ArgumentTypeName ?Result}
    case NextArguments of nil then
      Result = none
      {PrintMissingArgumentError ArgumentName ArgumentTypeName}
    [] Parameter|_ then
      Value = {ExtractInput ArgumentType Parameter none $}
    in
      if Value == none then
        Result = none
        {PrintWrongArgumentError ArgumentName ArgumentTypeName Parameter}
      else
        Result = ArgumentRecord(Value)
      end
    end
  end

  proc {GetId NextArguments ?Result}
    Result = {Get NextArguments int id "id" "integer" $}
  end

  proc {GetKindId NextArguments ?Result}
    Result = {Get NextArguments int kindId "kind id" "integer" $}
  end

  proc {GetGenerationId NextArguments ?Result}
    Result = {Get NextArguments int generationId "generation id" "integer" $}
  end

  proc {GetPriority NextArguments ?Result}
    Result = {Get NextArguments atom priority "priority" "lowercase string" $}
  end

  proc {GetType NextArguments ?Result}
    Result = {Get NextArguments atom type "type" "lowercase string" $}
  end

  proc {GetThreadsCondition Argument NextArguments ?Result}
    case Argument of "inserted" then Result = inserted
    [] "removed" then Result = removed
    [] "updated" then Result = updated
    [] "collected" then Result = collected

    [] "id" then
      Result = {GetId NextArguments $}

    [] "kindId" then
      Result = {GetKindId NextArguments $}

    [] "generationId" then
      Result = {GetGenerationId NextArguments $}

    [] "priority" then
      Result = {GetPriority NextArguments $}

    [] "type" then
      Result = {GetType NextArguments $}

    [] "runnable" then Result = runnable
    [] "preempted" then Result = preempted
    [] "preemptible" then Result = preemptible
    [] "terminated" then Result = terminated
    [] "dead" then Result = dead

    else
      Result = none
      {PrintUnexpectedOptionError Argument}
    end
  end

  proc {GetVariablesCondition Argument NextArguments ?Result}
    case Argument of "created" then Result = created
    [] "needed" then Result = needed
    [] "bound" then Result = bound
    [] "collected" then Result = collected
    [] "waited" then Result = waited

    [] "id" then
      Result = {GetId NextArguments $}

    [] "type" then
      Result = {GetType NextArguments $}

    else
      Result = none
      {PrintUnexpectedOptionError Argument}
    end
  end

  proc {GetSpecificCondition Aggregate Argument NextArguments ?Result}
    case Aggregate of threads then
      Result = {GetThreadsCondition Argument NextArguments $}
    [] variables then
      Result = {GetVariablesCondition Argument NextArguments $}
    end
  end

  proc {GetConditions Aggregate Arguments ?Error ?Result}
    case Arguments of nil then
      Error = false
      Result = nil
    [] Argument|NextArguments then
      Condition = {GetSpecificCondition Aggregate Argument NextArguments $}
    in
      if Condition == none then
        Error = true
        Result = nil
      else
        NextResult
        RealNextArguments
      in
        Result = Condition|NextResult

        case Condition of id(_) then
          RealNextArguments = NextArguments.2
        [] kindId(_) then
          RealNextArguments = NextArguments.2
        [] generationId(_) then
          RealNextArguments = NextArguments.2
        [] priority(_) then
          RealNextArguments = NextArguments.2
        [] type(_) then
          RealNextArguments = NextArguments.2
        else
          RealNextArguments = NextArguments
        end

        {GetConditions Aggregate RealNextArguments Error NextResult}
      end
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
        Error
        Conditions = {GetConditions Aggregate Arguments Error $}
      in
        if {Bool.'not' Error $} then
          {Handle Type Aggregate Conditions}
        end
      end
    end
  end

  % alarm passive/active variables waited/referenced/bound/needed <id0> .... <idN>
  % alarm passive/active threads runnable/preempted/terminated/dead <id0> .... <idN>

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

  proc {ListAlarms Alarms AlarmId}
    proc {ConditionsToString Conditions Accumulated ?Result}
      case Conditions of nil then Result = Accumulated
      [] Condition|NextConditions then
        {ConditionsToString NextConditions Accumulated#" "#
          {Boot_System.getRepr Condition ~1 ~1 $} Result}
      end
    end
  in
    case Alarms of nil then skip
    [] Alarm|NextAlarms then
      {ListAlarms NextAlarms (AlarmId - 1)} % Not tail-recursive on purpose: displaying rules in reverse-order

      case Alarm of alarm(
        type: Type
        aggregate: Aggregate
        conditions: Conditions
      ) then
        {PrintInfo "\t"#AlarmId#") "#Type#" "#Aggregate#
          {ConditionsToString Conditions "" $}}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayMainOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayMainOptions}
    [] "active" then
      {HandleType active NextArguments}
    [] "passive" then
      {HandleType passive NextArguments}
    [] "list" then
      {PrintInfo "Alarms:"}
      {ListAlarms Alarms ({List.length Alarms $} - 1)}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end
