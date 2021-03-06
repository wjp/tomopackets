#pragma once

#include <cstring>
#include <memory>

namespace tomop {

struct scale {
    std::size_t size = 0;

    template <typename T>
    void operator|(T&) {
        size += sizeof(T);
    }

    void operator|(std::string& str) {
        size += (str.size() + 1) * sizeof(char);
    }

    template <typename T>
    void operator|(std::vector<T>& xs) {
        size += sizeof(int) + xs.size() * sizeof(T);
    }
};

struct memory_buffer {
    memory_buffer(std::size_t size) { buffer = std::make_unique<char[]>(size); }

    memory_buffer(std::size_t size, char* data) {
        buffer = std::make_unique<char[]>(size);
        // double buffer is kind of a waste
        memcpy(buffer.get(), data, size);
    }

    std::unique_ptr<char[]> buffer;
    std::size_t index = 0;

    template <typename T>
    void operator<<(const T& value) {
        memcpy(buffer.get() + index, &value, sizeof(T));
        index += sizeof(T);
    }

    template <typename T>
    void operator>>(T& value) {
        memcpy(&value, buffer.get() + index, sizeof(T));
        index += sizeof(T);
    }

    void operator<<(std::string& str) {
        for (auto c : str) (*this) << c;
        (*this) << '\0';
    }

    void operator>>(std::string& str) {
        str = std::string(buffer.get() + index);
        index += (str.size() + 1) * sizeof(char);
    }

    template <typename T>
    void operator<<(std::vector<T>& xs) {
        (*this) << (int)xs.size();
        memcpy(buffer.get() + index, xs.data(), sizeof(T) * xs.size());
        index += xs.size() * sizeof(T);
    }

    template <typename T>
    void operator>>(std::vector<T>& xs) {
        int size = 0;
        (*this) >> size;
        xs.resize(size);
        memcpy(xs.data(), buffer.get() + index, sizeof(T) * size);
        index += size * sizeof(T);
    }
};

}  // namespace tomop
