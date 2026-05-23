%%%
%%% Authors:
%%%   Mattéo Dubuisson
%%%
%%% Contributors:
%%%   
%%%
%%% Copyright:
%%%   
%%%
%%% Last change:
%%%   $Date$ by $Author$
%%%   $Revision$
%%%
%%% This file is part of Mozart, an implementation
%%% of Oz 3
%%%    http://www.mozart-oz.org
%%%
%%% See the file "LICENSE" or
%%%    http://www.mozart-oz.org/LICENSE.html
%%% for information on usage and redistribution
%%% of this file, and for a DISCLAIMER OF ALL
%%% WARRANTIES.
%%%

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

OPCODES = opcodes(
    'skip': opcode(Skip)
    
    'moveXX': opcode(MoveXX [x x])
    'moveXY': opcode(MoveXY [x y])
    'moveYX': opcode(MoveYX [y x])
    'moveYY': opcode(MoveYY [y y])
    'moveGX': opcode(MoveGX [g x])
    'moveGY': opcode(MoveGY [g y])
    'moveKX': opcode(MoveKX [k x])
    'moveKY': opcode(MoveKY [k y])

    'movemoveXYXY': opcode(MoveMoveXYXY [x y x y])
    'movemoveYXYX': opcode(MoveMoveYXYX [y x y x])
    'movemoveYXXY': opcode(MoveMoveYXXY [y x x y])
    'movemoveXYYX': opcode(MoveMoveXYYX [x y y x])

    'allocateY': opcode(AllocateY [x])

    'createVarX': opcode(CreateVarX [x])
    'createVarY': opcode(CreateVarY [y])
    'createVarMoveX': opcode(CreateVarMoveX [x x])
    'createVarMoveY': opcode(CreateVarMoveY [y x])

    'setupExceptionHandler': opcode(SetupExceptionHandler [distance])
    'popExceptionHandler': opcode(PopExceptionHandler)

    'callBuiltin0': opcode(CallBuiltin0 [k])
    'callBuiltin1': opcode(CallBuiltin1 [k x])
    'callBuiltin2': opcode(CallBuiltin2 [k x x])
    'callBuiltin3': opcode(CallBuiltin3 [k x x x])
    'callBuiltin4': opcode(CallBuiltin4 [k x x x x])
    'callBuiltin5': opcode(CallBuiltin5 [k x x x x x])
    'callBuiltinN': opcode(CallBuiltinN [k v length x(length)])

    'callX': opcode(CallX [x arity])
    'callY': opcode(CallY [y arity])
    'callG': opcode(CallG [g arity])
    'callK': opcode(CallK [k arity])

    'tailCallX': opcode(TailCallX [x arity])
    'tailCallY': opcode(CallY [y arity])
    'tailCallG': opcode(CallG [g arity])
    'tailCallK': opcode(CallK [k arity])

    'sendMsgX': opcode(SendMsgX [x k width])
    'sendMsgY': opcode(SendMsgY [y k width])
    'sendMsgG': opcode(SendMsgG [g k width])
    'sendMsgK': opcode(SendMsgK [k k width])

    'tailSendMsgX': opcode(TailSendMsgX [x k width])
    'tailSendMsgY': opcode(TailSendMsgY [y k width])
    'tailSendMsgG': opcode(TailSendMsgG [g k width])
    'tailSendMsgK': opcode(TailSendMsgK [k k width])

    'return': opcode(Return)
    'branch': opcode(Branch distance)
    'branchBackward': opcode(BranchBackward distance)
    'condBranch': opcode(CondBranch x)
    'condBranchFB': opcode(CondBranchFB x)
    'condBranchBF': opcode(CondBranchBF x)
    'condBranchBB': opcode(CondBranchBB x)

    'patternMatchX': opcode(PatternMatchX [x k])
    'patternMatchY': opcode(PatternMatchY [y k])
    'patternMatchG': opcode(PatternMatchG [g k])

    'unifyXX': opcode(UnifyXX [x x])
    'unifyXY': opcode(UnifyXY [x y])
    'unifyXG': opcode(UnifyXG [x g])
    'unifyXK': opcode(UnifyXK [x k])
    'unifyYY': opcode(UnifyYY [y y])
    'unifyYG': opcode(UnifyYG [y g])
    'unifyYK': opcode(UnifyYK [y k])
    'unifyGG': opcode(UnifyGG [g g])
    'unifyGK': opcode(UnifyGK [g k])
    'unifyKK': opcode(UnifyKK [k k])

    'createAbstractionStoreX': opcode(CreateAbstractionStoreX [k length x])
    'createConsStoreX': opcode(CreateConsStoreX [k length x])
    'createTupleStoreX': opcode(CreateTupleStoreX [k length x])
    'createRecordStoreX': opcode(CreateRecordStoreX [k length x])

    'createAbstractionStoreY': opcode(CreateAbstractionStoreY [k length y])
    'createConsStoreY': opcode(CreateConsStoreY [k length y])
    'createTupleStoreY': opcode(CreateTupleStoreY [k length y])
    'createRecordStoreY': opcode(CreateRecordStoreY [k length y])

    'createAbstractionUnifyX': opcode(CreateAbstractionUnifyX [k length x])
    'createConsUnifyX': opcode(CreateConsUnifyX [k length x])
    'createTupleUnifyX': opcode(CreateTupleUnifyX [k length x])
    'createRecordUnifyX': opcode(CreateRecordUnifyX [k length x])

    'createAbstractionUnifyY': opcode(CreateAbstractionUnifyY [k length y])
    'createConsUnifyY': opcode(CreateConsUnifyY [k length y])
    'createTupleUnifyY': opcode(CreateTupleUnifyY [k length y])
    'createRecordUnifyY': opcode(CreateRecordUnifyY [k length y])

    'createAbstractionUnifyG': opcode(CreateAbstractionUnifyG [k length g])
    'createConsUnifyG': opcode(CreateConsUnifyG [k length g])
    'createTupleUnifyG': opcode(CreateTupleUnifyG [k length g])
    'createRecordUnifyG': opcode(CreateRecordUnifyG [k length g])

    'createAbstractionUnifyK': opcode(CreateAbstractionUnifyK [k length k])
    'createConsUnifyK': opcode(CreateConsUnifyK [k length k])
    'createTupleUnifyK': opcode(CreateTupleUnifyK [k length k])
    'createRecordUnifyK': opcode(CreateRecordUnifyK [k length k])

    'inlineEqualsInteger': opcode(InlineEqualsInteger [x value distance])
    'inlineAdd': opcode(InlineAdd [x x x])
    'inlineSubtract': opcode(InlineSubtract [x x x])
    'inlinePlus1': opcode(InlinePlus1 [x x])
    'inlineMinus1': opcode(InlineMinus1 [x x])
    'inlineGetClass': opcode(InlineGetClass [x x])

    'debugEntry': opcode(DebugEntry [k line column k])
    'debugExit': opcode(DebugExit [k line column k])
    'localVarname': opcode(LocalVarname)
    'globalVarname': opcode(GlobalVarname)
    'clearY': opcode(ClearY)
)