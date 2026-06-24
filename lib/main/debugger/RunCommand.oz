local
  proc {DisplayOptions}
    {PrintInfo "schedules <x> <skipSystemThreads=true>\tschedules x threads"}
    {PrintInfo "operations <x> <skipSystemThreads=true>\texecutes x threads"}
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
        SkipSystemThreads
      in
        case NextArguments of nil then
          SkipSystemThreads = true
        [] SkipSystemThreadsString|_ then
          SkipSystemThreads = {ExtractInput bool SkipSystemThreadsString none $}

          if SkipSystemThreads == none then
            {PrintError "Provided second parameter "#Thing#" flag '"#SkipSystemThreadsString#"' is not an integer"}
          end
        end

        if SkipSystemThreads \= none then
          {RunN Count SkipSystemThreads}
          {Boot_Thread.preempt This}
        end
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
      {HandleOption "schedules" Boot_VirtualMachine.runNSchedules NextArguments}      
    [] "operations" then
      {HandleOption "operations" Boot_VirtualMachine.runNOperations NextArguments}
    else
      {PrintError "Unexpected run option '"#Argument#"'"#TRYHELP}
    end
  end
end