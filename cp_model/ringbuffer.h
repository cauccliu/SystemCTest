#pragma once

#include <iostream>
#include <vector>

class RingBuffer {
private:
    std::vector<uint64_t> buffer;
    int ringbuffer_size;
    int ringbuffer_head;
    int ringbuffer_tail;

public:
    RingBuffer(int size) : ringbuffer_size(size), ringbuffer_head(0), ringbuffer_tail(0) {
        buffer.resize(size);
    }

    void add(uint64_t value) {
        buffer[ringbuffer_tail] = value;
        ringbuffer_tail = (ringbuffer_tail + 1) % ringbuffer_size;
        if (ringbuffer_tail == ringbuffer_head) {
            ringbuffer_head = (ringbuffer_head + 1) % ringbuffer_size;
        }
    }

    uint32_t get_count(int index) {
        if (ringbuffer_head == ringbuffer_tail) {
            std::cerr << "Buffer is empty." << std::endl;
            return 0;
        }
        uint32_t count = static_cast<uint32_t>(buffer[(ringbuffer_head + index) % ringbuffer_size] & 0xFFFFFFFF);
        return count;
    }

    uint64_t get_addr(int index) {
        if (ringbuffer_head == ringbuffer_tail) {
            std::cerr << "Buffer is empty." << std::endl;
            return 0;
        }
        uint64_t addr = buffer[(ringbuffer_head + index) % ringbuffer_size] >> 32;
        return addr;
    }
};

class IndirectBuffer {
private:
    uint32_t* data;
    uint32_t* ptr;
    size_t depth;

public:
    IndirectBuffer(size_t depth) : depth(depth) {
        data = new uint32_t[depth];
        ptr = data;
    }

    ~IndirectBuffer() {
        delete[] data;
    }

    uint32_t* getPtr() {
        return ptr;
    }

    uint32_t& operator[](size_t offset) {
        return data[offset];
    }
};
