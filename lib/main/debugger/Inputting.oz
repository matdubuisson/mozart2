local
  X = unit
in
  proc {InputCommand ?Command ?Arguments}
    {PrintPrefix}
    local
      Input = {Boot_System.inputVSLine $}
    in
      case Input of "" then
        Command = none Arguments = nil
      else
        Inputs = {String.tokens Input 32 $}
      in
        Command|Arguments = Inputs
      end
    end
  end
end