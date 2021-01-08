#pragma once
#include "Mutex.h"

namespace core {

    class MutexScope {
    public:
        MutexScope(Mutex* mutex) : mutex(mutex) {
            mutex->WaitAndEnter();
        }

        virtual ~MutexScope() {
            mutex->Leave();
        }

    private:
        Mutex* mutex;
    };
}