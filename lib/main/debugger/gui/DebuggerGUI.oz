functor
import
  Boot at 'x-oz://boot/Boot'
  System
  Module
  % QTk at 'x-oz://system/wp/QTk'
export
  Agent
define
  Boot_Thread = {Boot.getInternal 'Thread'}
  Boot_Scheduler = {Boot.getInternal 'Scheduler'}
  Boot_Introspection = {Boot.getInternal 'Introspection'}

  CommandsStream
  CommandsPort = {NewPort CommandsStream $}

  proc {Execute Command}
    {Send CommandsPort Command}
  end

  InputText
  InputTextWidget = entry(
    glue: we
    init: "help"
    handle: InputText
  )

  OutputText
  OutputTextWidget = text(
    glue: we
    height: 10
    tdscrollbar: true
    lrscrollbar: true
    init: ""
    handle: OutputText
  )
  
  proc {ResetOutputText}
    {OutputText set("")}
  end

  proc {InsertInOutputText String}
    {OutputText insert('end' String)}
  end
  
  Board

  FileMenu = menu(
    command(
      text: "Help"
      action: proc {$} {Execute "help"} end
    )
    command(
      text: "Exit"
      action: proc {$} {System.show exit} end
    )
  )

  proc {GenerateMenu Tree Path ?Result}
    proc {ForEach I Keys Tree Accumulated Make ?Result}
      case Keys of nil then Result = Accumulated
      [] Key|NextKeys then
        NewAccumulated = {Make I Key Path Tree.Key Accumulated $}
      in
        {ForEach (I + 1) NextKeys Tree NewAccumulated Make Result}
      end
    end
  in
    case Tree of tree(...) then
      Arity = {Record.arity Tree $}
    in
      Result = {ForEach 1 Arity Tree menu()
        proc {$ I Key Path Value Accumulated ?NewAccumulated}
          if {List.is Value $} then
            ReversedPath = {List.reverse Key|Path $}
            NewPath = {List.append ReversedPath Value $}
          in
            NewAccumulated = {Record.adjoin Accumulated menu(
              I: command(
                text: {Atom.toString Key $}
                action: proc {$}
                  {Execute NewPath}
                end
              )
            )}
          else
            NewAccumulated = {Record.adjoin Accumulated menu(
              I: cascade(
                text: {Atom.toString Key $}
                menu: {GenerateMenu Value Key|Path $}
              )
            )}
          end
        end
      $}
    [] askIdentityTree(...) then
      NewTree = {Record.adjoin Tree tree() $}
    in
      Result = {GenerateMenu NewTree askIdentity|Path $}
    end
  end

  IntrospectionTree = tree(
    'count': tree(
      'vm': tree(
        'schedules': nil
        'operations': nil
        'nonSystemSchedules': nil
        'nonSystemOperations': nil
        'systemSchedules': nil
        'systemOperations': nil
        'gcSchedules': nil
        'all': nil
      )

      'threads': tree(
        'active': nil
        'passive': nil
        'total': nil
        'all': nil
      )

      'variables': tree(
        'bound': nil
        'unbound': nil
        'total': nil
        'all': nil
      )

      'nodes': tree(
        'variable': nil
        'value': nil
        'structural': nil
        'token': nil
        'stable': nil
        'unstable': nil
        'x': nil
        'y': nil
        'g': nil
        'k': nil
        'stackDepth': nil
        'total': nil
        'all': nil
      )
    )

    'vm': tree(
      'state': nil
      'statistics': nil
      'nodes': nil
    )

    'thread': askIdentityTree(
      'state': nil
      'statistics': nil
      'nodes': nil
      'all': nil
      'depth': nil
      'register': [
        askAtom("Register type (x, y, g or k)")
        askInt("Stack depth where to locate register")
      ]
      'registers': nil
      'abstractions': nil
    )

    'threads': tree(
      'state': [askCondition]
      'statistics': [askCondition]
      'nodes': [askCondition]
    )

    'variable': askIdentityTree(
      'state': nil
      'candidates': nil
      'pendings': nil
    )

    'variables': tree(
      'state': [askCondition]
      'candidates': [askCondition]
      'pendings': [askCondition]
    )
  )

  IntrospectionMenu = {GenerateMenu IntrospectionTree nil $}

  EventManagerTree = tree(
    'journal': [askCondition]
    'tracks': [askCondition]

    'track': tree(
      'runnable': tree(
        'created': [askOptionalInt("Author id") askIntList("Victim ids")]
        'updated': [askOptionalInt("Author id") askIntList("Victim ids")]
        'collected': [askOptionalInt("Author id") askIntList("Victim ids")]
        'scheduled': [askOptionalInt("Author id") askIntList("Victim ids")]
        'unscheduled': [askOptionalInt("Author id") askIntList("Victim ids")]
        'rescheduled': [askOptionalInt("Author id") askIntList("Victim ids")]
      )

      'variable': tree(
        'created': [askOptionalInt("Author id") askIntList("Victim ids")]
        'collected': [askOptionalInt("Author id") askIntList("Victim ids")]
        'needed': [askOptionalInt("Author id") askIntList("Victim ids")]
        'waited': [askOptionalInt("Author id") askIntList("Victim ids")]
        'bound': [askOptionalInt("Author id") askIntList("Victim ids")]
      )
    )

    'untrack': tree(
      'runnable': tree(
        'created': [askInt("Rule index")]
        'updated': [askInt("Rule index")]
        'collected': [askInt("Rule index")]
        'scheduled': [askInt("Rule index")]
        'unscheduled': [askInt("Rule index")]
        'rescheduled': [askInt("Rule index")]
      )

      'variable': tree(
        'created': [askInt("Rule index")]
        'collected': [askInt("Rule index")]
        'needed': [askInt("Rule index")]
        'waited': [askInt("Rule index")]
        'bound': [askInt("Rule index")]
      )
    )
  )

  EventManagerMenu = {GenerateMenu EventManagerTree nil $}

  ReachabilityTree = tree(
    'graph': nil
  )

  ReachabilityMenu = {GenerateMenu ReachabilityTree nil $}

  ControlTree = tree(
    'run': tree(
      'schedules': [askInt("Number of thread schedules")]
      'operations': [askInt("Number of bytecode operations")]
      'until': nil
      'steps': [askIdentityList("Identities of the threads or streams or variables to run a step by step on")]
    )

    'reset': nil
    
    'continue': nil
    
    'gc': tree(
      'on': nil
      'off': nil
    )
  )

  ControlMenu = {GenerateMenu ControlTree nil $}

  SettingsMenu = menu(
    command(
      text: "Hello world"
      action: proc {$} {System.showInfo "Hello world"} end
    )
  )
  
  Menus = lr(
    glue: nw

    menubutton(
      text: "File"
      menu: FileMenu
    )

    menubutton(
      text: "Introspection"
      menu: IntrospectionMenu
    )

    menubutton(
      text: "EventManager"
      menu: EventManagerMenu
    )

    menubutton(
      text: "Reachability"
      menu: ReachabilityMenu
    )

    menubutton(
      text: "Control"
      menu: ControlMenu
    )

    menubutton(
      text: "Settings"
      menu: SettingsMenu
    )
  )

  Description = td(
    glue: nswe

    Menus

    lr(
      glue: nswe


      td(
        glue: nswe

        scrollframe(
          glue:nswe
          grid(handle: Board)
          tdscrollbar: true 
          lrscrollbar: true
        )        
      )


      % td(
      %   glue: nswe

      %   td(
      %     glue: nswe

      %     label(
      %       glue: w
      %       text: "Total threads"
      %       foreground: red
      %     )
 
      %     canvas(
      %       handle: TotalThreadsCanvas
      %     )
      %   )

      %   td(
      %     glue: nswe

      %     label(
      %       glue: w
      %       text: "Active threads"
      %       foreground: orange
      %     )

      %     label(
      %       glue: w
      %       text: "Passive threads"
      %       foreground: blue
      %     )

      %     canvas(
      %       handle: ActiveVsPassiveThreadsCanvas
      %     )
      %   )
      % )
    )
    
    td(
      button(
        glue: w
        text: "Clear"
        action: proc {$} {ResetOutputText} end
      )

      OutputTextWidget
    )

    lr(
      glue: we

      label(
        glue: w
        text: "Enter command"
        foreground: red
      )

      InputTextWidget

      button(
        glue: e
        text: "Execute"
        action: proc {$}
          Command = {InputText get($)}
        in
          {Execute Command}
        end
      )
    )
  )

  Window
  thread
    This = {Boot_Thread.this $}
  in
    {Boot_Thread.setPriority This system}

    local QTk in
      [QTk] = {Module.link ["x-oz://system/wp/QTk.ozf"]}
      Window = {QTk.build td(Description)}
    end
  end

  ControllerStream
  ControllerPort = {NewPort ControllerStream $}

  RequestsStream
  RequestsPort = {NewPort RequestsStream $}

  proc {Loop Requests Commands}
    This = {Boot_Thread.this $}

    proc {Return}
      {Boot_Thread.setPreemptible This true}
      {Boot_Thread.preempt This}
    end
  in
    if {Boot_Thread.isPreemptible This $} then
      {Boot_Thread.setPreemptible This false}
    end

    if {Boot_Thread.getPriority This $} \= system then
      {Boot_Thread.setPriority This system}
    end

    case Requests of nil then {Return}
    [] Request|NextRequests then
      case Request of input(?Input) then
        case Commands of nil then {Return}
        [] Command|NextCommands then
          Input = Command
          {Loop NextRequests NextCommands}
        end
      end
    end
  end

  Agent = proc {$ Request}
    {Send RequestsPort Request}
  end
in
  {Window show}
  thread {Loop RequestsStream CommandsStream} end

  % thread
  %   proc {Loop}
  %     Input = {Agent input($)}
  %   in
  %     {Wait Input}
  %     {System.show got(Input)}
  %     {Loop}
  %   end
  % in
  %   {Loop}
  % end
end