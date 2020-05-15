#include "mau/io/file.hpp"

namespace mau {

file_t::file_t(std::string_view name, byte_t* data, size_t size) : name_m(name), data_m(data), size_m(size)
{
}
file_t::~file_t()
{
    delete[] data_m;
}
std::string_view file_t::name()
{
    return name_m;
}
byte_t* file_t::data()
{
    return data_m;
}
size_t file_t::size() const
{
    return size_m;
}

} // namespace mau
