local
  Options = [
    option("x (id <threadId>)" "Display infos about a x register")
    option("y (id <threadId>) (depth <depth>)" "Display infos about a y register at a certain depth")
    option("g (id <threadId>) (depth <depth>)" "Display infos about a g register at a certain depth")
    option("k (id <threadId>) (depth <depth>)" "Display infos about a k register at a certain depth")
  ]

  proc {DisplayHelp IsError}
    {PrintOptions "register" Options IsError}
  end

  proc {DisplayRegister RegisterType Arguments}
    ThreadId = {GetArgument Arguments "id" int ThisId $}
    Thread
  in
    Thread = {Boot_Introspection.getThread ThreadId $}
    
    case Thread of none then
      {PrintError "Thread with id "#ThreadId#" does no exist"}
    else
      local
        ThreadIdentity = {Boot_Introspection.getThreadIdentity Thread $}
      in
        case ThreadIdentity of identity(
          id: Id
          kindId: KindId
          generationId: GenerationId
          type: Type
          priority: Priority
        ) then
          {PrintLog "Owner thread : id "#Id#
            ", kindId "#KindId#
            ", genId "#GenerationId#
            ", type "#Type#
            ", priority "#Priority}
        end
      end

      local
        Depth = {GetArgument Arguments "depth" int 0 $}
        MaxDepth = {Boot_Introspection.getThreadStackDepth Thread $} - 1 % Starts counting from 0
      in
        if Depth > MaxDepth then
          {PrintError "Thread "#ThreadId#" has a stack of depth "#MaxDepth}
        else
          proc {PrintDepth}
            {PrintLog "Depth: "#Depth#"/"#MaxDepth}
          end

          proc {PrintRegister RegisterSize}
            local
              NodesCount = NodesCounts.nodesCount
              Percent = (RegisterSize * 100) div NodesCount
              DotPercent = (RegisterSize * 10000) mod NodesCount
            in
              {PrintLog "Size: "#RegisterSize#"/"#NodesCount#" nodes ("#Percent#"."#DotPercent#"%)"}
            end
          end

          proc {PrintNodeValues NodeValues}
            proc {ForEachNodeValue I NodeValues}
              case NodeValues of nil then skip
              [] NodeValue|NextNodeValues then
                case NodeValue of node(
                  name: Name
                  value: Value
                  copyable: Copyable
                  transient: Transient
                  feature: Feature
                ) then
                  {PrintLog I#
                    "\t"#Name#
                    "\t"#Value#
                    "\t"#{Bool.toString Copyable $}#
                    "\t"#{Bool.toString Transient $}#
                    "\t"#{Bool.toString Feature $}}
                end
                {ForEachNodeValue I+1 NextNodeValues}
              end
            end
          in
            {PrintLog "INDEX \tNAME \tVALUE \tCOPYABLE \tTRANSIENT \tFEATURE"}
            {ForEachNodeValue 0 NodeValues}
          end

          proc {PrintNodeValuesStats NodeValues RegisterSize}
            proc {ForEachNodeValue NodeValues
              CopyableCount TransientCount FeatureCount
              NameCounts
              ?ResultCopyableCount ?ResultTransientCount ?ResultFeatureCount
              ?ResultNameCounts}
              case NodeValues of nil then
                ResultCopyableCount = CopyableCount
                ResultTransientCount = TransientCount
                ResultFeatureCount = FeatureCount
                ResultNameCounts = NameCounts
              [] NodeValue|NextNodeValues then
                case NodeValue of node(
                  name: Name
                  value: Value
                  copyable: Copyable
                  transient: Transient
                  feature: Feature
                ) then
                  Names = {Record.arity NameCounts $}
                  NameCount
                in
                  if {List.member Name Names $} then
                    Count = NameCounts.Name + 1
                  in
                    NameCount = map(Name: Count)
                  else
                    NameCount = map(Name: 1)
                  end

                  {ForEachNodeValue
                    NextNodeValues
                    if Copyable then CopyableCount + 1 else CopyableCount end
                    if Transient then TransientCount + 1 else TransientCount end
                    if Feature then FeatureCount + 1 else FeatureCount end
                    {Record.adjoin NameCounts NameCount $}
                    ResultCopyableCount
                    ResultTransientCount
                    ResultFeatureCount
                    ResultNameCounts
                  }
                end
              end
            end

            proc {PrintNameCountPercent Name Count}
              Percent = (Count * 100) div RegisterSize
              DotPercent = (Count * 10000) mod RegisterSize
            in
              {PrintLog Name#
                "\t\t\t"#Count#
                "\t"#Percent#"."#DotPercent#"%"}
            end

            proc {PrintEachNameCountPercent Names NameCounts}
              case Names of nil then skip
              [] Name|NextNames then
                {PrintNameCountPercent Name NameCounts.Name}
                {PrintEachNameCountPercent NextNames NameCounts}
              end
            end
            
            CopyableCount
            TransientCount
            FeatureCount
            NameCounts
          in
            {ForEachNodeValue NodeValues 0 0 0 map()
              CopyableCount TransientCount FeatureCount NameCounts}

            {PrintLog "PROPERTY\t\t\tCOUNT\tPERCENT"}
            {PrintNameCountPercent "Copyable" CopyableCount}
            {PrintNameCountPercent "Transient" TransientCount}
            {PrintNameCountPercent "Feature" FeatureCount}

            {PrintLog "NAME\t\t\tCOUNT\tPERCENT"}
            {PrintEachNameCountPercent
              {Record.arity NameCounts $}
              NameCounts}
          end

          NodesCounts = {Boot_Introspection.getThreadNodesCounts Thread $}
          ContentFlag = {GetArgument Arguments "content" flag false $}
        in
          case RegisterType of x then
            RegisterSize = {Boot_Introspection.getThreadXNodesRegisterSize Thread $}
            NodeValues = {Boot_Introspection.getThreadXNodeAggregates
                Thread 0 RegisterSize value}
          in
            {PrintRegister RegisterSize}

            if ContentFlag then  
              {PrintNodeValues NodeValues}
            else
              {PrintNodeValuesStats NodeValues RegisterSize}
            end
          [] y then skip
            RegisterSize = {Boot_Introspection.getThreadYNodesRegisterSize Thread Depth $}
            NodeValues = {Boot_Introspection.getThreadYNodeAggregates
                Thread Depth 0 RegisterSize value}
          in
            {PrintDepth}
            {PrintRegister RegisterSize}

            if ContentFlag then
              {PrintNodeValues NodeValues}
            else
              {PrintNodeValuesStats NodeValues RegisterSize}
            end
          [] g then skip
            RegisterSize = {Boot_Introspection.getThreadGNodesRegisterSize Thread Depth $}
            NodeValues = {Boot_Introspection.getThreadGNodeAggregates
                Thread Depth 0 RegisterSize value}
          in
            {PrintDepth}
            {PrintRegister RegisterSize}

            if ContentFlag then
              {PrintNodeValues NodeValues}
            else
              {PrintNodeValuesStats NodeValues RegisterSize}
            end
          [] k then skip
            RegisterSize = {Boot_Introspection.getThreadKNodesRegisterSize Thread Depth $}
            NodeValues = {Boot_Introspection.getThreadKNodeAggregates
                Thread Depth 0 RegisterSize value}
          in
            {PrintDepth}
            {PrintRegister RegisterSize}

            if ContentFlag then
              {PrintNodeValues NodeValues}
            else
              {PrintNodeValuesStats NodeValues RegisterSize}
            end
          end
        end
      end
    end
  end
in
  case Arguments of Option|NextArguments then
    case Option of "x" then
      {DisplayRegister x NextArguments}
    [] "y" then
      {DisplayRegister y NextArguments}
    [] "g" then
      {DisplayRegister g NextArguments}
    [] "k" then
      {DisplayRegister k NextArguments}
    [] "help" then
      {DisplayHelp false}
    else
      {DisplayHelp true}
    end
  else
    {DisplayHelp true}
  end

  % {PrintLog "alert"}
  % case Arguments of Option|NextArguments then
  %   ThreadId = {GetArgument NextArguments "id" int {Thread.getId This $} $}
  %   Depth = {GetArgument NextArguments "depth" int 0 $}

  %   Thread = {Boot_Introspection.getThread ThreadId $}
  % in
  %   {PrintLog "alert2"}
  %   if {Thread.is Thread $} then
  %     NodesCounts = {Boot_Introspection.getThreadNodesCounts Thread $}
  %   in
  %     {PrintLog "alert3"}
  %     case Option of "x" then
  %       RegisterSize = {Boot_Introspection.getThreadXNodesRegisterSize Thread $}
  %     in
  %       {PrintLog "alert4"}
  %       if Depth > 0 then
  %         {PrintError "x register has no depth as it is unique for each thread, taking 0"}
  %       end

  %       {PrintLog "Size: "#RegisterSize#"/"#NodesCounts.nodesCount#" nodes"}
  %       local
  %         Fraction = RegisterSize / NodesCounts.nodesCount
  %       in
  %         {PrintLog "Fraction: "#Fraction#"%"}
  %       end
  %     % [] "y" then

  %     % [] "g" then

  %     % [] "k" then

  %     [] "help" then
  %       {PrintOptions "register" Options false}
  %     else
  %       {PrintOptions "register" Options true}
  %     end
  %   else
  %     {PrintError "Thread with id "#ThreadId#" is not found"}
  %   end
  % else
  %   {PrintOptions "register" Options true}
  % end
end