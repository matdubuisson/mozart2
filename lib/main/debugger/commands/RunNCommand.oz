proc {HandleRunN Thing RunN Arguments}
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