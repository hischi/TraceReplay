#pragma once
#include <atomic>

namespace core {

    class Thread {

    public:
        Thread();

        void CreateAndStart();
        void Stop();
        void WaitAndTerminate();

        virtual void Task();
        bool Run();

    private:
        std::atomic_bool started;

        void* handle;
    };
}