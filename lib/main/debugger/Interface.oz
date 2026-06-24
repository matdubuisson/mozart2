proc {ExtractInput Type Argument DefaultValue ?Result}
  proc {Convert Type F Argument ?Value}
    try
      Value = {F Argument $}
    catch _ then
      Value = DefaultValue
    end
  end
in
  case Type of atom then
    Result = {String.toAtom Argument $}
  [] string then
    Result = Argument
  [] bool then
    Result = {String.toAtom Argument $}
  [] int then
    Result = {Convert Type String.toInt Argument $}
  [] float then
    Result = {Convert Type String.toFloat Argument $}
  end
end