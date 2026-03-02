functor

import
  Application  
  Open
  OS
  System
define
  Skip = 0x00

  MoveXX = 0x01
  MoveXY = 0x02
  MoveYX = 0x03
  MoveYY = 0x04
  MoveGX = 0x05
  MoveGY = 0x06
  MoveKX = 0x07
  MoveKY = 0x08

  MoveMoveXYXY = 0x09
  MoveMoveYXYX = 0x0A
  MoveMoveYXXY = 0x0B
  MoveMoveXYYX = 0x0C

  AllocateY = 0x0D

  CreateVarX = 0x0F
  CreateVarY = 0x10
  CreateVarMoveX = 0x11
  CreateVarMoveY = 0x12

  SetupExceptionHandler = 0x18
  PopExceptionHandler = 0x19

  CallBuiltin0 = 0x20
  CallBuiltin1 = 0x21
  CallBuiltin2 = 0x22
  CallBuiltin3 = 0x23
  CallBuiltin4 = 0x24
  CallBuiltin5 = 0x25
  CallBuiltinN = 0x26

  CallX = 0x27
  CallY = 0x28
  CallG = 0x29
  CallK = 0x2A
  TailCallX = 0x2B
  TailCallY = 0x2C
  TailCallG = 0x2D
  TailCallK = 0x2E

  SendMsgX = 0x30
  SendMsgY = 0x31
  SendMsgG = 0x32
  SendMsgK = 0x33
  TailSendMsgX = 0x34
  TailSendMsgY = 0x35
  TailSendMsgG = 0x36
  TailSendMsgK = 0x37

  Return = 0x40
  Branch = 0x41
  BranchBackward = 0x42
  CondBranch = 0x43
  CondBranchFB = 0x44
  CondBranchBF = 0x45
  CondBranchBB = 0x46

  PatternMatchX = 0x47
  PatternMatchY = 0x48
  PatternMatchG = 0x49

  UnifyXX = 0x50
  UnifyXY = 0x51
  UnifyXG = 0x52
  UnifyXK = 0x53
  UnifyYY = 0x54
  UnifyYG = 0x55
  UnifyYK = 0x56
  UnifyGG = 0x57
  UnifyGK = 0x58
  UnifyKK = 0x59

  CreateAbstractionStoreX = 0x60
  CreateConsStoreX = 0x61
  CreateTupleStoreX = 0x62
  CreateRecordStoreX = 0x63

  CreateAbstractionStoreY = 0x64
  CreateConsStoreY = 0x65
  CreateTupleStoreY = 0x66
  CreateRecordStoreY = 0x67

  CreateAbstractionUnifyX = 0x68
  CreateConsUnifyX = 0x69
  CreateTupleUnifyX = 0x6A
  CreateRecordUnifyX = 0x6B

  CreateAbstractionUnifyY = 0x6C
  CreateConsUnifyY = 0x6D
  CreateTupleUnifyY = 0x6E
  CreateRecordUnifyY = 0x6F

  CreateAbstractionUnifyG = 0x70
  CreateConsUnifyG = 0x71
  CreateTupleUnifyG = 0x72
  CreateRecordUnifyG = 0x73

  CreateAbstractionUnifyK = 0x74
  CreateConsUnifyK = 0x75
  CreateTupleUnifyK = 0x76
  CreateRecordUnifyK = 0x77

  ArrayFillX = 0
  ArrayFillY = 1
  ArrayFillG = 2
  ArrayFillK = 3
  ArrayFillNewVarX = 4
  ArrayFillNewVarY = 5
  ArrayFillNewVars = 6

  InlineEqualsInteger = 0x80
  InlineAdd = 0x81
  InlineSubtract = 0x82
  InlinePlus1 = 0x83
  InlineMinus1 = 0x84
  InlineGetClass = 0x90

  DebugEntry = 0xa0
  DebugExit = 0xa1
  LocalVarname = 0xa2
  GlobalVarname = 0xa3
  ClearY = 0xa4

  fun {ExtractArguments FD Count}
    % This procedure ensures to read exactly 'Size' bytes
    proc {Read FD Size ?Bytes}
      Length Tail
    in
      Bytes = {FD read(list:$ tail:Tail size:Size len:Length)}
      if Size - Length == 0 then Tail = nil
      else Tail = {Read FD Size - Length $} end
    end

    proc {Refactor Bytes Result}
      case Bytes of FirstByte|SecondByte|Tail then
        Word = FirstByte * 256 + SecondByte
        Next
      in
        Result = Word|Next
        {Refactor Tail Next}
      [] nil then Result = nil end
    end

    Bytes = {Read FD Count * 2 $}
    Result
  in
    {Refactor Bytes Result}
    Result
  end

  proc {PrintInstruction InstructionOffSet UseAnsi ActionName Arguments}
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    PURPLE = "\033[35m"
    CYAN = "\033[36m"
    NO_COLOR = "\033[0m"

    proc {AppendArguments Arguments ?Result}
      case Arguments of Argument|NextArguments then
        ArgumentByteString
      in
        case Argument of x(Value) then
          ArgumentByteString = {ByteString.make CYAN#"%x("#NO_COLOR#Value#CYAN#")"#NO_COLOR $}
        [] y(Value) then
          ArgumentByteString = {ByteString.make CYAN#"%y("#NO_COLOR#Value#CYAN#")"#NO_COLOR $}
        [] g(Value) then
          ArgumentByteString = {ByteString.make CYAN#"%g("#NO_COLOR#Value#CYAN#")"#NO_COLOR $}
        [] k(Value) then
          ArgumentByteString = {ByteString.make CYAN#"%k("#NO_COLOR#Value#CYAN#")"#NO_COLOR $}
        [] data(Value) then
          ArgumentByteString = {ByteString.make YELLOW#"'"#{Int.toString Value $}#"'"#NO_COLOR $}
        [] word(Word) then
          ArgumentByteString = {ByteString.make YELLOW#"'"#{Int.toString Word $}#"'"#NO_COLOR $}
        [] array(size:Size type:Type list:List) then
          proc {FormatArrayArguments Type List ?Arguments}
            case List of Value|NextList then
              Argument
              NextArguments
            in
              case Type of x() then
                Argument = x(Value)
              [] y() then
                Argument = y(Value)
              [] g() then
                Argument = g(Value)
              [] k() then
                Argument = k(Value)
              [] data() then
                Argument = data(Value)
              end
              Arguments = Argument|NextArguments
              {FormatArrayArguments Type NextList NextArguments}
            [] nil then Arguments = nil end
          end
        in
          ArgumentByteString = {AppendArguments {FormatArrayArguments Type List $} $}
        end

        local PartialResult
          IsLastArgument = {Value.isDet NextArguments $} andthen NextArguments == nil
        in
          PartialResult = {AppendArguments NextArguments $}
          Result = {ByteString.append
            if IsLastArgument then ArgumentByteString
            else {ByteString.append ArgumentByteString {ByteString.make "," $} $} end
            PartialResult
            $
          }
        end
      [] nil then Result = {ByteString.make "" $} end
    end
    ActionNameByteString = {ByteString.make GREEN#InstructionOffSet#NO_COLOR#"\t"#PURPLE#ActionName#NO_COLOR#"\t" $}
    ArgumentsByteString = {AppendArguments Arguments $}
    InstructionByteString = {ByteString.append ActionNameByteString ArgumentsByteString $}
  in
    {System.showInfo {ByteString.toString InstructionByteString $}}
  end

  proc {ParseStructOperation Op KRegister Length UnknownRegister ?OperationName ?OperationFormat}
    proc {GetStructType Op ?Type}
      Abstractions = [CreateAbstractionStoreX CreateAbstractionStoreY
      CreateAbstractionUnifyX CreateAbstractionUnifyY
      CreateAbstractionUnifyG CreateAbstractionUnifyK]
      Conss = [CreateConsStoreX CreateConsStoreY
        CreateConsUnifyX CreateConsUnifyY
        CreateConsUnifyG CreateConsUnifyK]
      Tuples = [CreateTupleStoreX CreateTupleStoreY
        CreateTupleUnifyX CreateTupleUnifyY
        CreateTupleUnifyG CreateTupleUnifyK]
      Records = [CreateRecordStoreX CreateRecordStoreY
        CreateRecordUnifyX CreateRecordUnifyY
        CreateRecordUnifyG CreateRecordUnifyK]
    in
      if {List.member Op Abstractions $} then Type = 'Abstraction'
      elseif {List.member Op Conss $} then Type = 'Cons'
      elseif {List.member Op Tuples $} then Type = 'Tuple'
      elseif {List.member Op Records $} then Type = 'Record'
      else Type = 'None' end
    end

    proc {GetOperationType Op ?Type}
      StoreXs = [CreateAbstractionStoreX CreateConsStoreX
        CreateTupleStoreX CreateRecordStoreX]
      StoreYs = [CreateAbstractionStoreY CreateConsStoreY
        CreateTupleStoreY CreateRecordStoreY]
      UnifyXs = [CreateAbstractionUnifyX CreateConsUnifyX
        CreateTupleUnifyX CreateRecordUnifyX]
      UnifyYs = [CreateAbstractionUnifyY CreateConsUnifyY
        CreateTupleUnifyY CreateRecordUnifyY]
      UnifyGs = [CreateAbstractionUnifyG CreateConsUnifyG
        CreateTupleUnifyG CreateRecordUnifyG]
      UnifyKs = [CreateAbstractionUnifyK CreateConsUnifyK
        CreateTupleUnifyK CreateRecordUnifyK]
    in
      if {List.member Op StoreXs $} then Type = 'StoreX'
      elseif {List.member Op StoreYs $} then Type = 'StoreY'
      elseif {List.member Op UnifyXs $} then Type = 'UnifyX'
      elseif {List.member Op UnifyYs $} then Type = 'UnifyY'
      elseif {List.member Op UnifyGs $} then Type = 'UnifyG'
      elseif {List.member Op UnifyKs $} then Type = 'UnifyK'
      else Type = 'None' end
    end

    What = {GetStructType Op $}
    Where = {GetOperationType Op $}
    DestinationType
  in
    case Where of 'StoreX' then
      DestinationType = x(UnknownRegister)
    [] 'StoreY' then
      DestinationType = y(UnknownRegister)
    [] 'UnifyX' then
      DestinationType = x(UnknownRegister)
    [] 'UnifyY' then
      DestinationType = y(UnknownRegister)
    [] 'UnifyG' then
      DestinationType = g(UnknownRegister)
    [] 'UnifyK' then
      DestinationType = k(UnknownRegister)
    end
    OperationName = "Create"#What#Where
    OperationFormat = [k(KRegister) data(Length) DestinationType]
  end

  proc {ParseFile FD InstructionOffSet DisplayAll UseAnsi}
    Arguments
  in
      case {FD read(list: $ size: 2)} of [FirstByte SecondByte] then
        Command = (FirstByte * 256) + SecondByte
      in
        if Command == Skip then
          {PrintInstruction InstructionOffSet UseAnsi "Skip" nil}
        elseif Command == DebugEntry then
          case {ExtractArguments FD 4} of [KFile LineNumber ColumnNumber KKind] then
            {PrintInstruction InstructionOffSet UseAnsi "DebugEntry" [k(KFile) data(LineNumber) data(ColumnNumber) k(KKind)]}
          end
        elseif Command == DebugExit then
          case {ExtractArguments FD 4} of [KFile LineNumber ColumnNumber KKind] then
            {PrintInstruction InstructionOffSet UseAnsi "DebugExit" [k(KFile) data(LineNumber) data(ColumnNumber) k(KKind)]}
          end
        elseif Command == LocalVarname then
          case {ExtractArguments FD 1} of [VarName] then
            {PrintInstruction InstructionOffSet UseAnsi "LocalVarname" [data(VarName)]}
          end
        elseif Command == GlobalVarname then
          case {ExtractArguments FD 1} of [VarName] then
            {PrintInstruction InstructionOffSet UseAnsi "GlobalVarname" [data(VarName)]}
          end
        elseif Command == ClearY then
          case {ExtractArguments FD 1} of [Y] then
            {PrintInstruction InstructionOffSet UseAnsi "ClearY" [y(Y)]}
          end

        elseif Command == MoveXX then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveXX" [x(X0) x(X1)]}
          end
        elseif Command == MoveXY then
          case {ExtractArguments FD 2} of [X Y] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveXY" [x(X) y(Y)]}
          end
        elseif Command == MoveYX then
          case {ExtractArguments FD 2} of [Y X] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveYX" [y(Y) x(X)]}
          end
        elseif Command == MoveYY then
          case {ExtractArguments FD 2} of [Y0 Y1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveYY" [y(Y0) y(Y1)]}
          end
        elseif Command == MoveGX then
          case {ExtractArguments FD 2} of [G X] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveGX" [g(G) x(X)]}
          end
        elseif Command == MoveGY then
          case {ExtractArguments FD 2} of [G Y] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveGY" [g(G) y(Y)]}
          end
        elseif Command == MoveKX then
          case {ExtractArguments FD 2} of [K X] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveKX" [k(K) x(X)]}
          end
        elseif Command == MoveKY then
          case {ExtractArguments FD 2} of [K Y] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveKY" [k(K) y(Y)]}
          end

        elseif Command == MoveMoveXYXY then
          case {ExtractArguments FD 4} of [X0 Y0 X1 Y1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveMoveXYXY" [x(X0) y(Y0) x(X1) y(Y1)]}
          end
        elseif Command == MoveMoveYXYX then
          case {ExtractArguments FD 4} of [Y0 X0 Y1 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveMoveYXYX" [y(Y0) x(X0) y(Y1) x(X1)]}
          end
        elseif Command == MoveMoveYXXY then
          case {ExtractArguments FD 4} of [Y0 X0 X1 Y1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveMoveXYXY" [y(Y0) x(X0) x(X1) y(Y1)]}
          end
        elseif Command == MoveMoveXYYX then
          case {ExtractArguments FD 4} of [X0 Y0 Y1 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "MoveMoveXYXY" [x(X0) y(Y0) y(Y1) x(X1)]}
          end

        elseif Command == AllocateY then
          case {ExtractArguments FD 1} of [Count] then
            {PrintInstruction InstructionOffSet UseAnsi "AllocateY" [data(Count)]}
          end

        elseif Command == CreateVarX then
          case {ExtractArguments FD 1} of [X] then
            {PrintInstruction InstructionOffSet UseAnsi "CreateVarX" [x(X)]}
          end
        elseif Command == CreateVarY then
          case {ExtractArguments FD 1} of [Y] then
            {PrintInstruction InstructionOffSet UseAnsi "CreateVarY" [y(Y)]}
          end
        elseif Command == CreateVarMoveX then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "CreateVarMoveX" [x(X0) x(X1)]}
          end
        elseif Command == CreateVarMoveY then
          case {ExtractArguments FD 2} of [Y X] then
            {PrintInstruction InstructionOffSet UseAnsi "CreateVarMoveY" [y(Y) x(X)]}
          end

        elseif Command == SetupExceptionHandler then
          case {ExtractArguments FD 1} of [OffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "SetupExceptionHandler" [data(OffSet)]}
          end
        elseif Command == PopExceptionHandler then
          {PrintInstruction InstructionOffSet UseAnsi "PopExceptionHandler" nil}

        elseif Command == CallBuiltin0 then
          case {ExtractArguments FD 1} of [K] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin0" [k(K)]}
          end
        elseif Command == CallBuiltin1 then
          case {ExtractArguments FD 2} of [K X] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin1" [k(K) x(X)]}
          end
        elseif Command == CallBuiltin2 then
          case {ExtractArguments FD 3} of [K X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin2" [k(K) x(X0) x(X1)]}
          end
        elseif Command == CallBuiltin3 then
          case {ExtractArguments FD 4} of [K X0 X1 X2] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin3" [k(K) x(X0) x(X1) x(X2)]}
          end
        elseif Command == CallBuiltin4 then
          case {ExtractArguments FD 5} of [K X0 X1 X2 X3] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin4" [k(K) x(X0) x(X1) x(X2) x(X3)]}
          end
        elseif Command == CallBuiltin5 then
          case {ExtractArguments FD 6} of [K X0 X1 X2 X3 X4] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltin5" [k(K) x(X0) x(X1) x(X2) x(X3) x(X4)]}
          end
        elseif Command == CallBuiltinN then
          case {ExtractArguments FD 2} of [K Count] then
            {PrintInstruction InstructionOffSet UseAnsi "CallBuiltinN" [k(K) data(Count) array(
              size:Count type:x() list:{ExtractArguments FD Count})]}
          end

        elseif Command == CallX then
          case {ExtractArguments FD 2} of [X Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "CallX" [x(X) data(Arity)]}
          end
        elseif Command == CallY then
          case {ExtractArguments FD 2} of [Y Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "CallY" [y(Y) data(Arity)]}
          end
        elseif Command == CallG then
          case {ExtractArguments FD 2} of [G Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "CallG" [g(G) data(Arity)]}
          end
        elseif Command == CallK then
          case {ExtractArguments FD 2} of [K Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "CallK" [k(K) data(Arity)]}
          end
        elseif Command == TailCallX then
          case {ExtractArguments FD 2} of [X Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "TailCallX" [x(X) data(Arity)]}
          end
        elseif Command == TailCallY then
          case {ExtractArguments FD 2} of [Y Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "TailCallY" [y(Y) data(Arity)]}
          end
        elseif Command == TailCallG then
          case {ExtractArguments FD 2} of [G Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "TailCallG" [g(G) data(Arity)]}
          end
        elseif Command == TailCallK then
          case {ExtractArguments FD 2} of [K Arity] then
            {PrintInstruction InstructionOffSet UseAnsi "TailCallK" [k(K) data(Arity)]}
          end

        elseif Command == SendMsgX then
          case {ExtractArguments FD 3} of [X K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "SendMsgX" [x(X) k(K) data(Size)]}
          end
        elseif Command == SendMsgY then
          case {ExtractArguments FD 3} of [Y K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "SendMsgY" [y(Y) k(K) data(Size)]}
          end
        elseif Command == SendMsgG then
          case {ExtractArguments FD 3} of [G K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "SendMsgG" [g(G) k(K) data(Size)]}
          end
        elseif Command == SendMsgK then
          case {ExtractArguments FD 3} of [K0 K1 Size] then
            {PrintInstruction InstructionOffSet UseAnsi "SendMsgK" [k(K0) k(K1) data(Size)]}
          end
        elseif Command == TailSendMsgX then
          case {ExtractArguments FD 3} of [X K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "TailSendMsgX" [x(X) k(K) data(Size)]}
          end
        elseif Command == TailSendMsgY then
          case {ExtractArguments FD 3} of [Y K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "TailSendMsgY" [y(Y) k(K) data(Size)]}
          end
        elseif Command == TailSendMsgG then
          case {ExtractArguments FD 3} of [G K Size] then
            {PrintInstruction InstructionOffSet UseAnsi "TailSendMsgG" [g(G) k(K) data(Size)]}
          end
        elseif Command == TailSendMsgK then
          case {ExtractArguments FD 3} of [K0 K1 Size] then
            {PrintInstruction InstructionOffSet UseAnsi "TailSendMsgK" [k(K0) k(K1) data(Size)]}
          end

        elseif Command == Return then
          {PrintInstruction InstructionOffSet UseAnsi "Return" nil}
        elseif Command == Branch then
          case {ExtractArguments FD 1} of [OffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "Branch" [data(OffSet)]}
          end
        elseif Command == BranchBackward then
          case {ExtractArguments FD 1} of [OffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "BranchBackward" [data(OffSet)]}
          end
        elseif Command == CondBranch then
          case {ExtractArguments FD 3} of [X FalseOffSet ErrorOffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "CondBranch" [x(X) data(FalseOffSet) data(ErrorOffSet)]}
          end
        elseif Command == CondBranchFB then
          case {ExtractArguments FD 3} of [X FalseOffSet ErrorOffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "CondBranchFB" [x(X) data(FalseOffSet) data(ErrorOffSet)]}
          end
        elseif Command == CondBranchBF then
          case {ExtractArguments FD 3} of [X FalseOffSet ErrorOffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "CondBranchBF" [x(X) data(FalseOffSet) data(ErrorOffSet)]}
          end
        elseif Command == CondBranchBB then
          case {ExtractArguments FD 3} of [X FalseOffSet ErrorOffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "CondBranchBB" [x(X) data(FalseOffSet) data(ErrorOffSet)]}
          end

        elseif Command == PatternMatchX then
          case {ExtractArguments FD 2} of [X K] then
            {PrintInstruction InstructionOffSet UseAnsi "PatternMatchX" [x(X) k(K)]}
          end
        elseif Command == PatternMatchY then
          case {ExtractArguments FD 2} of [Y K] then
            {PrintInstruction InstructionOffSet UseAnsi "PatternMatchY" [y(Y) k(K)]}
          end
        elseif Command == PatternMatchG then
          case {ExtractArguments FD 2} of [G K] then
            {PrintInstruction InstructionOffSet UseAnsi "PatternMatchG" [g(G) k(K)]}
          end

        elseif Command == UnifyXX then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyXX" [x(X0) x(X1)]}
          end
        elseif Command == UnifyXY then
          case {ExtractArguments FD 2} of [X Y] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyXY" [x(X) y(Y)]}
          end
        elseif Command == UnifyXG then
          case {ExtractArguments FD 2} of [X G] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyXG" [x(X) g(G)]}
          end
        elseif Command == UnifyXK then
          case {ExtractArguments FD 2} of [X K] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyXK" [x(X) k(K)]}
          end
        elseif Command == UnifyYY then
          case {ExtractArguments FD 2} of [Y0 Y1] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyYY" [y(Y0) y(Y1)]}
          end
        elseif Command == UnifyYG then
          case {ExtractArguments FD 2} of [Y G] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyYG" [y(Y) g(G)]}
          end
        elseif Command == UnifyYK then
          case {ExtractArguments FD 2} of [Y K] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyYK" [y(Y) k(K)]}
          end
        elseif Command == UnifyGG then
          case {ExtractArguments FD 2} of [G0 G1] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyGG" [g(G0) g(G1)]}
          end
        elseif Command == UnifyGK then
          case {ExtractArguments FD 2} of [G K] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyGK" [g(G) k(K)]}
          end
        elseif Command == UnifyKK then
          case {ExtractArguments FD 2} of [K0 K1] then
            {PrintInstruction InstructionOffSet UseAnsi "UnifyKK" [k(K0) k(K1)]}
          end

        elseif Command == CreateAbstractionStoreX
        orelse Command == CreateConsStoreX
        orelse Command == CreateTupleStoreX
        orelse Command == CreateRecordStoreX
        orelse Command == CreateAbstractionStoreY
        orelse Command == CreateConsStoreY
        orelse Command == CreateTupleStoreY
        orelse Command == CreateRecordStoreY

        orelse Command == CreateAbstractionUnifyX
        orelse Command == CreateConsUnifyX
        orelse Command == CreateTupleUnifyX
        orelse Command == CreateRecordUnifyX

        orelse Command == CreateAbstractionUnifyY
        orelse Command == CreateConsUnifyY
        orelse Command == CreateTupleUnifyY
        orelse Command == CreateRecordUnifyY

        orelse Command == CreateAbstractionUnifyG
        orelse Command == CreateConsUnifyG
        orelse Command == CreateTupleUnifyG
        orelse Command == CreateRecordUnifyG

        orelse Command == CreateAbstractionUnifyK
        orelse Command == CreateConsUnifyK
        orelse Command == CreateTupleUnifyK
        orelse Command == CreateRecordUnifyK then
          OperationName OperationFormat
        in
          case {ExtractArguments FD 5} of [K Length V ArrayOp ArrayValue] then
            {ParseStructOperation Command K Length V OperationName OperationFormat}
            {PrintInstruction InstructionOffSet UseAnsi OperationName OperationFormat}

            case ArrayOp of ArrayFillX then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillX" [x(ArrayValue)]}
            [] ArrayFillY then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillY" [y(ArrayValue)]}
            [] ArrayFillG then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillG" [g(ArrayValue)]}
            [] ArrayFillK then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillK" [k(ArrayValue)]}
            [] ArrayFillNewVarX then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillNewVarX" [x(ArrayValue)]}
            [] ArrayFillNewVarY then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillNewVarY" [y(ArrayValue)]}
            [] ArrayFillNewVars then
              {PrintInstruction InstructionOffSet UseAnsi "ArrayFillNewVars" [data(ArrayValue)]}
            end
          end

        elseif Command == InlineEqualsInteger then
          case {ExtractArguments FD 3} of [X Value OffSet] then
            {PrintInstruction InstructionOffSet UseAnsi "InlineEqualsInteger" [x(X) data(Value) data(OffSet)]}
          end
        elseif Command == InlineAdd then
          case {ExtractArguments FD 3} of [X0 X1 X2] then
            {PrintInstruction InstructionOffSet UseAnsi "InlineAdd" [x(X0) x(X1) x(X2)]}
          end
        elseif Command == InlineSubtract then
          case {ExtractArguments FD 3} of [X0 X1 X2] then
            {PrintInstruction InstructionOffSet UseAnsi "InlineSubtract" [x(X0) x(X1) x(X2)]}
          end
        elseif Command == InlinePlus1 then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "InlinePlus1" [x(X0) x(X1)]}
          end
        elseif Command == InlineMinus1 then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "InlineMinus1" [x(X0) x(X1)]}
          end
        elseif Command == InlineGetClass then
          case {ExtractArguments FD 2} of [X0 X1] then
            {PrintInstruction InstructionOffSet UseAnsi "InlineGetClass" [x(X0) x(X1)]}
          end
        else
          {PrintInstruction InstructionOffSet UseAnsi "Word" [word(Command)]}
        end

        {ParseFile FD InstructionOffSet + 1 DisplayAll UseAnsi}
    else skip end
  end
in
  
  case {Application.getArgs record()} of optRec([FileName]) then
    FD = {New Open.file init(
      name: {String.toAtom FileName $}
      flags: [read binary]
    )}
  in
    {ParseFile FD 0 false true}
    {FD close()}
  end
end