#include "mau/base/state.hpp"

#include "mau/base/engine_context.hpp"

#include <fmt/format.h>

namespace mau {

state_t::state_t(engine_context_t& engine, std::string_view name) : engine_m(engine), name_m(name)
{
    auto message = fmt::format("initializing state: {}", name_m);
    engine_m.log().log(message);
}

state_t::~state_t()
{
    auto message = fmt::format("destroyed state: {}", name_m);
    engine_m.log().log(message);
}

std::string_view state_t::name()
{
    return name_m;
}

} // namespace mau
