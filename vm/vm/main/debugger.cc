#include <iostream>

#include "mozart.hh"
#include "coremodules.hh"
#include "debugger.hh"

OpCode DebuggerPipe::getNext(void) {
    if (!this->hasNext()) return UINT16_MAX;
    OpCode opCode = UINT16_MAX;
    this->buffer_mutex.lock();
    try {
        this->buffer_size.store(this->buffer_size.load() - 1);
        opCode = this->buffer.front();
        this->buffer.pop();
    } catch (...) {}
    this->buffer_mutex.unlock();
    return opCode;
}

void DebuggerPipe::wait(OpCode opCode) {
    unsigned int backoff_time = DEBUGGER_PIPE_INITIAL_BACKOFF_TIME;
    while (this->buffer_size.load() == this->max_buffer_size) {
        this_thread::sleep_for(chrono::milliseconds(backoff_time));
        if (backoff_time < DEBUGGER_PIPE_MAXIMAL_BACKOFF_TIME) {
            backoff_time <<= 1;
        }
    }

    this->buffer_mutex.lock();
    try {
        this->buffer_size.store(this->buffer_size.load() + 1);
        this->buffer.push(opCode);
    } catch (...) {}
    this->buffer_mutex.unlock();
}

void DebuggerAgent::start(void) {
    this->agent = thread([this] {
        unsigned long int backoff_time = DEBUGGER_PIPE_INITIAL_BACKOFF_TIME;
        OpCode opCode;
        while (this->flag.load()) {
            if (!this->pipe->hasNext()) {
                this_thread::sleep_for(chrono::milliseconds(backoff_time));
                if (backoff_time < DEBUGGER_PIPE_MAXIMAL_BACKOFF_TIME)
                    backoff_time <<= 1;
                continue;
            }

            opCode = this->pipe->getNext();
            if (opCode == UINT16_MAX) continue;
            else backoff_time = DEBUGGER_PIPE_INITIAL_BACKOFF_TIME;

            cout << "Buffer size: " << this->pipe->size() << "\n";
            switch (opCode) {
                case OpSkip: {
                    cout << "SKIP INSTRUCTION DONE" << "\n";
                    break;
                } default: {
                    cout << "Other opCode: " << opCode << "\n";
                }
            }
            cout << "HELP ME" << endl;
        }

        cout << "GOOD BYE" << endl;
    });
}

void DebuggerAgent::stop(void) {
    if (this->flag.load()) this->flag.store(false);
    if (this->agent.joinable()) this->agent.join();
    cout << "DEBUGGER STOPPED" << endl;
}