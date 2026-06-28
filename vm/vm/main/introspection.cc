// Copyright © 2014, Université catholique de Louvain
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

#include <iostream>
#include "mozart.hh"

namespace mozart {

Introspection::OperationArgument getData(
  ProgramCounter PC,
  XRegArray* const xregs,
  StaticArray<UnstableNode>& yregs,
  StaticArray<StableNode>& gregs,
  StaticArray<StableNode>& kregs,
  Introspection::ArgumentType type, size_t index) {

#define IntPC(offset) PC[offset]
#define XPC(offset) (*xregs)[PC[offset]]
#define YPC(offset) (yregs)[PC[offset]]
#define GPC(offset) (gregs)[PC[offset]]
#define KPC(offset) (kregs)[PC[offset]]

  using AT = Introspection::ArgumentType;

  switch (type) {
    case AT::I: return {type, index, IntPC(index)};
    case AT::X: return {type, index, XPC(index)};
    case AT::Y: return {type, index, YPC(index)};
    case AT::G: return {type, index, GPC(index)};
    case AT::K: return {type, index, KPC(index)};
    default: assert(false);
  }

#undef IntPC
#undef XPC
#undef YPC
#undef GPC
#undef KPC

}

Introspection::Operation Introspection::getNextExecutedOperation(VM vm, bool includeSystemThreads) {
  Runnable* runnable = vm->threadPool.getNext(includeSystemThreads);
  assert(runnable != nullptr);

  Operation operation(OpSkip);

  if (Thread* thread = dynamic_cast<Thread*>(vm, runnable)) {
    StackEntry& entry = thread->stack.front();
    assert(!entry.isExceptionHandler());

    XRegArray* const xregs = &thread->xregs;

    StableNode* abstraction = entry.abstraction;
    ProgramCounter PC = entry.PC;
    size_t yregCount = entry.yregCount;
    StaticArray<UnstableNode> yregs = entry.yregs;
    StaticArray<StableNode> gregs = entry.gregs;
    StaticArray<StableNode> kregs = entry.kregs;

#define IntPC(offset) PC[offset]
#define XPC(offset) (*xregs)[PC[offset]]
#define YPC(offset) (yregs)[PC[offset]]
#define GPC(offset) (gregs)[PC[offset]]
#define KPC(offset) (kregs)[PC[offset]]

    OpCode op = *PC;

    operation.opCode = op;
#define NAME(string) operation.name = string
#define ARG(type, index) operation.arguments.push_back(getData(PC, xregs, yregs, gregs, kregs, type, index))

    switch (op) {
      // SKIP

      case OpSkip:
        NAME("Skip");
        break;

      // DEBUG

      case OpDebugEntry: {
        NAME("DebugEntry");
        ARG(K, 1);
        ARG(I, 2);
        ARG(I, 3);
        ARG(K, 4);
        break;
      }

      case OpDebugExit: {
        NAME("DebugExit");
        ARG(K, 1);
        ARG(I, 2);
        ARG(I, 3);
        ARG(K, 4);
        break;
      }

      case OpLocalVarname:
        NAME("LocalVanname");
        break;

      case OpGlobalVarname:
        NAME("GlobalVarname");
        break;

      case OpClearY:
        NAME("ClearY");
        break;

      // MOVES

      case OpMoveXX:
        NAME("MoveXX");
        ARG(X, 1);
        ARG(X, 2);
        break;

      case OpMoveXY:
        NAME("MoveXY");
        ARG(X, 1);
        ARG(Y, 2);
        break;

      case OpMoveYX:
        NAME("MoveYX");
        ARG(Y, 1);
        ARG(X, 2);
        break;

      case OpMoveYY:
        NAME("MoveYY");
        ARG(Y, 1);
        ARG(Y, 2);
        break;

      case OpMoveGX:
        NAME("MoveGX");
        ARG(G, 1);
        ARG(X, 2);
        break;

      case OpMoveGY:
        NAME("MoveGY");
        ARG(G, 1);
        ARG(Y, 2);
        break;

      case OpMoveKX:
        NAME("MoveKX");
        ARG(K, 1);
        ARG(X, 2);
        break;

      case OpMoveKY:
        NAME("MoveKY");
        ARG(K, 1);
        ARG(Y, 2);
        break;

      // Double moves

      case OpMoveMoveXYXY:
        NAME("MoveMoveXYXY");
        ARG(X, 1);
        ARG(Y, 2);
        ARG(X, 3);
        ARG(Y, 4);
        break;

      case OpMoveMoveYXYX:
        NAME("MoveMoveYXYX");
        ARG(Y, 1);
        ARG(X, 2);
        ARG(Y, 3);
        ARG(X, 4);
        break;

      case OpMoveMoveYXXY:
        NAME("MoveMoveYXXY");
        ARG(Y, 1);
        ARG(X, 2);
        ARG(X, 3);
        ARG(Y, 4);
        break;

      case OpMoveMoveXYYX:
        NAME("MoveMoveXYYX");
        ARG(X, 1);
        ARG(Y, 2);
        ARG(Y, 3);
        ARG(X, 4);
        break;

      // Y allocations

      case OpAllocateY: {
        NAME("AllocateY");
        ARG(I, 1);
        break;
      }

      // Variable allocation

      case OpCreateVarX: {
        NAME("CreateVarX");
        ARG(X, 1);
        break;
      }

      case OpCreateVarY: {
        NAME("CreateVarY");
        ARG(Y, 1);
        break;
      }

      case OpCreateVarMoveX: {
        NAME("CreateVarMoveX");
        ARG(X, 1);
        ARG(X, 2);
        break;
      }

      case OpCreateVarMoveY: {
        NAME("CreateVarMoveY");
        ARG(Y, 1);
        ARG(X, 2);
        break;
      }

      // Exception handlers

      case OpSetupExceptionHandler: {
        NAME("SetupExceptionHandler");
        ARG(I, 1);
        break;
      }

      case OpPopExceptionHandler: {
        NAME("PopExceptionHandler");
        break;
      }

      // Control

      case OpCallBuiltin0: {
        NAME("CallBuiltin0");
        ARG(K, 1);
        break;
      }

      case OpCallBuiltin1: {
        NAME("CallBuiltin1");
        ARG(K, 1);
        ARG(X, 2);
        break;
      }

      case OpCallBuiltin2: {
        NAME("CallBuiltin2");
        ARG(K, 1);
        ARG(X, 2);
        ARG(X, 3);
        break;
      }

      case OpCallBuiltin3: {
        NAME("CallBuiltin3");
        ARG(K, 1);
        ARG(X, 2);
        ARG(X, 3);
        ARG(X, 4);
        break;
      }

      case OpCallBuiltin4: {
        NAME("CallBuiltin4");
        ARG(K, 1);
        ARG(X, 2);
        ARG(X, 3);
        ARG(X, 4);
        ARG(X, 5);
        break;
      }

      case OpCallBuiltin5: {
        NAME("CallBuiltin5");
        ARG(K, 1);
        ARG(X, 2);
        ARG(X, 3);
        ARG(X, 4);
        ARG(X, 5);
        ARG(X, 6);
        break;
      }

      case OpCallBuiltinN: {
        NAME("CallBuiltinN");
        ARG(K, 1);
        ARG(I, 2);

        size_t argc = IntPC(2);

        UnstableNode* args[argc];
        for (size_t i = 0; i < argc; i++)
          ARG(X, 3 + i);

        break;
      }

      case OpCallX: {
        NAME("CallX");
        ARG(X, 1);
        ARG(I, 2);
        break;
      }

      case OpCallY: {
        NAME("CallY");
        ARG(Y, 1);
        ARG(I, 2);
        break;
      }

      case OpCallG: {
        NAME("CallG");
        ARG(G, 1);
        ARG(I, 2);
        break;
      }

      case OpCallK: {
        NAME("CallK");
        ARG(K, 1);
        ARG(I, 2);
        break;
      }

      case OpTailCallX: {
        NAME("TailCallX");
        ARG(X, 1);
        ARG(I, 2);
        break;
      }

      case OpTailCallY: {
        NAME("TailCallY");
        ARG(Y, 1);
        ARG(I, 2);
        break;
      }

      case OpTailCallG: {
        NAME("TailCallG");
        ARG(G, 1);
        ARG(I, 2);
        break;
      }

      case OpTailCallK: {
        NAME("TailCallK");
        ARG(K, 1);
        ARG(I, 2);
        break;
      }

      case OpSendMsgX: {
        NAME("SendMsgX");
        ARG(X, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpSendMsgY: {
        NAME("SendMsgY");
        ARG(Y, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpSendMsgG: {
        NAME("SendMsgG");
        ARG(G, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpSendMsgK: {
        NAME("SendMsgK");
        ARG(K, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpTailSendMsgX: {
        NAME("TailSendMsgX");
        ARG(X, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpTailSendMsgY: {
        NAME("TailSendMsgY");
        ARG(Y, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpTailSendMsgG: {
        NAME("TailSendMsgG");
        ARG(G, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpTailSendMsgK: {
        NAME("TailSendMsgK");
        ARG(K, 1);
        ARG(K, 2);
        ARG(I, 3);
        break;
      }

      case OpReturn: {
        NAME("Return");
        break;
      }

      case OpBranch: {
        NAME("Branch");
        ARG(I, 1);
        break;
      }

      case OpBranchBackward: {
        NAME("BranchBackward");
        ARG(I, 1);
        break;
      }

      case OpCondBranch: {
        NAME("CondBranch");
        ARG(X, 1);
        ARG(I, 2);
        ARG(I, 3);
        break;
      }

      case OpCondBranchFB: {
        NAME("CondBranchFB");
        ARG(X, 1);
        ARG(I, 2);
        ARG(I, 3);
        break;
      }

      case OpCondBranchBF: {
        NAME("CondBranchBF");
        ARG(X, 1);
        ARG(I, 2);
        ARG(I, 3);
        break;
      }

      case OpCondBranchBB: {
        NAME("CondBranchBB");
        ARG(X, 1);
        ARG(I, 2);
        ARG(I, 3);
        break;
      }

      case OpPatternMatchX: {
        NAME("PatternMatchX");
        ARG(X, 1);
        ARG(K, 2);
        break;
      }

      case OpPatternMatchY: {
        NAME("PatternMatchY");
        ARG(Y, 1);
        ARG(K, 2);
        break;
      }

      case OpPatternMatchG: {
        NAME("PatternMatchG");
        ARG(G, 1);
        ARG(K, 2);
        break;
      }

      // Unification

      case OpUnifyXX: {
        NAME("UnifyXX");
        ARG(X, 1);
        ARG(X, 2);
        break;
      }

      case OpUnifyXY: {
        NAME("UnifyXY");
        ARG(X, 1);
        ARG(Y, 2);
        break;
      }

      case OpUnifyXG: {
        NAME("UnifyXG");
        ARG(X, 1);
        ARG(G, 2);
        break;
      }

      case OpUnifyXK: {
        NAME("UnifyXK");
        ARG(X, 1);
        ARG(K, 2);
        break;
      }

      case OpUnifyYY: {
        NAME("UnifyYY");
        ARG(Y, 1);
        ARG(Y, 2);
        break;
      }

      case OpUnifyYG: {
        NAME("UnifyYG");
        ARG(Y, 1);
        ARG(G, 2);
        break;
      }

      case OpUnifyYK: {
        NAME("UnifyYK");
        ARG(Y, 1);
        ARG(K, 2);
        break;
      }

      case OpUnifyGG: {
        NAME("UnifyGG");
        ARG(G, 1);
        ARG(G, 2);
        break;
      }

      case OpUnifyGK: {
        NAME("UnifyGK");
        ARG(G, 1);
        ARG(K, 2);
        break;
      }

      case OpUnifyKK: {
        NAME("UnifyKK");
        ARG(K, 1);
        ARG(K, 2);
        break;
      }

      // Creation of data structures

      case OpCreateAbstractionStoreX: NAME("CreateAbstractionStoreX"); goto OpCreate;
      case OpCreateConsStoreX: NAME("CreateConsStoreX"); goto OpCreate;
      case OpCreateTupleStoreX: NAME("CreateTupleStoreX"); goto OpCreate;
      case OpCreateRecordStoreX: NAME("CreateRecordStoreX"); goto OpCreate;

      case OpCreateAbstractionStoreY: NAME("CreateAbstractionStoreY"); goto OpCreate;
      case OpCreateConsStoreY: NAME("CreateConsStoreY"); goto OpCreate;
      case OpCreateTupleStoreY: NAME("CreateTupleStoreY"); goto OpCreate;
      case OpCreateRecordStoreY: NAME("CreateRecordStoreY"); goto OpCreate;

      case OpCreateAbstractionUnifyX: NAME("CreateAbstractionUnifyX"); goto OpCreate;
      case OpCreateConsUnifyX: NAME("CreateConsUnifyX"); goto OpCreate;
      case OpCreateTupleUnifyX: NAME("CreateTupleUnifyX"); goto OpCreate;
      case OpCreateRecordUnifyX: NAME("CreateRecordUnifyX"); goto OpCreate;

      case OpCreateAbstractionUnifyY: NAME("CreateAbstractionUnifyY"); goto OpCreate;
      case OpCreateConsUnifyY: NAME("CreateConsUnifyY"); goto OpCreate;
      case OpCreateTupleUnifyY: NAME("CreateTupleUnifyY"); goto OpCreate;
      case OpCreateRecordUnifyY: NAME("CreateRecordUnifyY"); goto OpCreate;

      case OpCreateAbstractionUnifyG: NAME("CreateAbstractionUnifyG"); goto OpCreate;
      case OpCreateConsUnifyG: NAME("CreateConsUnifyG"); goto OpCreate;
      case OpCreateTupleUnifyG: NAME("CreateTupleUnifyG"); goto OpCreate;
      case OpCreateRecordUnifyG: NAME("CreateRecordUnifyG"); goto OpCreate;

      case OpCreateAbstractionUnifyK: NAME("CreateAbstractionUnifyK"); goto OpCreate;
      case OpCreateConsUnifyK: NAME("CreateConsUnifyK"); goto OpCreate;
      case OpCreateTupleUnifyK: NAME("CreateTupleUnifyK"); goto OpCreate;
      case OpCreateRecordUnifyK: NAME("CreateRecordUnifyK"); goto OpCreate;

      OpCreate: {
        auto what = op & OpCreateStructWhatMask;
        auto where = op & OpCreateStructWhereMask;

        bool isStoreMode;
        UnstableNode* writeDest = nullptr;
        RichNode readDest = nullptr;

        switch (where) {
          case OpCreateStructStoreX: {
            isStoreMode = true;
            writeDest = &XPC(3);
            ARG(X, 3);
            break;
          }

          case OpCreateStructStoreY: {
            isStoreMode = true;
            writeDest = &YPC(3);
            ARG(Y, 3);
            break;
          }

          case OpCreateStructUnifyX: {
            isStoreMode = false;
            readDest = XPC(3);
            ARG(X, 3);
            break;
          }

          case OpCreateStructUnifyY: {
            isStoreMode = false;
            readDest = YPC(3);
            ARG(Y, 3);
            break;
          }

          case OpCreateStructUnifyG: {
            isStoreMode = false;
            readDest = GPC(3);
            ARG(G, 3);
            break;
          }

          case OpCreateStructUnifyK: {
            isStoreMode = false;
            readDest = KPC(3);
            ARG(K, 3);
            break;
          }

          default: {
            assert(false);
          }
        } // switch (where)

        StaticArray<StableNode> array;
        size_t length = IntPC(2);
        ARG(I, 2);

        if (isStoreMode) {
          for (size_t index = 0; index < length; index++) {
            auto subOpCode = *PC;

            switch (subOpCode) {
              case SubOpArrayFillX: {
                ARG(X, 1);
                break;
              }
              case SubOpArrayFillY: {
                ARG(Y, 1);
                break;
              }
              case SubOpArrayFillG: {
                ARG(G, 1);
                break;
              }
              case SubOpArrayFillK: {
                ARG(K, 1);
                break;
              }

              case SubOpArrayFillNewVarX: {
                ARG(X, 1);
                break;
              }
              case SubOpArrayFillNewVarY: {
                ARG(Y, 1);
                break;
              }

              case SubOpArrayFillNewVars: {
                ARG(I, 1);
                break;
              }

              default: {
                assert(false);
              }
            }
          }
        } else {
          for (size_t index = 0; index < length; index++) {
            auto subOpCode = *PC;

            switch (subOpCode) {
              case SubOpArrayFillX: {
                ARG(X, 1);
                break;
              }
              case SubOpArrayFillY: {
                ARG(Y, 1);
                break;
              }
              case SubOpArrayFillG: {
                ARG(G, 1);
                break;
              }
              case SubOpArrayFillK: {
                ARG(K, 1);
                break;
              }

              case SubOpArrayFillNewVarX: {
                ARG(X, 1);
                break;
              }
              case SubOpArrayFillNewVarY: {
                ARG(Y, 1);
                break;
              }

              case SubOpArrayFillNewVars: {
                ARG(I, 1);
                break;
              }

              default: {
                assert(false);
              }
            }
          }
        }

        break;
      }

      // Inlines for some builtins

      case OpInlineEqualsInteger: {
        NAME("InlineEqualsInteger");
        ARG(X, 1);
        ARG(I, 2);
        ARG(I, 3);
        break;
      }

      case OpInlinePlus1: {
        NAME("InlinePlus1");
        ARG(X, 1);
        ARG(X, 2);
        break;
      }

      case OpInlineMinus1: {
        NAME("InlineMinus1");
        ARG(X, 1);
        ARG(X, 2);
        break;
      }

      case OpInlineAdd: {
        NAME("InlineAdd");
        ARG(X, 1);
        ARG(X, 2);
        ARG(X, 3);
        break;
      }

      case OpInlineSubtract: {
        NAME("InlineSubstract");
        ARG(X, 1);
        ARG(X, 2);
        ARG(X, 3);
        break;
      }

      case OpInlineGetClass: {
        NAME("InlineGetClass");
        ARG(X, 1);
        ARG(X, 2);
        break;
      }

      default: {
        assert(false);
      }
    }
  }

#undef REPR
#undef ARG

#undef IntPC
#undef XPC
#undef YPC
#undef GPC
#undef KPC

  return operation;
}

}