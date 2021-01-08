#include "Thread.h"
#include <stdexcept>
#include <Windows.h>

using namespace core;

DWORD WINAPI ThreadFunction(LPVOID lpParam);

Thread::Thread() {
    started = false;
    handle = nullptr;
}

void Thread::CreateAndStart() {
    started = true;

    handle = (void*)CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        ThreadFunction,         // thread function name
        this,                   // argument to thread function 
        0,                      // use default creation flags 
        0);                     // returns the thread identifier 

    if (handle == NULL)
        throw std::runtime_error("Can't create thread");
}

void Thread::Stop() {
    started = false;
}

void Thread::WaitAndTerminate() {
    if (handle) {
        WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);
        handle = nullptr;
    }
}

void Thread::Task() {

}

bool Thread::Run() {
    Task();

    return started;
}

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    if (lpParam == NULL) {
        return 1;
    }

    while (((Thread*)lpParam)->Run());

    return 0;
}