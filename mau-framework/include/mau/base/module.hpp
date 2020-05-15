#pragma once

#include "mau/base/types.hpp"

namespace mau {

// Forward declarations.
class engine_context_t;

class module_t : non_copyable_t, non_movable_t
{
public:
    module_t(engine_context_t& engine, std::string_view name);
    virtual ~module_t();

protected:
    engine_context_t& engine_m;
    std::string       name_m;
};

} // namespace mau
