#pragma once

#include "mau/base/types.hpp"
#include "mau/io/file.hpp"

namespace mau {

// Forward declarations.
class engine_context_t;

// Base resource class.
class resource_t : non_copyable_t, non_movable_t
{
public:
    virtual ~resource_t() = default;
};

} // namespace mau
