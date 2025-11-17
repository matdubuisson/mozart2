// Copyright © 2011, Université catholique de Louvain
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MOZART_OPCODES_H
#define MOZART_OPCODES_H

namespace mozart {

typedef short unsigned int uint16_t;

typedef uint16_t ByteCode;
typedef ByteCode OpCode;

typedef const ByteCode* ProgramCounter;

/**
 * Four register types explanation :
 *    - X registers are mutable, owned by a thread's current scope and used
 *        for local computation. They are dynamically allocated on need, they
 *        are invalidated after a procedure call and are destroyed on thread death.
 *    - Y registers are mutable, temporary storage allocated owned by the stack frame.
 *        They are explictly (de)allocated based on byte-code instructions.
 *    - G registers are immutable and contain environment values.
 *    - K registers are immutable and contain defined constant values.
 */

/**
 * Byte-code explanation :
 *    This is an array of unsigned short integers and each instruction is composed of
 * two parts : first an opcode (see macros below) and optional arguments such that the
 * number is variable and depends on the opcode. There are two types of argument :
 * constant values and register indexes.
 */

/**
 * Simple instruction doing nothing
 */
const OpCode OpSkip = 0x00;

/**
 * Copy the value from source register into a destination register
 * @param RegisterSource a register of any type (X, Y, G or K)
 * @param RegisterDestination a register of type X or Y
 * @remark As G and K registers are immutable, there are no instructions to output something inside these register types
 */
const OpCode OpMoveXX = 0x01, OpMoveXY = 0x02, OpMoveYX = 0x03, OpMoveYY = 0x04, OpMoveGX = 0x05, OpMoveGY = 0x06, OpMoveKX = 0x07, OpMoveKY = 0x08;

/**
 * Execute simultaneously two mode instructions
 * @param RegisterSource1 a register of type X or Y
 * @param RegisterDestination1 a register of type X or Y
 * @param RegisterSource2 a register of type X or Y
 * @param RegisterDestination2 a register of type X or Y
 * @remark It has been developed for an optimization purpose
 */
const OpCode OpMoveMoveXYXY = 0x09, OpMoveMoveYXYX = 0x0A, OpMoveMoveYXXY = 0x0B, OpMoveMoveXYYX = 0x0C;

/**
 * Allocates a specified number of Y registers
 * @param Count the number of allocated Y registers
 * @remark Deallocation is automatically done by the garbage collector
 */
const OpCode OpAllocateY = 0x0D;

/**
 * Create a new unbound variable into the specified register
 * @param XRegisterDestination
 */
const OpCode OpCreateVarX = 0x0F;
/**
 * Create a new unbound variable into the specified register
 * @param YRegisterDestination
 */
const OpCode OpCreateVarY = 0x10;

/**
 * Create a new unbound variable into the specified register and copy it into the second specified register
 * @param XDestinationRegister1
 * @param XDestinationRegister2
 */
const OpCode OpCreateVarMoveX = 0x11;
/**
 * Create a new unbound variable into the specified register and copy it into the second specified register
 * @param YDestinationRegister1
 * @param XDestinationRegister2
 */
const OpCode OpCreateVarMoveY = 0x12;

/**
 * Push an exception handler on the stack and from the next instruction, a triggered exception will redirect the control flow at the specified offset and the handler is removed from the stack
 * @param Offset address number
 * @remark All X registers are invalidated, the first X register X(0) contains the exception
 */
const OpCode OpSetupExceptionHandler = 0x18;
/**
 * Pop an execution handler from the stack
 * @remark The top of the stack must be an exception handler
 */
const OpCode OpPopExceptionHandler = 0x19;

/**
 * Call a C++ builtin abstraction with an arity of 0 argument
 * @param KRegister the address of the code space
 */
const OpCode OpCallBuiltin0 = 0x20;
/**
 * Call a C++ builtin abstraction with an arity of 1 argument
 * @param KRegister the address of the code space
 * @param XRegister an argument value
 */
const OpCode OpCallBuiltin1 = 0x21;
/**
 * Call a C++ builtin abstraction with an arity of 2 arguments
 * @param KRegister the address of the code space
 * @param XRegister1 an argument value
 * @param XRegister2 an argument value
 */
const OpCode OpCallBuiltin2 = 0x22;
/**
 * Call a C++ builtin abstraction with an arity of 3 arguments
 * @param KRegister the address of the code space
 * @param XRegister1 an argument value
 * @param XRegister2 an argument value
 * @param XRegister3 an argument value
 */
const OpCode OpCallBuiltin3 = 0x23;
/**
 * Call a C++ builtin abstraction with an arity of 4 arguments
 * @param KRegister the address of the code space
 * @param XRegister an argument value
 * @param XRegister an argument value
 * @param XRegister an argument value
 * @param XRegister an argument value
 */
const OpCode OpCallBuiltin4 = 0x24;
/**
 * Call a C++ builtin abstraction with an arity of 5 arguments
 * @param KRegister the address of the code space
 * @param XRegister1 an argument value
 * @param XRegister2 an argument value
 * @param XRegister3 an argument value
 * @param XRegister4 an argument value
 * @param XRegister5 an argument value
 */
const OpCode OpCallBuiltin5 = 0x25;
/**
 * Call a C++ builtin abstraction with an arity of N arguments
 * @param KRegister the address of the code space
 * @param XRegister1 an argument value
 * @param XRegister2 an argument value
 * ....
 * @param XRegisterN an argument value
 */
const OpCode OpCallBuiltinN = 0x26;

/**
 * Calls a procedure target (abstraction, builtin, object, etc) with arity actual parameters. When the call is terminated, it pushes the current state on top of the stack and invalidate all X registers
 * @param Register register of any type containing the address of the procedure
 * @param Arity the number of arguments taken by the procedure
 * @remark Arguments are provided through X registers indexed from 0 to Arity - 1
 */
const OpCode OpCallX = 0x27, OpCallY = 0x28, OpCallG = 0x29, OpCallK = 0x2A;

/**
 * Equivalent to OpCall operations + OpReturn. It doesn't consume stack space
 * @param Register register of any type containing the address of the procedure
 * @param Arity the number of arguments taken by the procedure
 * @remark Arguments are provided through X registers indexed from 0 to Arity - 1
 */
const OpCode OpTailCallX = 0x2B, OpTailCallY = 0x2C, OpTailCallG = 0x2D, OpTailCallK = 0x2E;

const OpCode OpSendMsgX = 0x30;
const OpCode OpSendMsgY = 0x31;
const OpCode OpSendMsgG = 0x32;
const OpCode OpSendMsgK = 0x33;
const OpCode OpTailSendMsgX = 0x34;
const OpCode OpTailSendMsgY = 0x35;
const OpCode OpTailSendMsgG = 0x36;
const OpCode OpTailSendMsgK = 0x37;

/**
 * Pops a stack frame off the stack and installs it in other words returns from the current invocation of a procedure
 * @remark The current stack frale has no allocated Y registers and this is a regular stack frame
 */
const OpCode OpReturn = 0x40;
/**
 * Jumps to the specified address label
 * @package Offset
 */
const OpCode OpBranch = 0x41, OpBranchBackward = 0x42;
/**
 * Jumps to the right specified address label according to the value of a X register
 * @param XRegister the condition value
 * @param FalseOffset the jump to do if the condition is false
 * @param TrueOffset the jump to do if the condition is true
 * @param ErrorOffset the jump to do if there is an error
 */
const OpCode OpCondBranch = 0x43, OpCondBranchFB = 0x44, OpCondBranchBF = 0x45, OpCondBranchBB = 0x46;

/**
 * Apply a deep pattern matching on the provided test value against the provided patterns
 * @param Register a register of type X, Y or G containing the value to test
 * @param KRegister the patterns
 * @remark Patterns must have the following form '(pat1#off1)#....#(patN#offN)'
 */
const OpCode OpPatternMatchX = 0x47, OpPatternMatchY = 0x48, OpPatternMatchG = 0x49;

/**
 * Unify the two provided operands
 * @param XRegister left operand
 * @param XRegister right operand
 */
const OpCode OpUnifyXX = 0x50;
/**
 * Unify the two provided operands
 * @param XRegister left operand
 * @param YRegister right operand
 */
const OpCode OpUnifyXY = 0x51;
/**
 * Unify the two provided operands
 * @param XRegister left operand
 * @param GRegister right operand
 */
const OpCode OpUnifyXG = 0x52;
/**
 * Unify the two provided operands
 * @param XRegister left operand
 * @param KRegister right operand
 */
const OpCode OpUnifyXK = 0x53;
/**
 * Unify the two provided operands
 * @param YRegister left operand
 * @param YRegister right operand
 */
const OpCode OpUnifyYY = 0x54;
/**
 * Unify the two provided operands
 * @param YRegister left operand
 * @param GRegister right operand
 */
const OpCode OpUnifyYG = 0x55;
/**
 * Unify the two provided operands
 * @param YRegister left operand
 * @param KRegister right operand
 */
const OpCode OpUnifyYK = 0x56;
/**
 * Unify the two provided operands
 * @param GRegister left operand
 * @param GRegister right operand
 */
const OpCode OpUnifyGG = 0x57;
/**
 * Unify the two provided operands
 * @param GRegister left operand
 * @param KRegister right operand
 */
const OpCode OpUnifyGK = 0x58;
/**
 * Unify the two provided operands
 * @param KRegister left operand
 * @param KRegister right operand
 */
const OpCode OpUnifyKK = 0x59;

const OpCode OpCreateStructBase = 0x60;
static_assert((OpCreateStructBase & 0x1F) == 0,
              "OpCreateStructBase must be aligned on 0x20");

const OpCode OpCreateStructWhatMask = 0x03;
const OpCode OpCreateStructAbstraction = 0;
const OpCode OpCreateStructCons = 1;
const OpCode OpCreateStructTuple = 2;
const OpCode OpCreateStructRecord = 3;

const OpCode OpCreateStructWhereMask = 0x07 << 2;
const OpCode OpCreateStructStoreX = 0 << 2;
const OpCode OpCreateStructStoreY = 1 << 2;
const OpCode OpCreateStructUnifyX = 2 << 2;
const OpCode OpCreateStructUnifyY = 3 << 2;
const OpCode OpCreateStructUnifyG = 4 << 2;
const OpCode OpCreateStructUnifyK = 5 << 2;

const OpCode OpCreateAbstractionStoreX =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructStoreX;
const OpCode OpCreateConsStoreX =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructStoreX;
const OpCode OpCreateTupleStoreX =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructStoreX;
const OpCode OpCreateRecordStoreX =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructStoreX;

const OpCode OpCreateAbstractionStoreY =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructStoreY;
const OpCode OpCreateConsStoreY =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructStoreY;
const OpCode OpCreateTupleStoreY =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructStoreY;
const OpCode OpCreateRecordStoreY =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructStoreY;

const OpCode OpCreateAbstractionUnifyX =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructUnifyX;
const OpCode OpCreateConsUnifyX =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructUnifyX;
const OpCode OpCreateTupleUnifyX =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructUnifyX;
const OpCode OpCreateRecordUnifyX =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructUnifyX;

const OpCode OpCreateAbstractionUnifyY =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructUnifyY;
const OpCode OpCreateConsUnifyY =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructUnifyY;
const OpCode OpCreateTupleUnifyY =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructUnifyY;
const OpCode OpCreateRecordUnifyY =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructUnifyY;

const OpCode OpCreateAbstractionUnifyG =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructUnifyG;
const OpCode OpCreateConsUnifyG =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructUnifyG;
const OpCode OpCreateTupleUnifyG =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructUnifyG;
const OpCode OpCreateRecordUnifyG =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructUnifyG;

const OpCode OpCreateAbstractionUnifyK =
  OpCreateStructBase | OpCreateStructAbstraction | OpCreateStructUnifyK;
const OpCode OpCreateConsUnifyK =
  OpCreateStructBase | OpCreateStructCons | OpCreateStructUnifyK;
const OpCode OpCreateTupleUnifyK =
  OpCreateStructBase | OpCreateStructTuple | OpCreateStructUnifyK;
const OpCode OpCreateRecordUnifyK =
  OpCreateStructBase | OpCreateStructRecord | OpCreateStructUnifyK;

const OpCode SubOpArrayFillX = 0;
const OpCode SubOpArrayFillY = 1;
const OpCode SubOpArrayFillG = 2;
const OpCode SubOpArrayFillK = 3;
const OpCode SubOpArrayFillNewVarX = 4;
const OpCode SubOpArrayFillNewVarY = 5;
const OpCode SubOpArrayFillNewVars = 6;

// Inlines for some builtins
const OpCode OpInlineEqualsInteger = 0x80;
const OpCode OpInlineAdd = 0x81;
const OpCode OpInlineSubtract = 0x82;
const OpCode OpInlinePlus1 = 0x83;
const OpCode OpInlineMinus1 = 0x84;

const OpCode OpInlineGetClass = 0x90;

const OpCode OpDebugEntry = 0xa0;
const OpCode OpDebugExit = 0xa1;
const OpCode OpLocalVarname = 0xa2;
const OpCode OpGlobalVarname = 0xa3;
const OpCode OpClearY = 0xa4;

}

#endif // MOZART_OPCODES_H
