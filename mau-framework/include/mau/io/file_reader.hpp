#pragma once

#include "mau/io/file.hpp"

namespace mau {

inline static constexpr uint64_t max_deserialized_string_length_k = 1024;

// File reader utility.
class file_reader_t : non_movable_t, non_copyable_t
{
public:
    explicit file_reader_t(file_handle_t file);

    // Read a number of bytes specified by size into the destination array.
    void read(byte_t* destination, size_t size);

    // Read a struct.
    template<typename T>
    void read(T* destination)
    {
        read(reinterpret_cast<byte_t*>(destination), sizeof(T));
    }

    // Read a string from the file. Strings consist of count followed by character array of that count.
    void read_string(std::string& destination);

private:
    file_handle_t file_m;
    size_t        file_cursor_m{0};
};

} // namespace mau
