local
  proc {DisplayOptions}
    {PrintInfo "schedules <x> <includeSystemThreads=false>\tschedules x threads"}
    {PrintInfo "operations <x> <includeSystemThreads=false>\texecutes x threads"}
    {PrintInfo "operationByOperation (alias obo)\tEnters the operation by operation execution mode"}
  end

  proc {HandleOption Thing RunN Arguments}
    case Arguments of nil then
      {PrintError "Missing "#Thing#" count"}
    [] CountString|NextArguments then
      Count = {ExtractInput int CountString none $}
    in
      if Count == none then
        {PrintError "Provided first parameter "#Thing#" count '"#CountString#"' is not an integer"}
      else
        {RunN Count}
        {Boot_Thread.preempt This}
      end
    end
  end

  proc {HandleOperationByOperation Arguments}
    proc {Loop}
      NextThread = {Boot_Introspection.getNextScheduledThread false $}
      Id = {Boot_Thread.getId NextThread $}
      Preemptible = {Boot_Thread.isPreemptible NextThread $}
      Priority = {Boot_Thread.getPriority NextThread $}
      Statistics = {Boot_Introspection.getThreadStatistics NextThread $}

      NextOperation = {Boot_Introspection.getNextOperation false $}

      proc {PrintArguments Arguments}
        case Arguments of nil then skip
        [] Argument|NextArguments then
          case Argument of operationArgument(
            index: Index
            type: Type
            image: Image
          ) then
            {Boot_System.printVS {Atom.toString Type $}#"["#Index#"]("#Image#")" false false}
          end

          case NextArguments of _|_ then
            {Boot_System.printVS ", " false false}
          else skip end

          {PrintArguments NextArguments}
        end
      end
    in
      {PrintOtherPrefix "OBO"}

      if NextThread == none then
        {PrintWarning "No remaining thread to schedule"}
      else
        case NextOperation of operation(
          name: Name
          opCode: OpCode
          arguments: Arguments
        ) then
          {Boot_System.printVS "thread "#
            {Int.toString Id $}#" "#
            {Atom.toString Priority $}#" "#
            if Preemptible then "preemptible" else "non-preemptible" end #" "#
            {Int.toString Statistics.operationsCount $}#" ops "#
            {Int.toString Statistics.runsCount $}#" runs => "#
            Name#"("#OpCode#") "
            false false}

          {PrintArguments Arguments}

          {Boot_System.printVS "" false true}
        end

        {PrintOtherPrefix "OBO"}

        local
          Command = {Boot_System.inputVSLine $}
        in
          case Command of "help" then
            {PrintInfo "<empty-string>\texecutes one more operation"}
            {PrintInfo "status\tdisplays the current scheduler status"}
            {PrintInfo "exit\texits the operation by operation execution mode"}
            {Loop}
          [] "" then
            {Boot_Thread.preempt This}
            {Loop}
          [] "status" then
            SchedulesCounter = {Boot_Introspection.getSchedulesCounter $}
            OperationsCounter = {Boot_Introspection.getOperationsCounter $}
            ThreadsCount = {Boot_Introspection.getThreadsCount $}
          in
            {PrintInfo "\tSchedules counter: "#
              {Int.toString SchedulesCounter $}}
            {PrintInfo "\tOperations counter: "#
              {Int.toString OperationsCounter $}}
            {PrintInfo "\tThreads counter: "#
              {Int.toString ThreadsCount $}}
            
            {PrintInfo "Current thread "#Id#" activity:"}
            case Statistics of statistics(
              bindsCount: BindsCount
              operationsCount: OperationsCount
              resumesCount: ResumesCount
              runsCount: RunsCount
              suspendsCount: SuspendsCount
              suspendsOnVarCount: SuspendsOnVarCount
            ) then
              {PrintInfo "\tBindsCount: "#
                {Int.toString BindsCount $}}
              {PrintInfo "\tOperationsCount: "#
                {Int.toString OperationsCount $}}
              {PrintInfo "\tResumesCount: "#
                {Int.toString ResumesCount $}}
              {PrintInfo "\tRunsCount: "#
                {Int.toString RunsCount $}}
              {PrintInfo "\tSuspendsCount: "#
                {Int.toString SuspendsCount $}}
              {PrintInfo "\tSuspendsOnVarCount: "#
                {Int.toString SuspendsOnVarCount $}}
            end

            {Loop}
          [] "exit" then skip
          else
            {PrintError "Invalid command '"#Command#"'"#TRYHELP}
            {Loop}
          end
        end
      end
    end
  in
    {Boot_Scheduler.runOperationByOperation}
    {Loop}
    {Boot_Scheduler.reset}
  end

  proc {DisplayStepByStepOptions}
    {PrintInfo "Step by step options"}
  end

  proc {HandleStepByStepLists Ids}
    proc {ForEachList Id Hash Lists ?NewHash ?List}
      case Lists of nil then
        NewHash = ~1
        List = list(id:Id hash:~1 repr:"collected")
      [] ThisList|NextLists then
        case ThisList of list(
          id: ThisId
          hash: ThisHash
          ...
        ) then
          if Id == ThisId then
            NewHash = Hash
            List = ThisList
            {Boot_System.printRepr List false true}
          else
            {ForEachList Id Hash NextLists NewHash List}
          end
        end
      end
    end

    proc {ForEachHash Ids Hashes Lists Flag ?Lists ?RequestStop}
      case Ids#Hashes of nil#nil then
        Lists = nil
        RequestStop = Flag
      [] (Id|NextIds)#(Hash|NextHashes) then
        NewHash List
      in
        {ForEachList Id Hash Lists NewHash List}

        local
          IsHashChanged = (Hash \= NewHash)
          NewFlag = {Bool.'or' Flag IsHashChanged $}
          NewLists
        in
          Lists = List|NewLists
          {ForEachHash NextIds NextHashes Lists NewFlag NewLists RequestStop}
        end
      end
    end

    proc {Loop Ids Hashes}
      Lists = {Boot_Introspection.getLists $}
      RequestStop
    in
      local FilteredLists in
        {ForEachHash Ids Hashes Lists false FilteredLists RequestStop}

        {MaskedDisplayCSV
          ["Id" "Hash" "Repr"]
          FilteredLists 10 FormatList
          [true true false]}
      end

      if RequestStop then
        {PrintOtherPrefix "SBS"}

        local
          Command = {Boot_System.inputVSLine $}
        in
          case Command of "help" then
            {PrintInfo "<empty-string>\texecutes one more step"}
            {PrintInfo "status\tdisplays the current scheduler status"}
            {PrintInfo "exit\texits the step by step execution mode"}
            {Loop Ids Hashes}
          [] "" then
            {Boot_Thread.preempt This}
            {Loop Ids Hashes}
          [] "status" then
            SchedulesCounter = {Boot_Introspection.getSchedulesCounter $}
            OperationsCounter = {Boot_Introspection.getOperationsCounter $}
            ThreadsCount = {Boot_Introspection.getThreadsCount $}
          in
            {PrintInfo "\tSchedules counter: "#
              {Int.toString SchedulesCounter $}}
            {PrintInfo "\tOperations counter: "#
              {Int.toString OperationsCounter $}}
            {PrintInfo "\tThreads counter: "#
              {Int.toString ThreadsCount $}}
            {Loop Ids Hashes}
          [] "exit" then skip
          else
            {PrintError "Invalid command '"#Command#"'"#TRYHELP}
            {Loop Ids Hashes}
          end
        end
      else
        {Boot_Thread.preempt This}
        {Loop Ids Hashes}
      end
    end

    proc {InitHashes Size ?Hashes}
      if Size == 0 then Hashes = nil
      else
        NewHashes
      in
        Hashes = 0|NewHashes
        {InitHashes (Size - 1) NewHashes}
      end
    end
  in
    {Loop Ids
      {InitHashes {List.length Ids $} $}}
  end

  proc {HandleStepByStep Arguments}
    case Arguments of nil then
      {DisplayStepByStepOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayStepByStepOptions}
      [] "lists" then
        Error
        Ids = {ExtractInputsWithError int NextArguments none Error $}
      in
        if Error then
          {PrintError "Not all provided ids are valid"}
        else
          {HandleStepByStepLists Ids}
        end
      else
        {PrintUnexpectedOptionError Argument}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    [] "schedules" then
      {HandleOption "schedules" Boot_Scheduler.runNSchedules NextArguments}      
    [] "operations" then
      {HandleOption "operations" Boot_Scheduler.runNOperations NextArguments}
    [] "operationByOperation" then
      {HandleOperationByOperation NextArguments}
    [] "obo" then
      {HandleOperationByOperation NextArguments}
    [] "stepByStep" then
      {HandleStepByStep NextArguments}
    [] "sbs" then
      {HandleStepByStep NextArguments}
    [] "untilAlarm" then
      {Cell.assign ModeCell false}
      {Boot_Thread.preempt This}
    else
      {PrintError "Unexpected run option '"#Argument#"'"#TRYHELP}
    end
  end
end