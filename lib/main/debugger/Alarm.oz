local
  proc {ParseThreadsAlarm Alarm AlarmId Conditions ?FoundRaisedAlarm}
    proc {ForEach States Announce Conditions Match Raise ?FoundRaisedAlarm}
      proc {Loop States Flag}
        case States of nil then FoundRaisedAlarm = Flag
        [] State|NextStates then
          Matched = {Match State Announce Conditions $}
          NewFlag = {Bool.'or' Flag Matched $}
        in
          if Matched then
            {Raise State Announce}
          end
          
          {Loop NextStates NewFlag}
        end
      end
    in
      {Loop States false}
    end

    proc {Match State Announce Conditions ?Result}
      case Conditions of nil then Result = true
      [] Condition|NextConditions then
        case State of state(
          id: Id
          kindId: KindId
          generationId: GenerationId

          priority: Priority
          type: Type

          runnable: Runnable
          terminated: Terminated
          dead: Dead
          preempted: Preempted
          preemptible: Preemptible
        ) then
          proc {ContinueOrReturn Predicate}
            if Predicate then {Match State Announce NextConditions Result}
            else Result = false end
          end
        in
          case Condition of inserted then {ContinueOrReturn (Announce == inserted)}
          [] removed then {ContinueOrReturn (Announce == removed)}
          [] updated then {ContinueOrReturn (Announce == updated)}
          [] collected then {ContinueOrReturn (Announce == collected)}
          
          [] id(ThisId) then {ContinueOrReturn (ThisId == Id)}
          [] kindId(ThisKindId) then {ContinueOrReturn (ThisKindId == KindId)}
          [] generationId(ThisGenerationId) then {ContinueOrReturn (ThisGenerationId == GenerationId)}

          [] priority(ThisPriority) then {ContinueOrReturn (ThisPriority == Priority)}
          [] type(ThisType) then {ContinueOrReturn (ThisType == Type)}

          [] runnable then {ContinueOrReturn Runnable}
          [] preempted then {ContinueOrReturn Preempted}
          [] preemptible then {ContinueOrReturn Preemptible}
          [] terminated then {ContinueOrReturn Terminated}
          [] dead then {ContinueOrReturn Dead}
          end
        end
      end
    end

    proc {Raise State Announce}
      case State of state(
        id: Id
        kindId: KindId
        generationId: GenerationId

        priority: Priority
        type: Type

        runnable: Runnable
        terminated: Terminated
        dead: Dead
        preempted: Preempted
        preemptible: Preemptible
      ) then
        {PrintAlarm Alarm AlarmId Announce "thread "#{Int.toString Id $}}
      end
    end
  in
    FoundRaisedAlarm = (
      {ForEach InsertedRunnables inserted Conditions Match Raise $} orelse
      {ForEach RemovedRunnables removed Conditions Match Raise $} orelse
      {ForEach UpdatedRunnables updated Conditions Match Raise $} orelse
      {ForEach CollectedRunnables collected Conditions Match Raise $}
    )
  end

  proc {ParseVariablesAlarm Alarm AlarmId Conditions ?FoundRaisedAlarm}    
    proc {Match State Announce VariableType Conditions ?Result}
      case Conditions of nil then Result = true
      [] Condition|NextConditions then
        proc {ContinueOrReturn Predicate}
          if Predicate then {Match State Announce VariableType NextConditions Result}
          else Result = false end
        end
      in
        case State of variable(
          id: Id
          type: Type

          isBound: IsBound
          isNeeded: IsNeeded
          isWaited: IsWaited

          ...
        ) then
          case Condition of created then {ContinueOrReturn (Announce == created)}
          [] needed then {ContinueOrReturn (Announce == needed)}
          [] bound then {ContinueOrReturn (Announce == bound)}
          [] collected then {ContinueOrReturn (Announce == collected)}
          [] waited then {ContinueOrReturn (Announce == waited)}
          
          [] id(ThisId) then {ContinueOrReturn (ThisId == Id)}
          [] type(ThisType) then {ContinueOrReturn (ThisType == Type)}
          
          [] isBound then {ContinueOrReturn IsBound}
          [] isNeeded then {ContinueOrReturn IsNeeded}
          [] isWaited then {ContinueOrReturn IsWaited}
          end
        end
      end
    end

    proc {Raise State Announce VariableType}
      proc {GetWaiterString Waiter ?Result}
        case Waiter of state(
          id: Id

          priority: Priority
          type: Type

          ...
        ) then
          Result = "thread "#
            {Int.toString Id $}#
            " ("#
            {Atom.toString Type $}#
            ", "#
            {Atom.toString Priority}#
            ")"
        [] variable(
          id: Id
          type: Type
          ...
        ) then
          Result = VariableType#" "#
            {Int.toString Id $}#" ("#
            {Atom.toString Type $}#")"
        else
          Result = Waiter
        end
      end
    in
      case State of variable(
        id: Id
        type: Type

        isBound: IsBound
        isNeeded: IsNeeded
        isWaited: IsWaited
      ) then
        {PrintAlarm Alarm AlarmId Announce VariableType#" "#{Int.toString Id $}}
      [] variable(
        id: Id
        type: Type

        isBound: IsBound
        isNeeded: IsNeeded
        isWaited: IsWaited

        waiter: Waiter
      ) then
        String = VariableType#" "#{Int.toString Id $}#
          " waited by "#{GetWaiterString Waiter $}
      in
        {PrintAlarm Alarm AlarmId Announce String}
      [] variable(
        id: Id
        type: Type

        isBound: IsBound
        isNeeded: IsNeeded
        isWaited: IsWaited

        destinationNodeId: _
        sourceNodeId: SourceNodeId
      ) then
        String = VariableType#" "#{Int.toString Id $}#
          " bound from node "#{Int.toString SourceNodeId $}%#
          %" to node "#{Int.toString DestinationNodeId $}
      in
        {PrintAlarm Alarm AlarmId Announce String}
      end
    end

    proc {ForEvery Journals VariableTypes Announces ?FoundRaisedAlarm}
      proc {ForEachVariable States VariableType Announce Conditions
        Flag ?FoundRaisedAlarm}
        case States of nil then FoundRaisedAlarm = Flag
        [] State|NextStates then
          Matched = {Match State Announce VariableType Conditions $}
          NewFlag = {Bool.'or' Flag Matched $}
        in
          if Matched then
            {Raise State Announce VariableType}
          end

          {ForEachVariable NextStates VariableType Announce Conditions
            NewFlag FoundRaisedAlarm}
        end
      end

      proc {ForEachAnnounce Journal VariableType Announces
        Flag ?FoundRaisedAlarm}
        case Announces of nil then FoundRaisedAlarm = Flag
        [] Announce|NextAnnounces then
          Matched = {ForEachVariable Journal.Announce VariableType Announce
            Conditions false $}
          NewFlag = {Bool.'or' Flag Matched $}
        in
          {ForEachAnnounce Journal VariableType NextAnnounces
            NewFlag FoundRaisedAlarm}
        end
      end

      proc {ForEachJournal Journals VariableTypes
        Flag ?FoundRaisedAlarm}
        case Journals#VariableTypes of nil#nil then FoundRaisedAlarm = Flag
        [] (Journal|NextJournals)#(VariableType|NextVariablesTypes) then
          Matched = {ForEachAnnounce Journal VariableType Announces false $}
          NewFlag = {Bool.'or' Flag Matched $}
        in
          {ForEachJournal NextJournals NextVariablesTypes
            NewFlag FoundRaisedAlarm}
        end
      end
    in
      FoundRaisedAlarm = {ForEachJournal Journals VariableTypes false $}
    end
  in
    FoundRaisedAlarm = {ForEvery
      [OptVariablesJournal VariablesJournal ReadOnlyVariablesJournal]
      ["Opt variable" "variable" "read-only variable"]
      [created collected needed bound waited]
      $}
  end

  proc {ParseAlarm Alarm AlarmId ?RaisedActiveAlarm}
    case Alarm of alarm(
      type: Type
      aggregate: Aggregate
      conditions: Conditions
    ) then
      IsActiveAlarm = (Type == active)

      RaisedAlarm
    in
      case Aggregate of threads then
        RaisedAlarm = {ParseThreadsAlarm Alarm AlarmId Conditions $}
      [] variables then
        RaisedAlarm = {ParseVariablesAlarm Alarm AlarmId Conditions $}
      end

      RaisedActiveAlarm = {Bool.'and' IsActiveAlarm RaisedAlarm $}
    end
  end
in
  proc {ParseAlarms Alarms ?Result}
    proc {Loop Alarms AlarmId FoundRaisedActiveAlarm}
      case Alarms of nil then Result = FoundRaisedActiveAlarm
      [] Alarm|NextAlarms then
        RaisedAlarm = {ParseAlarm Alarm AlarmId $}
        NewAlarmId = AlarmId - 1
        NewFoundRaisedActiveAlarm = {Bool.'or' FoundRaisedActiveAlarm RaisedAlarm $}
      in
        {Loop NextAlarms NewAlarmId NewFoundRaisedActiveAlarm}
      end
    end

    FirstAlarmId = ({List.length Alarms $} - 1) % Alarms are listed in reverse order
  in
    {Loop Alarms FirstAlarmId false}
  end
end