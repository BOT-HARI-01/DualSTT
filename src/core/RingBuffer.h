#pragma once
#include <vector>
#include <mutex>

class RingBuffer {
private:
    std::vector<float> buffer;
    size_t capacity;
    size_t writePos;
    size_t readPos;
    std::mutex mtx;

public:
    RingBuffer(size_t capacity)
        : buffer(capacity), capacity(capacity), writePos(0), readPos(0) {}

    void push(const std::vector<float> &samples) {
        std::lock_guard<std::mutex> lock(mtx);
        for (float sample : samples) {
            buffer[writePos] = sample;
            writePos = (writePos + 1) % capacity;
            if (writePos == readPos)
                readPos = (readPos + 1) % capacity;
        }
    }

    size_t pop(float *out, size_t max_samples) {
        std::lock_guard<std::mutex> lock(mtx);
        size_t count = 0;
        while (readPos != writePos && count < max_samples) {
            out[count++] = buffer[readPos];
            readPos = (readPos + 1) % capacity;
        }
        return count;
    }

    size_t available() {
        std::lock_guard<std::mutex> lock(mtx);
        if (writePos >= readPos) return writePos - readPos;
        return capacity - (readPos - writePos);
    }
};