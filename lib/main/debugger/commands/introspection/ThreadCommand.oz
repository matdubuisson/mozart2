local
  proc {DisplayOptions}
    {DisplayFrame "Thread command options" [
      "<identity> state" "display the state of the specified thread(s)"
      "<identity> statistics" "display statistics related to the thread(s)"
      "<identity> nodes" "display the state nodes proportions related to thread(s)"
      "<identity> all" "display all available options just listed before"
      "<identity> depth" "display the thread stack depth"
      "<identity> register <type> (<depth>=0)" "display the state of a specific register owned by the specified thread"
      "<identity> registers" "display registers' nodes proportions towards the thread and the virtual machine"
    ]}
  end

  proc {DisplayState Id Thread}
    State = {Boot_Introspection.getThreadState Thread $}
  in
    case State of state(
      id: _
      kindId: KindId
      generationId: GenerationId
      priority: Priority

      runnable: Runnable
      terminated: Terminated
      dead: Dead
      preempted: Preempted
      preemptible: Preemptible
    ) then
      {DisplayFrame "Thread state" [
        "Id" Id
        "Kind Id" KindId
        "Generation Id" GenerationId
        "Priority" Priority

        "Is runnable" {Bool.toString Runnable $}
        "Is terminated" {Bool.toString Terminated $}
        "Is dead" {Bool.toString Dead $}
        "Is preempted" {Bool.toString Preempted $}
        "Is preemptible" {Bool.toString Preemptible $}
      ]}
    end
  end

  proc {DisplayStatistics Id Thread}
    Statistics = {Boot_Introspection.getThreadStatistics Thread $}
  in
    case Statistics of statistics(
      runsCount: RunsCount
      resumesCount: ResumesCount
      suspendsCount: SuspendsCount
      suspendsOnVarCount: SuspendsOnVarCount
      operationsCount: OperationsCount
      bindsCount: BindsCount
    ) then
      TotalCount = RunsCount + ResumesCount + SuspendsCount
        + SuspendsOnVarCount + OperationsCount + BindsCount
    in
      {DisplayFrame "Thread state" [
        "Id" Id

        {MakeStringWithPercent "Runs count" RunsCount TotalCount $} RunsCount
        {MakeStringWithPercent "Resumes count" ResumesCount TotalCount $} ResumesCount
        {MakeStringWithPercent "Suspends count" SuspendsCount TotalCount $} SuspendsCount
        {MakeStringWithPercent "Suspends on var count" SuspendsOnVarCount TotalCount $} SuspendsOnVarCount
        {MakeStringWithPercent "Operations count" OperationsCount TotalCount $} OperationsCount
        {MakeStringWithPercent "Binds count" BindsCount TotalCount $} BindsCount
      ]}
    end
  end

  proc {DisplayNodes Id Thread}
    Nodes = {Boot_Introspection.getThreadNodesCounts Thread $}
  in
    case Nodes of nodes(
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
      {DisplayFrame "Thread state" [
        "Id" Id

        {MakeStringWithPercent "Variable nodes count" VariableNodesCount NodesCount $} VariableNodesCount
        {MakeStringWithPercent "Value nodes count" ValueNodesCount NodesCount $} ValueNodesCount
        {MakeStringWithPercent "Structural nodes count" StructuralNodesCount NodesCount $} StructuralNodesCount
        {MakeStringWithPercent "Token nodes count" TokenNodesCount NodesCount $} TokenNodesCount
        
        {MakeStringWithPercent "Stable nodes count" StableNodesCount NodesCount $} StableNodesCount
        {MakeStringWithPercent "Unstable nodes count" UnstableNodesCount NodesCount $} UnstableNodesCount
        
        {MakeStringWithPercent "X nodes count" XNodesCount NodesCount $} XNodesCount
        {MakeStringWithPercent "Y nodes count" YNodesCount NodesCount $} YNodesCount
        {MakeStringWithPercent "G nodes count" GNodesCount NodesCount $} GNodesCount
        {MakeStringWithPercent "K nodes count" KNodesCount NodesCount $} KNodesCount
        
        "Stack depth" StackDepth
        "Nodes count" NodesCount
      ]}
    end
  end

  proc {DisplayDepth Id Thread}
    Depth = {Boot_Introspection.getThreadStackDepth Thread $}
  in
    {PrintInfo "Thread "#Id#" stack depth : "#Depth}
  end

  proc {DisplayRegister Id Thread Arguments}
    StackDepth = {Boot_Introspection.getThreadStackDepth Thread $}

    proc {DisplayOptions}
      {DisplayFrame "Thread register types" [
        "x" "Each thread possesses only one x register independant from the stack (no depth) that is used for local computation"
        "y <depth> <condition>" "Threads use Y registers to save X register's values from a previous scope"
        "g <depth> <condition>" "Threads use G registers to store the program defined constants for each scope"
        "k <depth> <condition>" "Threads use K registers to access environment constants reachable at a certain scope"
      ]}
    end

    proc {ExtractDepth Argument ?Depth}
      if {String.isInt Argument $} then
        D = {String.toInt Argument $}
      in
        if D < StackDepth then
          Depth = D
        else
          Depth = none
          {PrintError "Provided depth is too high, please enter a value below "#StackDepth}
        end
      else
        Depth = none
        {PrintError "Invalid depth '"#Argument#"' provided, please enter a value between 0 and "#(StackDepth - 1)}
      end
    end

    proc {DisplayNodes Nodes Conditions}
      {DisplayCSV
        [
          "Name            "
          "BindingPriority" "StructuralBehavior" "Copyable" "Transient" "Feature"
          flexible("Value")
        ]
        Nodes
        proc {$ Node ?Result} Result = Node end
        proc {$ Node ?Result} Result = {CheckConditions Conditions Node $} end
        proc {$ Node ?Result}
          case Node of node(
            name: Name
            bindingPriority: BindingPriority
            structuralBehavior: StructuralBehavior
            copyable: Copyable
            transient: Transient
            feature: Feature
            uuid: _
            value: Value
          ) then
            Result = [
              {Atom.toString Name $}
              {Int.toString BindingPriority $}
              {Atom.toString StructuralBehavior $}
              {Bool.toString Copyable $}
              {Bool.toString Transient $}
              {Bool.toString Feature $}
              {ExtractString Value $}
            ]
          end
        end}
    end

    proc {DisplayNodesAux Type Arguments}
      proc {Aux Arguments Get}
        Conditions = {ExtractConditions Arguments $}
      in
        if Conditions \= none then
          Nodes = {Get $}
        in
          {DisplayNodes Nodes Conditions}
        end
      end

      proc {Aux2 Arguments Get}
        case Arguments of nil then
          {PrintError "Missing an integer depth value between 0 and "#(StackDepth - 1)}
        [] Argument|NextArguments then
          Depth = {ExtractDepth Argument $}
        in
          if Depth \= none then
            {Aux NextArguments proc {$ ?Nodes}
              Nodes = {Get Depth $}
            end}
          end
        end
      end
    in
      case Type of x then
        {Aux Arguments proc {$ ?Nodes}
          Nodes = {Boot_Introspection.getThreadXNodes Thread 0 10000 $}
        end}
      [] y then
        {Aux2 Arguments proc {$ Depth ?Nodes}
          Nodes = {Boot_Introspection.getThreadYNodes Thread Depth 0 10000 $}
        end}
      [] g then
        {Aux2 Arguments proc {$ Depth ?Nodes}
          Nodes = {Boot_Introspection.getThreadGNodes Thread Depth 0 10000 $}
        end}
      [] k then
        {Aux2 Arguments proc {$ Depth ?Nodes}
          Nodes = {Boot_Introspection.getThreadKNodes Thread Depth 0 10000 $}
        end}
      end
    end
  in
    case Arguments of nil then
      {DisplayOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayOptions}
      [] "x" then
        {DisplayNodesAux x NextArguments}
      [] "y" then
        {DisplayNodesAux y NextArguments}
      [] "g" then
        {DisplayNodesAux g NextArguments}
      [] "k" then
        {DisplayNodesAux k NextArguments}
      else
        {PrintInvalidOptionError Argument}
      end
    end
  end

  proc {DisplayRegisters Id Thread}
    VMNodesCount = {Boot_Introspection.getNodesCount $}
    NodesCount = {Boot_Introspection.getThreadNodesCount Thread $}
    StackDepth = {Boot_Introspection.getThreadStackDepth Thread $}

    proc {MakeStringWithDepth String Depth ?Result}
      DepthString = {Int.toString Depth $}
      Left = {List.append String " (depth " $}
      Right = {List.append DepthString ")" $}
    in
      Result = {List.append Left Right $}
    end

    proc {MakeStringWithPercents String Count ?Result}
      Result = {MakeStringWithPercent
        {MakeStringWithPercent String Count NodesCount $}
        Count VMNodesCount $}
    end

    proc {MakeString String Depth Count ?Result}
      AtFirst = {MakeStringWithDepth String Depth $}
    in
      Result = {MakeStringWithPercents AtFirst Count $}
    end

    proc {GetFrameAux Depth ?Frame}
      if Depth < StackDepth then
        YRegisterSize = {Boot_Introspection.getThreadYNodesRegisterSize Thread Depth $}
        GRegisterSize = {Boot_Introspection.getThreadGNodesRegisterSize Thread Depth $}
        KRegisterSize = {Boot_Introspection.getThreadKNodesRegisterSize Thread Depth $}

        NewFrame
      in
        Frame = {MakeString "Y register size" Depth YRegisterSize $}|YRegisterSize
          | {MakeString "G register size" Depth GRegisterSize $}|GRegisterSize
          | {MakeString "K register size" Depth KRegisterSize $}|KRegisterSize
          | NewFrame

        {GetFrameAux (Depth + 1) NewFrame}
      else Frame = nil end
    end

    proc {GetFrame ?Frame}
      XRegisterSize = {Boot_Introspection.getThreadXNodesRegisterSize Thread $}
      NewFrame
    in
      Frame = {MakeStringWithPercents "X register size" XRegisterSize $}|XRegisterSize|NewFrame
      {GetFrameAux 0 NewFrame}
    end
  in
    {DisplayFrame "Thread registers (thread%) (vm%)" {GetFrame $}}
  end

  proc {HandleOption Id Thread Arguments}
    case Arguments of nil then
      {DisplayOptions}
    [] Argument|NextArguments then
      case Argument of "help" then
        {DisplayOptions}
      [] "state" then
        {DisplayState Id Thread}
      [] "statistics" then
        {DisplayStatistics Id Thread}
      [] "nodes" then
        {DisplayNodes Id Thread}
      [] "depth" then
        {DisplayDepth Id Thread}
      [] "register" then
        {DisplayRegister Id Thread NextArguments}
      [] "registers" then
        {DisplayRegisters Id Thread}
      else
        {PrintInvalidOptionError Argument}
      end
    end
  end
in
  case Arguments of nil then
    {DisplayOptions}
  [] Argument|NextArguments then
    case Argument of "help" then
      {DisplayOptions}
    else
      Id = {ExtractIdentity Argument $}
    in
      if Id == none then
        {PrintInvalidIdError Argument}
      else
        Thread = {GetThreadFromId Id $}
      in
        if Thread == none then
          {PrintThreadNotFoundError Id}
        else
          {HandleOption Id Thread NextArguments}
        end
      end
    end
  end
end