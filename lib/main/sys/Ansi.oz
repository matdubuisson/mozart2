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
    ESC = '\033'
    CSI = ESC#'['
    RESET = CSI#'0m'

    RED = CSI#'38;5;1m'
    GREEN = CSI#'38;5;2m'
    ORANGE = CSI#'38;5;3m'
    BLUE = CSI#'38;5;4m'
    PURPLE = CSI#'38;5;5m'
    CYAN = CSI#'38;5;6m'
    WHITE = CSI#'38;5;7m'
    BLACK = CSI#'38;5;8m'

    proc {MakeInfo Input ?Output}
        Output = BLUE#Input#RESET
    end

    proc {MakeSuccess Input ?Output}
        Output = GREEN#Input#RESET
    end

    proc {MakeWarning Input ?Output}
        Output = ORANGE#Input#RESET
    end

    proc {MakeError Input ?Output}
        Output = RED#Input#RESET
    end

    proc {MakeDebug Input ?Output}
        Output = PURPLE#Input#RESET
    end
in
    skip
end