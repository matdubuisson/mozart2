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
    else
      {PrintError "Unexpected run option '"#Argument#"'"#TRYHELP}
    end
  end
end