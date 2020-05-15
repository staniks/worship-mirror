#pragma once

#include "mau/base/engine_context.hpp"
#include "mau/base/types.hpp"
#include "mau/io/resource_cache.hpp"
#include "mau/rendering/renderer.hpp"

namespace mau {

// Forward declaration.
class engine_context_t;

// Chainable screen-space effect. Applied to render targets.
class effect_t : non_movable_t, non_copyable_t
{
public:
    effect_t(engine_context_t& engine);
    virtual ~effect_t() = default;

    // Apply the effect to the render target and return a render target with effect applied. Note: render target creation is
    // expensive and should not be done on the fly, cache and reuse when using this function.
    virtual render_target_handle_t apply(render_target_handle_t input) = 0;

protected:
    engine_context_t& engine_m;
    renderer_t&       renderer_m;
};
} // namespace mau
