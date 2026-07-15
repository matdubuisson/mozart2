proc {FormatNode Node ?Result}
  proc {GetPrintableByteString BString ?Result}
    Length = {ByteString.length BString $}
    proc {ForI I Acc ?Result}
      if I >= Length then Result = Acc
      else
        Value = {ByteString.get BString I $}
      in
        {ForI I + 1 Value + Acc Result}
      end
    end
  in
    Result = {ForI 0 0 $}
  end
in
  case Node of node(
    name: Name
    bindingPriority: BindingPriority
    structuralBehavior: StructuralBehavior
    copyable: Copyable
    transient: Transient
    feature: Feature
    uuid: Uuid
    value: Value
    ) then
    Result = [
      {Atom.toString Name $}
      {Int.toString BindingPriority $}
      {Atom.toString StructuralBehavior $}
      {Bool.toString Copyable $}
      {Bool.toString Transient $}
      {Bool.toString Feature $}
      {Int.toString {GetPrintableByteString Uuid $} $}
      Value
    ]
  end
end