#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace mau {

// Base class with copy constructor and assignment disabled. Polymorphic deletion via pointer to this class is forbidden -
// virtual destructor is omitted for performance reasons.
class non_copyable_t
{
public:
    non_copyable_t()                      = default;
    non_copyable_t(const non_copyable_t&) = delete;
    non_copyable_t& operator=(const non_copyable_t&) = delete;
};

// Base class with move constructor and assignment disabled. Polymorphic deletion via pointer to this class is forbidden -
// virtual destructor is omitted for performance reasons
class non_movable_t
{
public:
    non_movable_t()                = default;
    non_movable_t(non_movable_t&&) = delete;
    non_movable_t& operator=(non_movable_t&&) = delete;
};

using byte_t      = uint8_t;
using gl_handle_t = uint32_t;

template<typename T>
using range_t = std::pair<T, T>;

using exception_t = std::runtime_error;

} // namespace mau
