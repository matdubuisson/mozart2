local
  proc {DisplayOptions}
    {DisplayFrame "Untrack command options" [
      "runnable <created|updated|collected|scheduled|unscheduled|rescheduled> <ruleIndex>" "add a new tracking rule detecting some runnables/threads activities"
      "variable <created|collected|needed|waited|bound> <ruleIndex>" "add a new tracking rule detecting some variables activities"
    ]}
  end

  proc {DisplayRunnableOptions}
    {DisplayFrame "Untrack runnable option arguments" [
      "created <ruleIndex>" "detect a created thread from the last schedule"
      "updated <ruleIndex>" "detect a updated thread from the last schedule"
      "collected <ruleIndex>" "detect a collected thread from the last schedule"
      "scheduled <ruleIndex>" "detect a scheduled thread from the last schedule"
      "unscheduled <ruleIndex>" "detect a unscheduled thread from the last schedule"
      "rescheduled <ruleIndex>" "detect a rescheduled thread from the last schedule"
    ]}
  end

  proc {DisplayVariableOptions}
    {DisplayFrame "Untrack variable option arguments" [
      "created <ruleIndex>" "detect a created variable from the last schedule"
      "collected <ruleIndex>" "detect a collected variable from the last schedule"
      "needed <ruleIndex>" "detect a needed variable from the last schedule"
      "waited <ruleIndex>" "detect a waited variable from the last schedule"
      "bound <ruleIndex>" "detect a bound variable from the last schedule"
    ]}
  end

  proc {DisplayParameters}
    {DisplayFrame "Untrack parameters" [
      "<ruleIndex>" "the index of the rule to delete for the specified type and category"
    ]}
  end

  proc {HandleCase Type Category Arguments}
    case Arguments of nil then
      {DisplayParameters}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayParameters}
      else
        Index = {ExtractIndex Argument $}
      in
        if Index \= none then
          {Boot_EventManager.untrack Type Category Index}
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