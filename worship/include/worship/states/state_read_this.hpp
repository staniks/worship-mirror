#pragma once

#include <mau/base/state.hpp>
#include <mau/base/timer.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

namespace mau {

enum class read_this_behavior_t
{
    goto_main_menu_k,
    pop_k
};

class state_read_this_t : public state_t
{
public:
    explicit state_read_this_t(engine_context_t& engine, std::string_view text_path, std::string_view texture_path, read_this_behavior_t behavior);

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time);

private:
    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    font_handle_t          font_small_m;
    texture_handle_t       background_m;
    read_this_behavior_t   behavior_m;

    std::vector<std::string> lines_m;
};

} // namespace mau
