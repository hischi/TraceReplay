#pragma once
#include "StdTypes.h"
#include <cassert>

namespace core {

    class Packet {

    public:
        Packet(size_t maxLen = 0, size_t len = 0, Time timeStamp = Time(Time::clock::duration::zero())) :
            maxLen(maxLen), len(len), data(new uint8_t[maxLen]), timeStamp(timeStamp) {
            assert(maxLen >= len);
        }

        Packet(size_t maxLen, size_t len, const uint8_t* data, Time timeStamp = Time(Time::clock::duration::zero())) :
            maxLen(maxLen), len(len), data(new uint8_t[maxLen]), timeStamp(timeStamp) {
            assert(maxLen >= len);
            memcpy(this->data, data, len);
        }

        Packet(const Packet& other) : 
            maxLen(other.maxLen), len(other.len), data(new uint8_t[other.maxLen]), timeStamp(other.timeStamp) {
            assert(maxLen >= len);
            memcpy(this->data, other.data, len);
        }

        Packet(Packet&& other) noexcept :
            maxLen(other.maxLen), len(other.len), data(other.data), timeStamp(other.timeStamp) {
            assert(maxLen >= len);
            other.data = nullptr;
            other.maxLen = 0;
            other.len = 0;
        }

        virtual ~Packet() {
            if (data)
                delete[](data);
        }

        Packet& operator=(const Packet& other) {
            if (data)
                delete[](data);

            maxLen = other.maxLen;
            len = other.len;
            data = new uint8_t[maxLen];

            assert(maxLen >= len);
            memcpy(data, other.data, len);
            timeStamp = other.timeStamp;
        }

        Packet& operator=(Packet&& other) noexcept {
            maxLen = other.maxLen;
            len = other.len;
            data = other.data;
            other.data = nullptr;
            other.maxLen = 0;
            other.len = 0;
            timeStamp = other.timeStamp;
        }

        size_t GetMaxLen() const {
            return maxLen;
        }

        void Realloc(size_t maxLen, size_t frontOffset = 0) {
            assert(len + frontOffset <= maxLen);

            uint8_t* newData = new uint8_t[maxLen];
            
            if (data) {
                memcpy(&newData[frontOffset], data, len);
                delete[](data);
            }

            data = newData;
            len += frontOffset;
            this->maxLen = maxLen;
        }

        size_t GetLen() const {
            return len;
        }

        void SetLen(size_t len) {
            assert(len <= this->maxLen);
            this->len = len;
        }

        uint8_t* GetData() {
            return data;
        }

        void SetData(size_t len, const uint8_t* data) {
            if (len > maxLen) { // realloc necessary
                if (this->data)
                    delete[](this->data);

                this->maxLen = len;
                this->len = len;
                this->data = new uint8_t[maxLen];
            }
            
            memcpy(this->data, data, len);
        }

        void AppendFront(size_t len, const uint8_t* data) {
            if (this->len + len <= maxLen) {
                memcpy(&this->data[len], this->data, this->len);
                this->len += len;
            }
            else {
                // realloc necessary
                Realloc(this->len + len, len);
            }
            memcpy(this->data, data, len);
        }

        void AppendBack(size_t len, const uint8_t* data) {
            if (this->len + len > maxLen) {
                // realloc necessary
                Realloc(this->len + len, 0);
            }
            memcpy(&this->data[this->len], data, len);
            this->len += len;
        }

        Time GetTimeStamp() const {
            return timeStamp;
        }

        void SetTimeStamp(Time timeStamp) {
            this->timeStamp = timeStamp;
        }

    private:
        size_t maxLen;
        size_t len;
        uint8_t* data;
        Time timeStamp;
    };


}