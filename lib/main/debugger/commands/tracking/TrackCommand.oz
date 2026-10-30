local
  proc {DisplayOptions}
    {DisplayFrame "Track command options" [
      "runnable <created|updated|collected|scheduled|unscheduled|rescheduled> <authorId|none> <victimIds>" "add a new tracking rule detecting some runnables/threads activities"
      "variable <created|collected|needed|waited|bound> <authorId|none> <victimIds>" "add a new tracking rule detecting some variables activities"
    ]}
  end

  proc {DisplayRunnableOptions}
    {DisplayFrame "Track runnable option arguments" [
      "created <authorId|none> <victimIds>" "detect a created thread from the last schedule"
      "updated <authorId|none> <victimIds>" "detect a updated thread from the last schedule"
      "collected <authorId|none> <victimIds>" "detect a collected thread from the last schedule"
      "scheduled <authorId|none> <victimIds>" "detect a scheduled thread from the last schedule"
      "unscheduled <authorId|none> <victimIds>" "detect a unscheduled thread from the last schedule"
      "rescheduled <authorId|none> <victimIds>" "detect a rescheduled thread from the last schedule"
      "calling <authorId|none> <victimIds>" "detect a thread calling a certain abstraction, procedure or object"
    ]}
  end

  proc {DisplayVariableOptions}
    {DisplayFrame "Track variable option arguments" [
      "created <authorId|none> <victimIds>" "detect a created variable from the last schedule"
      "collected <authorId|none> <victimIds>" "detect a collected variable from the last schedule"
      "needed <authorId|none> <victimIds>" "detect a needed variable from the last schedule"
      "waited <authorId|none> <victimIds>" "detect a waited variable from the last schedule"
      "bound <authorId|none> <victimIds>" "detect a bound variable from the last schedule"
    ]}
  end

  proc {DisplayParameters}
    {DisplayFrame "Track parameters" [
      "<authorId|none> (<victimIds>)" "to be provided at the end of each command"
      "<authorId|none>" "tell which thread should generate this event, none if it can be any thread"
      "(<victimIds>)" "an optional list of ids telling which threads or variables should be the subject of this event, if the list is empty then any objects can be the subject"
    ]}
  end

  proc {HandleCaseNext Type Category AuthorId Arguments}
    case Arguments of nil then
      {Boot_EventManager.track Type Category AuthorId nil}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayParameters}
      else
        Ids = {ExtractIdentities Arguments $}
      in
        if {List.is Ids $} then
          {Boot_EventManager.track Type Category AuthorId Ids}
        end
      end
    end
  end

  proc {HandleCase Type Category Arguments}
    case Arguments of nil then
      {DisplayParameters}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayParameters}
      [] "none" then
        {HandleCaseNext Type Category none NextArguments}
      else
        Id = {ExtractIdentity Argument $}
      in
        if Id \= none then
          {HandleCaseNext Type Category Id NextArguments}
        end
      end
    end
  end

  proc {HandleRunnableOption Arguments}
    case Arguments of nil then
      {DisplayRunnableOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayRunnableOptions}
      [] "created" then
        {HandleCase runnable created NextArguments}
      [] "updated" then
        {HandleCase runnable updated NextArguments}
      [] "collected" then
        {HandleCase runnable collected NextArguments}
      [] "scheduled" then
        {HandleCase runnable scheduled NextArguments}
      [] "unscheduled" then
        {HandleCase runnable unscheduled NextArguments}
      [] "rescheduled" then
        {HandleCase runnable rescheduled NextArguments}
      [] "calling" then
        {HandleCase runnable calling NextArguments}
      else
        {PrintUnexpectedOptionError Argument}
      end
    end
  end

  proc {HandleVariableOption Arguments}
    case Arguments of nil then
      {DisplayVariableOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayVariableOptions}
      [] "created" then
        {HandleCase variable created NextArguments}
      [] "collected" then
        {HandleCase variable collected NextArguments}
      [] "needed" then
        {HandleCase variable needed NextArguments}
      [] "waited" then
        {HandleCase variable waited NextArguments}
      [] "bound" then
        {HandleCase variable bound NextArguments}
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
    [] "runnable" then
      {HandleRunnableOption NextArguments}
    [] "variable" then
      {HandleVariableOption NextArguments}
    else
      {PrintUnexpectedOptionError Argument}
    end
  end
end