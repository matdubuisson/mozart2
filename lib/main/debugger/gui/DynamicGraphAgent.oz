functor
import
  System
  QTk at 'x-oz://system/wp/QTk'
export
  new: NewDynamicGraphAgent
define
  WidthPaddingPercent = 0.1
  HeightPaddingPercent = 0.1
  TraitPercent = 0.03
  FontSizePercent = 0.05

  proc {GetScaledDistance Measure Percent ?ScaledDistance}
    FMeasure = {Int.toFloat Measure $}
  in
    ScaledDistance = {Float.toInt FMeasure * Percent $}
  end

  proc {GetScaledDot Measure Scale PaddingPercent Dot ?ScaledDot}
    FMeasure = {Int.toFloat Measure $}
    FScale = {Int.toFloat Scale $}
    FDot = {Int.toFloat Dot $}

    FScaledPadding = FMeasure * PaddingPercent % {GetScaledDistance Measure PaddingPercent $}
    ReducedFMeasure = (FMeasure - 2.0 * FScaledPadding)
    FScaledDot = (FDot / FScale) * ReducedFMeasure
  in
    ScaledDot = {Float.toInt FScaledPadding + FScaledDot $}
  end

  proc {GetExpectation I Dots NDots Sum ?Result}
    if I < NDots then
      case Dots of nil then Result = 0
      [] dot(Dot _ _)|NextDots then
        NewSum = Sum + Dot
      in
        {GetExpectation (I + 1) NextDots NDots NewSum Result}
      end
    else Result = Sum div NDots end
  end

  proc {GetVariance I Dots NDots Expectation Sum ?Result}
    if I < NDots then
      case Dots of nil then Result = 0
      [] dot(Dot _ _)|NextDots then
        Diff = Dot - Expectation
        NewSum = Diff * Diff
      in
        {GetVariance (I + 1) NextDots NDots Expectation NewSum Result}
      end
    else
      Div = {Int.toFloat Sum $} / {Int.toFloat NDots $}
    in
      Result = {Float.toInt {Float.sqrt Div $} $}
    end
  end

  proc {InitDotsStream I NDots End ?Stream}
    if I < NDots then NewStream in
      Stream = dot(0 0 0)|NewStream
      {InitDotsStream (I + 1) NDots End NewStream}
    else Stream = End end
  end

  proc {SkipDotsStream DotsStream NDots ?NewDotsStream}
    if NDots == 0 then NewDotsStream = DotsStream
    else
      case DotsStream of nil then NewDotsStream = nil
      [] _|NextDotsStream then
        {SkipDotsStream NextDotsStream (NDots - 1) NewDotsStream}
      end
    end
  end

  proc {NewDynamicGraphAgent Canvas DefaultConfig ?Agent}
    proc {DrawGraph Dots Config}
      % MaxX == Config.nDots

      proc {GetMaxY I Dots CurrentMaxY ?MaxY}
        if I < Config.nDots then
          case Dots of nil then MaxY = CurrentMaxY
          [] dot(Dot Expectation Variance)|NextDots then
            Best
          in
            if Config.statisticsEnabled andthen Expectation > Dot andthen Expectation > Variance then
              Best = Expectation
            elseif Config.statisticsEnabled andthen Variance > Dot then
              Best = Variance
            else Best = Dot end

            if Best > CurrentMaxY then
              {GetMaxY (I + 1) NextDots Best MaxY}
            else
              {GetMaxY (I + 1) NextDots CurrentMaxY MaxY}
            end
          end
        else MaxY = CurrentMaxY end
      end

      MaxY = {GetMaxY 0 Dots 1 $} % enforcing MaxY > 0
      Width Height
    in
      {Canvas get(width:Width height:Height)}

      % Draw a white background
      {Canvas create(rect 0 0 Width Height fill:white)}

      local
        X0 = {GetScaledDot Width Config.nDots WidthPaddingPercent 0 $}
        Y0 = {GetScaledDot Height MaxY HeightPaddingPercent 0 $}
        X1 = {GetScaledDot Width Config.nDots WidthPaddingPercent Config.nDots $}
        Y1 = {GetScaledDot Height MaxY HeightPaddingPercent MaxY $}

        proc {DrawDots I Dots PreX PreY PreExpectationY}
          if I < Config.nDots then
            case Dots of nil then skip
            [] dot(Dot Expectation Variance)|NextDots then
              X = {GetScaledDot Width Config.nDots WidthPaddingPercent I $}
              Y = {GetScaledDot Height MaxY HeightPaddingPercent (MaxY - Dot) $}
              ExpectationY = {GetScaledDot Height MaxY HeightPaddingPercent (MaxY - Expectation) $}
              VarianceY0 = {GetScaledDot Height MaxY HeightPaddingPercent (MaxY - (Expectation - Variance)) $}
              VarianceY1 = {GetScaledDot Height MaxY HeightPaddingPercent (MaxY - (Expectation + Variance)) $}
            in
              if Config.statisticsEnabled then
                {Canvas create(line X VarianceY0 X VarianceY1 fill:purple)}
                {Canvas create(line PreX PreExpectationY X ExpectationY fill:green)}
              end

              % Draw the line between the previous dot and the new one
              {Canvas create(line PreX PreY X Y fill:Config.color)}

              if Config.traitsEnabled then
                TraitSize = {GetScaledDistance Width TraitPercent $}
              in
                % Draw the trait on the x axis and the associated value
                {Canvas create(line X Y1 X (Y1 - TraitSize) fill:black)}

                % Draw the trait on the y axis and the associated value
                {Canvas create(line X0 Y (X0 + TraitSize) Y fill:black)}
              end

              if Config.digitsEnabled then
                LabelsPeriod = Config.nDots div Config.nLabels
                IsLabelTime = I mod LabelsPeriod == 0
              in                
                if IsLabelTime orelse I == Config.nDots - 1 then
                  % Draw the x value
                  XFontSize = {GetScaledDistance (Height - Y1) 0.25 $} % 95% of Padding size
                  DividedWidth = (Width - 2 * X0) div Config.nLabels
                  XFontWidth = {GetScaledDistance DividedWidth 0.95 $}
                  XFont = {QTk.newFont font(
                    family: "arial"
                    size: XFontSize
                  )}
                in
                  {Canvas create(text X (Y1 + XFontSize div 2 + 1)
                    text:{Int.toString I $} width:XFontWidth font:XFont)}
                end

                if IsLabelTime orelse Dot == MaxY then
                  % Draw the y value
                  DividedHeight = (Height - 2 * Y0) div Config.nLabels
                  YFontSize = {GetScaledDistance DividedHeight 0.25 $}
                  YFontWidth = {GetScaledDistance X0 0.95 $}
                  YFont = {QTk.newFont font(
                    family: "arial"
                    size: YFontSize
                  )}
                in
                  {Canvas create(text (X0 - YFontWidth div 4) Y
                    text:{Int.toString Dot $} width:YFontWidth font:YFont)}
                end
              end

              {DrawDots (I + 1) NextDots X Y ExpectationY}
            end
          end
        end
      in
        % Draw x and y axis
        {Canvas create(line X0 Y0 X0 Y1 X1 Y1 fill:blue)}
        {DrawDots 0 Dots X0 Y1 Y1}
      end
    end

    proc {Loop MessagesStream DotsStream EndDotsStream Config}
      case MessagesStream of Message|NextMessagesStream then
        case Message of draw() then
          {DrawGraph DotsStream Config}
          {Loop NextMessagesStream DotsStream EndDotsStream Config}

        [] newDot(Dot) then
          Expectation = {GetExpectation 0 DotsStream Config.nDots 0 $}
          Variance = {GetVariance 0 DotsStream Config.nDots Expectation 0 $}
          NewEndDotsStream
        in
          EndDotsStream = dot(Dot Expectation Variance)|NewEndDotsStream
          {Loop NextMessagesStream DotsStream.2 NewEndDotsStream Config}

        [] setNDots(NDots) then
          if NDots == Config.nDots then
            {Loop NextMessagesStream DotsStream EndDotsStream Config}
          elseif NDots > Config.nDots then
            MissingNDots = NDots - Config.nDots
            NewList = {InitDotsStream 0 MissingNDots nil $}
            NewDotsStream = {List.append NewList DotsStream $}
            NewConfig = {Adjoin Config config(nDots: NDots) $}
          in
            {Loop NextMessagesStream NewDotsStream EndDotsStream NewConfig}
          else
            OverloadNDots = Config.nDots - NDots
            NewDotsStream = {SkipDotsStream DotsStream OverloadNDots $}
            NewConfig = {Adjoin Config config(nDots: NDots) $}
          in
            {Loop NextMessagesStream NewDotsStream EndDotsStream NewConfig}
          end

        [] enableTraits() then
          NewConfig = {Adjoin Config config(traitsEnabled:true) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}
        [] disableTraits() then
          NewConfig = {Adjoin Config config(traitsEnabled:false) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}

        [] enableDigits() then
          NewConfig = {Adjoin Config config(digitsEnabled:true) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}
        [] disableDigits() then
          NewConfig = {Adjoin Config config(digitsEnabled:false) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}

        [] enableStatistics() then
          NewConfig = {Adjoin Config config(statisticsEnabled:true) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}
        [] disableStatistics() then
          NewConfig = {Adjoin Config config(statisticsEnabled:false) $}
        in
          {Loop NextMessagesStream DotsStream EndDotsStream NewConfig}

        end % else lets it crash
      end
    end

    Port MessagesStream
  in
    {NewPort MessagesStream Port} % Why Port.new not working ??
    
    local
      EndDotsStream
      DotsStream = {InitDotsStream 0 DefaultConfig.nDots EndDotsStream $}
    in
      thread {Loop MessagesStream DotsStream EndDotsStream DefaultConfig} end
    end

    Agent = proc {$ Message} {Send Port Message} end

    {Canvas bind(event:"<Configure>" action:proc {$} {Agent draw()} end)}
  end
in
  skip
end