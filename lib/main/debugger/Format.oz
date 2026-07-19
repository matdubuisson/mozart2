proc {FormatThreadState State ?Result}
  case State of state(
    id: Id
    kindId: KindId
    generationId: GenerationId

    priority: Priority
    type: Type

    runnable: Runnable
    terminated: Terminated
    dead: Dead
    preempted: Preempted
    preemptible: Preemptible
  ) then
    Result = [
      {Int.toString Id $}
      {Int.toString KindId $}
      {Int.toString GenerationId $}

      {Atom.toString Priority $}
      {Atom.toString Type $}

      {Bool.toString Runnable $}
      {Bool.toString Terminated $}
      {Bool.toString Dead $}
      {Bool.toString Preempted $}
      {Bool.toString Preemptible $}
    ]
  end
end

proc {FormatThreadStatistics Statistics ?Result}
  case Statistics of statistics(
    id: Id
    runsCount: RunsCount
    resumesCount: ResumesCount
    suspendsCount: SuspendsCount
    suspendsOnVarCount: SuspendsOnVarCount
    operationsCount: OperationsCount
    bindsCount: BindsCount
  ) then
    Result = [
      {Int.toString Id $}
      {Int.toString RunsCount $}
      {Int.toString ResumesCount $}
      {Int.toString SuspendsCount $}
      {Int.toString SuspendsOnVarCount $}
      {Int.toString OperationsCount $}
      {Int.toString BindsCount $}
    ]
  end
end

proc {FormatThreadNodesCounts NodesCounts ?Result}
  case NodesCounts of nodes(
    id: Id
    variableNodesCount: VariableNodesCount
    valueNodesCount: ValueNodesCount
    structuralNodesCount: StructuralNodesCount
    tokenNodesCount: TokenNodesCount
    stableNodesCount: StableNodesCount
    unstableNodesCount: UnstableNodesCount
    xNodesCount: XNodesCount
    yNodesCount: YNodesCount
    gNodesCount: GNodesCount
    kNodesCount: KNodesCount
    stackDepth: StackDepth
    nodesCount: NodesCount
  ) then
    Result = [
      {Int.toString Id $}
      {Int.toString VariableNodesCount $}
      {Int.toString ValueNodesCount $}
      {Int.toString StructuralNodesCount $}
      {Int.toString TokenNodesCount $}
      {Int.toString StableNodesCount $}
      {Int.toString UnstableNodesCount $}
      {Int.toString XNodesCount $}
      {Int.toString YNodesCount $}
      {Int.toString GNodesCount $}
      {Int.toString KNodesCount $}
      {Int.toString StackDepth $}
      {Int.toString NodesCount $}
    ]
  end
end

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
    id: Id
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
      {Int.toString Id $}
      {Atom.toString Name $}
      {Int.toString BindingPriority $}
      {Atom.toString StructuralBehavior $}
      {Bool.toString Copyable $}
      {Bool.toString Transient $}
      {Bool.toString Feature $}
      {Int.toString {GetPrintableByteString Uuid $} $}
      {ExtractString Value $}
    ]
  end
end

proc {FormatList List ?Result}
  case List of list(
    id: Id
    hash: Hash
    repr: Repr
  ) then
    Result = [
      {Int.toString Id $}
      {Int.toString Hash $}
      Repr
    ]
  end
end
