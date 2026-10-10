proc {DisplayFrame Label Rows}
  proc {GetMaxLength Rows Length ?Result}
    case Rows of nil then Result = Length
    [] Label|_|NextRows then
      NewLength = {List.length Label $}
    in
      {GetMaxLength NextRows
        if NewLength > Length then NewLength else Length end
        Result}
    end
  end

  proc {Display Rows Length}
    case Rows of nil then skip
    [] Label|Value|NextRows then
      {PrintExactly Label Length}
      {PrintLn ": "#Value}
      {Display NextRows Length}
    end
  end

  MaxLength = {GetMaxLength Rows 0 $}
in
  {PrintLn Label}
  {PrintTab "─" {List.length Label $}}
  {PrintLn ""}
  {Display Rows (MaxLength + 1)}
end

proc {DisplayCSV Labels Elements Get Filter Format}
  proc {DisplayLabels Labels}
    case Labels of nil then {PrintLn "|"}
    [] Label|NextLabels then
      {Print "| "#Label#" "}
      {DisplayLabels NextLabels}
    end
  end

  proc {DisplayLine Labels}
    proc {GetTotalLength Labels Acc ?Result}
      case Labels of nil then Result = Acc
      [] Label|NextLabels then
        NewAcc = Acc + {List.length Label $} + 3
      in
        {GetTotalLength NextLabels NewAcc Result}
      end
    end

    Length = {GetTotalLength Labels 1 $}
  in
    {PrintTab "─" Length}
    {PrintLn ""}
  end

  proc {DisplayFeatures Labels Features}
    case Labels#Features of nil#nil then {PrintLn "|"}
    [] (Label|NextLabels)#(Feature|NextFeatures) then
      Length = {List.length Label $}
    in
      {Print "| "}
      {PrintExactly Feature Length}
      {Print " "}
      {DisplayFeatures NextLabels NextFeatures}
    end
  end
  
  proc {DisplayElements Elements Acc ?Total}
    case Elements of nil then Total = Acc
    [] Element|NextElements then
      Record = {Get Element $}
      Predicate = {Filter Record $}
    in
      if Predicate == none then Total = ~1
      elseif Predicate then
        {DisplayFeatures Labels {Format Record $}}
        {DisplayElements NextElements (Acc + 1) Total}
      else {DisplayElements NextElements Acc Total} end
    end
  end
in
  {DisplayLabels Labels}
  {DisplayLine Labels}

  local
    HowMany = {DisplayElements Elements 0 $}
  in
    if HowMany > 0 then
      {PrintLn ""}
      {PrintLn HowMany#" elements listed"}
    end
  end
end
