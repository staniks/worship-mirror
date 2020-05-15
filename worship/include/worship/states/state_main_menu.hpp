#pragma once

#include "worship/menu/menu.hpp"

#include <mau/base/state.hpp>
#include <mau/base/timer.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

#include <functional>

namespace mau {

class state_main_menu_t : public state_t
{
public:
    enum class submenu_t
    {
        main_k,
        options_k,
        changes_require_restart_k,
        new_game_k
    };

    explicit state_main_menu_t(engine_context_t& engine);

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time) override;

    void change_submenu(submenu_t submenu);

private:
    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    font_handle_t          font_small_m;
    font_handle_t          font_medium_m;
    texture_handle_t       indicator_m;

    submenu_t                                    current_submenu_m{submenu_t::main_k};
    std::map<submenu_t, std::unique_ptr<menu_t>> submenus_m;

    audio_clip_handle_t blip_m;
};

} // namespace mau
