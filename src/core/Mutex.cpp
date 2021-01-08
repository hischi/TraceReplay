#include "Mutex.h"

#include <Windows.h>
#include <stdexcept>
#include <cassert>

using namespace core;

Mutex::Mutex() {
    handle = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex
    if (handle == NULL)
        throw std::runtime_error("Can't create new mutex");
}

Mutex::~Mutex() {
    if (handle) {
        CloseHandle(handle);
    }
}

void Mutex::WaitAndEnter() {
    assert(handle);

    DWORD waitResult = WaitForSingleObject(
        handle,      // handle to mutex
        INFINITE);  // no time-out interval

    if (waitResult != WAIT_OBJECT_0)
        throw std::runtime_error("Can't obtain lock for mutex (mutex probably dead)");
}

void Mutex::Leave() {
    assert(handle);
    
    ReleaseMutex(handle);
}
