#include "mau/rendering/effect.hpp"

namespace mau {

effect_t::effect_t(engine_context_t& engine) : engine_m(engine), renderer_m(engine.renderer())
{
}

} // namespace mau
