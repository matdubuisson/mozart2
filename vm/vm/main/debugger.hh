#ifndef MOZART_DEBUGGER_H
#define MOZART_DEBUGGER_H

#include <iostream>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "opcodes.hh"
#include "mozart.hh"

using namespace std;
using namespace mozart;

#define DEBUGGER_PIPE_INITIAL_BACKOFF_TIME 1
#define DEBUGGER_PIPE_MAXIMAL_BACKOFF_TIME 64

class DebuggerPipe {
    private:
        atomic<unsigned long int> buffer_size;
        unsigned long int max_buffer_size;
        queue<OpCode> buffer;
        mutex buffer_mutex;
    public:
        DebuggerPipe(bool initial_signal, unsigned long int buffer_size) {
            this->buffer_size = 0;
            this->buffer_size.store(0);
            this->max_buffer_size = buffer_size;
        }

        bool hasNext(void) {
            return this->buffer_size.load() > 0;
        }

        unsigned long int size(void) {
            return this->buffer_size.load();
        }

        OpCode getNext(void);
        void wait(OpCode opCode);
};

class DebuggerAgent {
    private:
        atomic<bool> flag;
        DebuggerPipe *pipe;
        thread agent;

    public:
        DebuggerAgent(DebuggerPipe *pipe) {
            this->flag.store(true);
            this->pipe = pipe;
        }

        ~DebuggerAgent() {
            this->stop();
        }

        void start(void);
        void stop(void);
};

#endif