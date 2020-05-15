#include "mau/base/module.hpp"

#include "mau/base/engine_context.hpp"

#include <fmt/format.h>

namespace mau {

module_t::module_t(engine_context_t& engine, std::string_view name) : engine_m(engine), name_m(name)
{
    engine.log().log(fmt::format("initializing module: {}", name));
}
module_t::~module_t()
{
    engine_m.log().log(fmt::format("destroyed module: {}", name_m));
}

} // namespace mau
