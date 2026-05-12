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

#ifndef MOZART_INTROSPECTION_H
#define MOZART_INTROSPECTION_H

#include "mozartcore.hh"

namespace mozart {

/* ========== Virtual Machine stats ========== */



/* ========== Threads stats ========== */

typedef RunnableList::iterator iterator;

inline size_t Introspection::getActiveThreadsCount(VM vm) {
    size_t count = 0;
    RunnableList& aliveThreads = vm->aliveThreads;

    for (iterator iter = aliveThreads.begin();
         iter != aliveThreads.end(); iter++) {
        Runnable *runnable = *iter;
        if (runnable->isRunnable())
            count++;
    }

    return count;
}

inline size_t Introspection::getPassiveThreadsCount(VM vm) {
    size_t count = 0;
    RunnableList& aliveThreads = vm->aliveThreads;

    for (iterator iter = aliveThreads.begin();
         iter != aliveThreads.end(); iter++) {
        Runnable *runnable = *iter;
        // Dead or terminated runnable are immediately removed from the list
        if (!runnable->isRunnable())
            count++;
    }

    return count;
}

inline size_t Introspection::getTotalThreadsCount(VM vm) {
    return vm->aliveThreads.length;
}

/* ========== Variables stats ========== */

inline size_t Introspection::getBoundVariablesCount(VM vm) {
    return 0;
}

inline size_t Introspection::getUnBoundVariablesCount(VM vm) {
    return 0;
}

inline size_t Introspection::getTotalVariablesCount(VM vm) {
    return 0;
}

}

#endif // MOZART_INTROSPECTION_H
