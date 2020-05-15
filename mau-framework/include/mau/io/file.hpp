#pragma once

#include "mau/base/types.hpp"

#include <memory>

namespace mau {

// File container. Contains filename, data and size.
class file_t : non_copyable_t, non_movable_t
{
public:
    explicit file_t(std::string_view name, byte_t* data, size_t size);
    ~file_t();

    std::string_view name();
    byte_t*          data();
    uint64_t         size() const;

private:
    std::string name_m;
    byte_t*     data_m;
    size_t      size_m;
};

using file_handle_t = std::shared_ptr<file_t>;

} // namespace mau
