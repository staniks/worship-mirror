#pragma once

#include <mau/base/state.hpp>
#include <mau/base/timer.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

#include "worship/gameplay/entities/player.hpp"

namespace mau {

class state_loading_t : public state_t
{
public:
    explicit state_loading_t(engine_context_t& engine, difficulty_t difficulty);

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time);

private:
    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    font_handle_t          font_medium_m;
    texture_handle_t       background_m;
    difficulty_t           difficulty_m;

    uint64_t delay_counter_m{0};
};

} // namespace mau
