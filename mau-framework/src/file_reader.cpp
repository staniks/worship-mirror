#include "mau/io/file_reader.hpp"

#include <fmt/format.h>

namespace mau {

file_reader_t::file_reader_t(file_handle_t file) : file_m(file)
{
}
void file_reader_t::read(byte_t* destination, size_t size)
{
    if(file_cursor_m + size > file_m->size())
        throw exception_t{"unable to read past the end of file"};

    std::copy(file_m->data() + file_cursor_m, file_m->data() + file_cursor_m + size, destination);

    file_cursor_m += size;
}

void file_reader_t::read_string(std::string& destination)
{
    uint16_t length;
    read(&length);

    if(length > max_deserialized_string_length_k)
        throw exception_t{fmt::format("string length exceeds maximum allowed length: {}", file_m->name())};

    destination.clear();
    destination.resize(length);

    read(reinterpret_cast<byte_t*>(destination.data()), sizeof(std::string::value_type) * length);
}

} // namespace mau
