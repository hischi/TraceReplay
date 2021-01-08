#pragma once

namespace core {
    class Mutex {

    public:
        Mutex();
        virtual ~Mutex();

        void WaitAndEnter();
        void Leave();

    private:
        void* handle;
    };
}