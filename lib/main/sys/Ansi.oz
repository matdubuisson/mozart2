functor
import
    System
export
    makeInfo: MakeInfo
    makeSuccess: MakeSuccess
    makeWarning: MakeWarning
    makeError: MakeError
    makeDebug: MakeDebug
define
    ESC = "\033"
    CSI = {List.append ESC "[" $}
    RESET = {List.append CSI "0m"}

    RED = {List.append CSI "38;5;1m"}
    GREEN = {List.append CSI "38;5;2m"}
    ORANGE = {List.append CSI "38;5;3m"}
    BLUE = {List.append CSI "38;5;4m"}
    PURPLE = {List.append CSI "38;5;5m"}
    CYAN = {List.append CSI "38;5;6m"}
    WHITE = {List.append CSI "38;5;7m"}
    BLACK = {List.append CSI "38;5;8m"}

    proc {MakeColoredString InputString Color ?OutputString}
        OutputString={List.flatten [Color InputString RESET] $}
    end

    proc {MakeInfo InputAtom ?OutputAtom}
        InputString = {Atom.toString InputAtom $}
        OutputString = {MakeColoredString InputString BLUE $}
    in
        OutputAtom = {String.toAtom OutputString $}
    end

    proc {MakeSuccess InputAtom ?OutputAtom}
        InputString = {Atom.toString InputAtom $}
        OutputString = {MakeColoredString InputString GREEN $}
    in
        OutputAtom = {String.toAtom OutputString $}
    end

    proc {MakeWarning InputAtom ?OutputAtom}
        InputString = {Atom.toString InputAtom $}
        OutputString = {MakeColoredString InputString ORANGE $}
    in
        OutputAtom = {String.toAtom OutputString $}
    end

    proc {MakeError InputAtom ?OutputAtom}
        InputString = {Atom.toString InputAtom $}
        OutputString = {MakeColoredString InputString RED $}
    in
        OutputAtom = {String.toAtom OutputString $}
    end

    proc {MakeDebug InputAtom ?OutputAtom}
        InputString = {Atom.toString InputAtom $}
        OutputString = {MakeColoredString InputString PURPLE $}
    in
        OutputAtom = {String.toAtom OutputString $}
    end
in
    skip
end