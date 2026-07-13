local
  MARKER = "[DEBUGGER]"

  proc {PrintAux Status String IsError}
    case Status of none then
      {Boot_System.printVS MARKER#": "#
        String IsError true}
    else
      {Boot_System.printVS MARKER#"["#Status#"]"#": "#
        String IsError true}
    end
  end
in
  proc {PrintPadding Length}
    proc {ForI I}
      if I < Length then
        {Boot_System.printVS " " false false}
      else skip end
    end
  in
    {ForI 0}
  end

  proc {PrintPrefix}
    {Boot_System.printVS MARKER#": " false false}
  end

  proc {PrintOtherPrefix Status}
    {Boot_System.printVS MARKER#"["#Status#"]: " false false}
  end

  proc {Print String}
    {Boot_System.printVS String false false}
  end

  proc {PrintLn String}
    {Print String#"\n"}
  end

  proc {PrintTab Character Size}
    proc {Loop I}
      if I == 0 then skip
      else
        {Print Character}
        {Loop I - 1}
      end
    end
  in
    {Loop Size}
  end

  proc {PrintExactly String Size}
    Length = {List.length String $}
  in
    if Length == Size then {Print String}
    elseif Length > Size then
      {Print {List.take String Size $}}
    else
      {Print String}
      {PrintTab " " Size - Length}
    end
  end

  proc {PrintList List}
    proc {Loop List}
      case List of nil then skip
      [] Head|Tail then
        String
      in
        if {Bool.is Head $} then String = {Bool.toString Head $}
        elseif {Int.is Head $} then String = {Int.toString Head $}
        elseif {Float.is Head $} then String = {Float.toString Head $}
        elseif {Atom.is Head $} then String = {Atom.toString Head $}
        elseif {Record.is Head $} then
          String = {Boot_System.getRepr Head ~1 ~1 $}
        else String = Head end

        {Print String}
        if Tail \= nil then {Print " "} end
        {Loop Tail}
      end
    end
  in
    {Print "["}
    {Loop List}
    {Print "]"}
  end

  proc {PrintLog String}
    {PrintAux none String false}
  end

  proc {PrintInfo String}
    {PrintAux "INFO" String false}
  end

  proc {PrintWarning String}
    {PrintAux "WARNING" String false}
  end

  proc {PrintError String}
    {PrintAux "ERROR" String true}
  end

  proc {PrintAlarm Alarm AlarmId Announce String}
    case Alarm of alarm(
      type: _
      aggregate: Aggregate
      conditions: Conditions
    ) then
      {PrintOtherPrefix "ALARM["#
        {Int.toString AlarmId $}#
        ", "#
        {Atom.toString Announce $}#
        "]"}
      {Print {Atom.toString Aggregate $}#", "}
      {PrintList Conditions}
      {PrintLn " => "#String}
    end
  end

  proc {PrintOther Status String}
    {PrintAux Status String false}
  end
end