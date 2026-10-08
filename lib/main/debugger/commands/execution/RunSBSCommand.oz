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
      NewHash ThisList
    in
      {ForEachList Id Hash Lists NewHash ThisList}

      local
        IsHashChanged = (Hash \= NewHash)
        NewFlag = {Bool.'or' Flag IsHashChanged $}
        NewLists
      in
        if IsHashChanged then
          {Boot_System.printVS "List updated, size: "#{Int.toString {List.length ThisList $} $} false true}
        end

        Lists = ThisList|NewLists
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
        ["Id" "Hash" "Owners" "Repr"]
        FilteredLists 10 FormatList
        [true true true false]}
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