#pragma once
#include "StdTypes.h"

namespace core {

    struct Packet {

    public:
        Time timestamp;
        size_t len;
        uint8_t* data;
    };


}