functor
import
  System
  QTk at 'x-oz://system/wp/QTk'
export
  new: NewDynamicGridAgent
define
  proc {Stringify Value ?StringValue}
    if {Int.is Value $} then
      StringValue = {Int.toString Value $}
    elseif {Float.is Value $} then
      StringValue = {Float.toString Value $}
    elseif {Atom.is Value $} then
      StringValue = {Atom.toString Value $}
    elseif {Bool.is Value $} then
      StringValue = {Bool.toString Value $}
    else StringValue = Value end
  end

  BACKGROUND0 = c(163 163 163)
  BACKGROUND1 = c(104 97 112)

  proc {NewDynamicGridAgent Grid DefaultConfig ?Agent}
    proc {DisplayData Data ?Labels}
      Arity = {Record.arity Data $}

      proc {ForEach I Arity ?Labels}
        case Arity of nil then Labels = nil
        [] Feature|NextArity then
          KeyLabel ValueLabel
          Value = Data.Feature
          NewLabels
        in
          Labels = KeyLabel|ValueLabel|NewLabels

          {Grid configure(
            label(
              glue: nw
              text: {List.append {Atom.toString Feature $} ": " $}
              handle: KeyLabel
            )
            column: 0
            row: I
          )}

          local Widget in
            if {List.is Value $} then
              Widget = listbox(
                glue: nw
                init:Value
                tdscrollbar:true
                handle: ValueLabel  
              )
            else
              Widget = label(
                glue: nw
                text: {Stringify Value $}
                handle: ValueLabel
              )
            end

            {Grid configure(Widget column:1 row:I)}
          end

          {ForEach (I + 1) NextArity NewLabels}
        end
      end
    in
      Labels = {ForEach 0 Arity $}
    end

    proc {DisplayCSV Labels Rows ?Widgets}
      proc {DisplayLabels I Labels EndWidgets ?Widgets}
        case Labels of nil then Widgets = EndWidgets
        [] Label|NextLabels then
          Widget
          NewWidgets
        in
          Widgets = Widget|NewWidgets

          {Grid configure(
            label(
              glue: nw
              text: Label
              foreground: white
              background: black
              handle: Widget
            )
            column: I
            row: 0
          )}

          {DisplayLabels (I + 1) NextLabels EndWidgets NewWidgets}
        end
      end

      proc {DisplayRow I J Values EndWidgets ?Widgets}
        case Values of nil then Widgets = EndWidgets
        [] Value|NextValues then
          Widget
          NewWidgets
        in
          Widgets = Widget|NewWidgets

          {Grid configure(
            label(
              glue: nw
              text: Value
              foreground: white
              background: if I mod 2 == 0 then BACKGROUND0 else BACKGROUND1 end
              handle: Widget
            )
            column: J
            row: I
          )}

          {DisplayRow I (J + 1) NextValues EndWidgets NewWidgets}
        end
      end


      proc {DisplayRows I Rows ?Widgets}
        case Rows of nil then Widgets = nil
        [] Row|NextRows then
          NewWidgets
        in
          Widgets = {DisplayRow I 0 Row NewWidgets $}
          {DisplayRows (I + 1) NextRows NewWidgets}
        end
      end
    in
      Widgets = {DisplayLabels 0 Labels
        {DisplayRows 1 Rows $}
      $}
    end

    proc {RemoveLabels Labels}
      case Labels of nil then skip
      [] Label|NextLabels then
        {Grid forget(Label)}
        {RemoveLabels NextLabels}
      end
    end

    proc {Loop Messages Widgets Config}
      case Messages of nil then skip
      [] Message|NextMessages then
        case Message of display(Data) then
          {RemoveLabels Widgets}

          local
            NewWidgets = {DisplayData Data $}
          in
            {Loop NextMessages NewWidgets Config}
          end
        [] displayCSV(Labels Matrix) then
          {RemoveLabels Widgets}

          local
            NewWidgets = {DisplayCSV Labels Matrix $}
          in
            {Loop NextMessages NewWidgets Config}
          end
        end
      end
    end

    Port Stream
  in
    {NewPort Stream Port}
    thread {Loop Stream nil DefaultConfig} end
    Agent = proc {$ Message} {Send Port Message} end
  end
in
  skip
end