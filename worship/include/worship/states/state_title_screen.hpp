#pragma once

#include "worship/menu/menu.hpp"

#include <mau/base/state.hpp>
#include <mau/base/timer.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

#include <functional>

namespace mau {

class state_title_screen_t : public state_t
{
public:
    explicit state_title_screen_t(engine_context_t& engine);

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time);

private:
    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    font_handle_t          font_small_m;
    font_handle_t          font_medium_m;
    texture_handle_t       background_m;

    bool                   press_any_key_visible_m{false};
    float                  press_any_key_timer_m{0.0f};
    static constexpr float press_any_key_frequency_m{2.0f};
};

} // namespace mau
