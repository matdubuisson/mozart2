functor
import
  OS
  System
  QTk at 'x-oz://system/wp/QTk'

  Boot at 'x-oz://boot/Boot'

  DynamicGridAgent
  DynamicGraphAgent
export
  new: NewDebuggerGUIAgent
define
  Boot_Introspection = {Boot.getInternal 'Introspection'}

  FileMenu = menu(
    command(
      text: "Help"
      action: proc {$} {System.show help} end
    )
    command(
      text: "Exit"
      action: proc {$} {System.show exit} end
    )
  )

  IntrospectionMenu = menu(
    command(
      text: "count"
      action: proc {$} {System.show count} end
    )
    command(
      text: "vm"
      action: proc {$} {System.show count} end
    )
    command(
      text: "thread"
      action: proc {$} {System.show count} end
    )
    command(
      text: "threads"
      action: proc {$} {System.show count} end
    )
    command(
      text: "variable"
      action: proc {$} {System.show count} end
    )
    command(
      text: "variables"
      action: proc {$} {System.show count} end
    )
  )

  EventManagerMenu = menu(
    command(
      text: "journal"
      action: proc {$} {System.show event} end
    )
    command(
      text: "tracks"
      action: proc {$} {System.show event} end
    )
    command(
      text: "track"
      action: proc {$} {System.show event} end
    )
    command(
      text: "untrack"
      action: proc {$} {System.show event} end
    )
  )

  ReachabilityMenu = menu(
    command(
      text: "graph"
      action: proc {$} {System.show graph} end
    )
  )

  ControlMenu = menu(
    command(
      text: "run"
      action: proc {$} {System.show control} end
    )
    command(
      text: "reset"
      action: proc {$} {System.show control} end
    )
    command(
      text: "continue"
      action: proc {$} {System.show control} end
    )
    command(
      text: "gc"
      action: proc {$} {System.show control} end
    )
  )

  SettingsMenu = menu(
    command(
      text: "Hello world"
      action: proc {$} {System.showInfo "Hello world"} end
    )
  )

  InputText
  OutputText
  Board
  TotalThreadsCanvas
  ActiveVsPassiveThreadsCanvas

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


      td(
        glue: nswe

        td(
          glue: nswe

          label(
            glue: w
            text: "Total threads"
            foreground: red
          )
 
          canvas(
            handle: TotalThreadsCanvas
          )
        )

        td(
          glue: nswe

          label(
            glue: w
            text: "Active threads"
            foreground: orange
          )

          label(
            glue: w
            text: "Passive threads"
            foreground: blue
          )

          canvas(
            handle: ActiveVsPassiveThreadsCanvas
          )
        )
      )
    )
    
    text(
      glue: we
      height: 10
      tdscrollbar: true
      lrscrollbar: false
      init: "Test retest"
      handle: OutputText
    )

    lr(
      glue: we

      label(
        glue: w
        text: "Enter command"
        foreground: red
      )

      entry(
        glue: we
        init: "help"
        handle: InputText
      )

      button(
        glue: e
        text: "Execute"
        action: proc {$} {System.show input({InputText get($)})} end
      )
    )
  )

  Window = {QTk.build td(Description)}

  BoardAgent = {DynamicGridAgent.new Board config() $}

  TotalThreadsDynamicGraphAgent = {DynamicGraphAgent.new TotalThreadsCanvas config(
    nDots: 10
    nLabels: 5
    color: red
    traitsEnabled: false
    digitsEnabled: true
    statisticsEnabled: true
  ) $}

  ActiveThreadsDynamicGraphAgent = {DynamicGraphAgent.new ActiveVsPassiveThreadsCanvas config(
    nDots: 20
    nLabels: 5
    color: orange
    traitsEnabled: false
    digitsEnabled: true
    statisticsEnabled: false
  ) $}

  PassiveThreadsDynamicGraphAgent = {DynamicGraphAgent.new ActiveVsPassiveThreadsCanvas config(
    nDots: 20
    nLabels: 5
    color: blue
    traitsEnabled: false
    digitsEnabled: true
    statisticsEnabled: false
  ) $}

  thread
    proc {Loop}
      {TotalThreadsDynamicGraphAgent newDot({Boot_Introspection.getThreadsCount $})}
      {ActiveThreadsDynamicGraphAgent newDot({Boot_Introspection.getActiveThreadsCount $})}
      {PassiveThreadsDynamicGraphAgent newDot({Boot_Introspection.getPassiveThreadsCount $})}
      {TotalThreadsDynamicGraphAgent draw()}
      {ActiveThreadsDynamicGraphAgent draw()}
      {PassiveThreadsDynamicGraphAgent draw()}

      {Time.delay 1000}
      {Loop}
    end
  in
    {Loop}
  end

  proc {NewDebuggerGUIAgent ?Agent}
    Port Stream

    proc {Loop Messages}
      case Messages of nil then skip
      [] Message|NextMessages then
        case Message of print(S _ NewLine) then
          String = if NewLine then {List.append S "\n" $} else S end
        in
          {OutputText insert('end' String)}
        [] display(Data) then
          {BoardAgent display(Data)}
        [] displayCSV(Labels Rows) then
          {BoardAgent displayCSV(Labels Rows)}
        end

        {Loop NextMessages}
      end
    end
  in
    {NewPort Stream Port}
    {Window show}

    thread {Loop Stream} end

    Agent = proc {$ Message} {Send Port Message} end
  end
in
  % local
  %   Agent = {NewDebuggerGUIAgent $}

  %   proc {Test I}
  %     {Agent displayCSV(
  %       ["Label0" "Label1" "Label2" "Label3" "Label4" "LastVeryVeryLongLabelThatIsVeryBoring"]
  %       [
  %         ["00000000000000000000" "1" "2" "3" "4" "5" "6"]
  %         ["01" "1" "2" "3" "4" "5" "6"]
  %         ["02" "1" "2" "3" "4" "5" "6"]
  %         ["03" "1" "2" "3" "4" "5" "6"]
  %       ]
  %     )}

  %     {Time.delay 500}
  %     {Agent print("This is a wonderful string" false false)}
  %     {Time.delay 1000}
  %     {Agent print(".... New string" false false)}
  %     {Agent print({Int.toString I $} false true)}

  %     {Agent display({Thread.getState {Thread.this $} $})}
  %     {Time.delay 1000}
  %     {Test (I + 1)}
  %   end
  % in
  %   {Test 0}
  % end
  skip
end