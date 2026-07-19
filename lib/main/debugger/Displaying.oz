proc {DisplayNameDescriptions Names Descriptions}
  case Names#Descriptions of nil#nil then skip
  [] (Name|NextNames)#(Description|NextDescriptions) then
    {PrintOtherPrefix "OPTION"}
    {PrintExactly Name 20}
    {PrintLn " "#Description}
    {DisplayOptions NextNames NextDescriptions}
  end
end

DisplayOptions = DisplayNameDescriptions

proc {IdsToString Ids ?String}
  proc {Aux Ids AccumulatedString ?String}
    case Ids of nil then String = AccumulatedString
    [] Id|NextIds then
      NewString = AccumulatedString#{Int.toString Id $}
    in
      if NextIds == nil then
        {Aux NextIds NewString String}
      else
        {Aux NextIds NewString#" " String}
      end
    end
  end
in
  String = "["#{Aux Ids "" $}#"]"
end

proc {DisplayThreadState Tab State}
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
    {PrintInfo Tab#"Thread "#Id#":"}
    
    local
      proc {Print String}
        {PrintInfo Tab#TAB#String}
      end
    in
      {Print "Id: "#
        {Int.toString Id $}}
      {Print "KindId: "#
        {Int.toString KindId $}}
      {Print "GenerationId: "#
        {Int.toString GenerationId $}}

      {Print "Priority: "#
        {Atom.toString Priority $}}
      {Print "Type: "#
        {Atom.toString Type $}}

      {Print "Runnable: "#
        {Bool.toString Runnable $}}
      {Print "Terminated: "#
        {Bool.toString Terminated $}}
      {Print "Dead: "#
        {Bool.toString Dead $}}
      {Print "Preempted: "#
        {Bool.toString Preempted $}}
      {Print "Preemptible: "#
        {Bool.toString Preemptible $}}
    end
  end
end

proc {DisplayThreadStatistics Tab Statistics}
  case Statistics of statistics(
    id: _
    runsCount: RunsCount
    resumesCount: ResumesCount
    suspendsCount: SuspendsCount
    suspendsOnVarCount: SuspendsOnVarCount
    operationsCount: OperationsCount
    bindsCount: BindsCount
  ) then        
    {PrintInfo Tab#"Statistics:"}

    local
      proc {Print String}
        {PrintInfo Tab#TAB#String}
      end
    in
      {Print "Runs: "#RunsCount}
      {Print "Resumes: "#ResumesCount}
      {Print "Suspends: "#SuspendsCount}
      {Print "SuspendsOnVar: "#SuspendsOnVarCount}
      {Print "Operations: "#OperationsCount}
      {Print "Binds: "#BindsCount}
    end
  end
end

proc {DisplayThreadNodes Tab Nodes}
  case Nodes of nodes(
    id: _
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
    {PrintInfo Tab#"Nodes:"}

    local
      proc {Print String}
        {PrintInfo Tab#TAB#String}
      end
    in
      {Print "Variable nodes: "#VariableNodesCount}
      {Print "Value nodes: "#ValueNodesCount}
      {Print "Structural nodes: "#StructuralNodesCount}
      {Print "Token nodes: "#TokenNodesCount}

      {Print "Stable nodes: "#StableNodesCount}
      {Print "Unstable nodes: "#UnstableNodesCount}

      {Print "X nodes: "#XNodesCount}
      {Print "Y nodes: "#YNodesCount}
      {Print "G nodes: "#GNodesCount}
      {Print "K nodes: "#KNodesCount}
      {Print "Stack depth: "#StackDepth}
      {Print "Total nodes: "#NodesCount}
    end
  end
end

proc {DisplayThread Tab Thread}
  State = {Boot_Introspection.getThreadStatus Thread $}
in
  case State of state(nodes:Nodes statistics:Statistics state:State) then
    {DisplayThreadState Tab State}
    {DisplayThreadStatistics Tab Statistics}
    {DisplayThreadNodes Tab Nodes}
  end
end

proc {DisplayVariable Tab Variable}
  case Variable of variable(
    id: Id
    type: Type
    isBound: IsBound
    isNeeded: IsNeeded
    candidates: Candidates
    pendings: Pendings
    value: Value
  ) then
    {PrintInfo Tab#"Variable "#{Int.toString Id $}#":"}

    local
      proc {Print String}
        {PrintInfo Tab#TAB#String}
      end
    in
      {Print "Type: "#
        {Atom.toString Type $}}
      
      {Print "IsBound: "#
        {Bool.toString IsBound $}}
      {Print "IsNeeded: "#
        {Bool.toString IsNeeded $}}

      {Print "Candidates: "#
        {IdsToString Candidates $}}

      {Print "Pendings: "#
        {IdsToString Pendings $}}

      {Print "Value: "#Value}
    end
  end
end

proc {MaskedDisplayCSV Labels Rows Width Format Masks}
  proc {DisplayRow Elements Masks}
    case Elements of nil then {PrintLn ""}
    [] Element|NextElements then
      case Masks of nil then
        {PrintExactly Element Width}
        {Print " "}

        {DisplayRow NextElements nil}
      [] none then
        {PrintExactly Element Width}
        {Print " "}

        {DisplayRow NextElements nil}
      [] Mask|NextMasks then
        if Mask then
          {PrintExactly Element Width}
          {Print " "}
        else
          {Print Element}
        end

        {DisplayRow NextElements NextMasks}
      end
    end
  end

  proc {DisplayRows Index Rows}
    case Rows of nil then skip
    [] Row|NextRows then
      {PrintExactly {Int.toString Index $} Width}
      {Print " "}

      {DisplayRow {Format Row $} if Masks == none then nil else Masks end}
      {DisplayRows (Index + 1) NextRows}
    end
  end
in
  {PrintInfo "New values: "#{Int.toString {List.length Rows $} $}}
  {PrintExactly "Index" Width}
  {Print " "}

  {DisplayRow Labels none}
  {DisplayRows 0 Rows}
end

proc {DisplayCSV Labels Rows Width Format}
  {MaskedDisplayCSV Labels Rows Width Format none}
end
