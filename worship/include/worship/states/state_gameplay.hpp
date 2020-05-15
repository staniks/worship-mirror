#pragma once

#include "worship/effects/effect_bloom.hpp"
#include "worship/gameplay/entities/player.hpp"
#include "worship/gameplay/event_callback.hpp"
#include "worship/gameplay/messages.hpp"
#include "worship/gameplay/screen_flash.hpp"
#include "worship/gameplay/world/world.hpp"

#include <mau/base/state.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

namespace mau {
class state_gameplay_t : public state_t
{
public:
    explicit state_gameplay_t(engine_context_t& engine, difficulty_t difficulty);

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time) override;

    // Exposed through event_callback_t.
    void display_message(message_t message);
    void flash_screen(screen_flash_t screen_flash);

private:
    event_callback_t event_callback_m;

    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    effect_bloom_t         effect_bloom_m;

    font_handle_t font_small_m;
    font_handle_t font_medium_m;
    font_handle_t font_large_m;

    texture_handle_t gui_indicator_health_m;
    texture_handle_t gui_indicator_armor_m;
    texture_handle_t gui_screen_flash_texture_m;

    std::map<ammo_type_t, texture_handle_t> ammo_icons_m;

    static constexpr uint64_t game_message_duration_k = 5;
    message_t                 game_message_m{message_t::none_k};
    timer_t                   game_message_timer_m;

    static constexpr double screen_flash_duration_k = 0.5;
    static constexpr double screen_flash_alpha      = 0.25;
    screen_flash_t          screen_flash_m{screen_flash_t::none_k};
    timer_t                 screen_flash_timer_m;

    world_t   world_m;
    player_t* player_m;
};

} // namespace mau
