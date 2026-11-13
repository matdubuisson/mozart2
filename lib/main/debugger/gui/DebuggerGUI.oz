functor
import
  OS
  System
  QTk at 'x-oz://system/wp/QTk'

  Boot at 'x-oz://boot/Boot'

  DynamicGridAgent
  DynamicGraphAgent
export
  Start
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

        canvas(
          handle: TotalThreadsCanvas
        )

        canvas(
          handle: ActiveVsPassiveThreadsCanvas
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

    entry(
      glue: we
      init: "Enter command"
      handle: InputText
      action: proc {$} {System.show input} end
    )
  )

  Window

  proc {Start}
    Window = {QTk.build td(Description)}
    {Window show}
  end

in

  skip
  % {System.showInfo "Hello"}
  % {Window show}

  % local
  %   Agent = {DynamicGridAgent.new Board config() $}
  % in
  %   % {Agent display(
  %   %   {Thread.getState {Thread.this $} $}
  %   % )}

  %   % {Agent display(
  %   %   test(
  %   %     'a very very very super very long long name that is annoying to read': aaaa
  %   %     b: [0 1 2 3 4 5]
  %   %     c: ["test" "eheheheheh"]
  %   %     d: [0 1 2 3 4 5]
  %   %     e: [0 1 2 3 4 5]
  %   %     f: [0 1 2 3 4 5]
  %   %   )
  %   % )}

  %   % {Time.delay 2000}

  %   {Agent displayCSV(
  %     ["Label0" "Label1" "Label2" "Label3" "Label4" "LastVeryVeryLongLabelThatIsVeryBoring"]
  %     [
  %       ["00000000000000000000" "1" "2" "3" "4" "5" "6"]
  %       ["01" "1" "2" "3" "4" "5" "6"]
  %       ["02" "1" "2" "3" "4" "5" "6"]
  %       ["03" "1" "2" "3" "4" "5" "6"]
  %     ]
  %   )}
  % end

  % local
  %   Agent = {DynamicGraphAgent.new TotalThreadsCanvas config(
  %     nDots: 10
  %     nLabels: 5
  %     traitsEnabled: true
  %     digitsEnabled: true
  %     statisticsEnabled: true
  %   ) $}

  %   Agent2 = {DynamicGraphAgent.new ActiveVsPassiveThreadsCanvas config(
  %     nDots: 10
  %     nLabels: 5
  %     traitsEnabled: true
  %     digitsEnabled: true
  %     statisticsEnabled: false
  %   ) $}

  %   Agent3 = {DynamicGraphAgent.new ActiveVsPassiveThreadsCanvas config(
  %     nDots: 10
  %     nLabels: 5
  %     traitsEnabled: false
  %     digitsEnabled: false
  %     statisticsEnabled: false
  %   ) $}
  % in
  %   % {Grid bind(event:"<Configure>" action:
  %   %   proc {$}
  %   %     {Agent draw()}
  %   %   end
  %   %   )}
  %   thread
  %     proc {Loop}
  %       {Agent newDot({Boot_Introspection.getThreadsCount $})}
  %       {Agent draw()}

  %       {Agent2 newDot({Boot_Introspection.getActiveThreadsCount $})}
  %       {Agent3 newDot({Boot_Introspection.getPassiveThreadsCount $})}
  %       {Agent2 draw()}
  %       {Agent3 draw()}

  %       {Time.delay 1000}
  %       {Loop}
  %     end
  %   in
  %     {Loop}
  %   end
  % end

  % thread
  %   proc {Soldier Order}
  %     thread
  %       {Wait Order}
  %       {System.show order(Order)}
  %     end
  %   end

  %   proc {Commander I}
  %     if I > 4096 then skip
  %     else
  %       thread X in
  %         {Soldier X}
  %         {Soldier X}
  %         {Soldier X}
  %         {Soldier X}

  %         {Time.delay 1000}
  %         X = I

  %         {Commander 2 * I}
  %         {Commander 2 * I + 1}
  %       end
  %     end
  %   end
  % in
  %   {Commander 1}
  % end


  % {Window bind(event:"<Configure>" action:
  %   proc {$}
  %     Width Height
  %   in
  %     {Window get(width:Width height:Height)}
  %   end
  %   )}
  % {Start}
end