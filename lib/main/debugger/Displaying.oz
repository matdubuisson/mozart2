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

local
  proc {DisplayLabels Labels}
    case Labels of nil then {PrintLn "|"}
    [] flexible(Label)|NextLabels then
      {Print "| "#Label#" "}
      {DisplayLabels NextLabels}
    [] Label|NextLabels then
      {Print "| "#Label#" "}
      {DisplayLabels NextLabels}
    end
  end

  proc {DisplayLine Labels}
    proc {GetTotalLength Labels Acc ?Result}
      case Labels of nil then Result = Acc
      [] flexible(Label)|NextLabels then
        NewAcc = Acc + {List.length Label $} + 3
      in
        {GetTotalLength NextLabels NewAcc Result}
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
    [] (flexible(Label)|NextLabels)#(Feature|NextFeatures) then
      Length = {List.length Label $}
    in
      {Print "| "}
      {PrintAtLeast Feature Length}
      {Print " "}
      {DisplayFeatures NextLabels NextFeatures}
    [] (Label|NextLabels)#(Feature|NextFeatures) then
      Length = {List.length Label $}
    in
      {Print "| "}
      {PrintExactly Feature Length}
      {Print " "}
      {DisplayFeatures NextLabels NextFeatures}
    end
  end
  
  proc {DisplayElements Labels Elements Get Filter Format Acc ?Total}
    case Elements of nil then Total = Acc
    [] Element|NextElements then
      Record = {Get Element $}
      Predicate = {Filter Record $}
    in
      if Predicate == none then Total = ~1
      elseif Predicate then
        {DisplayFeatures Labels {Format Record $}}
        {DisplayElements Labels NextElements Get Filter Format (Acc + 1) Total}
      else
        {DisplayElements Labels NextElements Get Filter Format Acc Total}
      end
    end
  end
in
  proc {DisplayCSV Labels Elements Get Filter Format}
    {DisplayLabels Labels}
    {DisplayLine Labels}

    local
      HowMany = {DisplayElements Labels Elements Get Filter Format 0 $}
    in
      if HowMany > 0 then
        {PrintLn ""}
        {PrintLn HowMany#" elements listed"}
      end
    end
  end

  proc {DisplayJournalCSV Labels Journal Get Filter Format}
    Types = {Record.arity Journal $}

    proc {ForEachCategory Type Categories Acc ?Total}
      case Categories of nil then Total = Acc
      [] Category|NextCategories then
        HowMany = {DisplayElements Labels Journal.Type.Category
        proc {$ Record ?Result}
          Result = {Get Type Category Record $}
        end
        Filter Format 0 $}
      in
        {ForEachCategory Type NextCategories (Acc + HowMany) Total}
      end
    end

    proc {ForEachType Types Acc ?Total}
      case Types of nil then Total = Acc
      [] Type|NextTypes then
        Categories = {Record.arity Journal.Type $}
        HowMany = {ForEachCategory Type Categories Acc $}
      in
        {ForEachType NextTypes HowMany Total}
      end
    end
  in
    {DisplayLabels Labels}
    {DisplayLine Labels}

    local
      Types = {Record.arity Journal $}
      HowMany = {ForEachType Types 0 $}
    in
      if HowMany > 0 then
        {PrintLn ""}
        {PrintLn HowMany#" elements listed"}
      end
    end
  end
end