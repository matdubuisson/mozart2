local
  % JS style

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

  proc {PrintOther Status String}
    {PrintAux Status String false}
  end
end