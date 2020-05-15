#pragma once

#include "mau/rendering/effect.hpp"

#include <array>

namespace mau {

class effect_bloom_t : public effect_t
{
public:
    effect_bloom_t(engine_context_t& engine);
    ~effect_bloom_t() override;

    render_target_handle_t apply(render_target_handle_t pInput) override;

private:
    // Gaussian blur shader.
    shader_handle_t blur_shader_m;

    // Blending shader.
    shader_handle_t bloom_shader_m;

    // Ping-pong buffers. Since we're doing two passes, and we can't read from and write to the same buffer, we use two
    // alternating buffers.
    std::array<render_target_handle_t, 2> blur_render_target_m;
    render_target_handle_t                result_render_target_m;
};

} // namespace mau
