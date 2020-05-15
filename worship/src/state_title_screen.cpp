#include "worship/states/state_title_screen.hpp"

#include "worship/states/state_main_menu.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

state_title_screen_t::state_title_screen_t(engine_context_t& engine) : state_t(engine, "title_screen")
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_small_m          = engine.resource_cache().resource<font_t>("fonts/font_small.mfo");
    font_medium_m         = engine.resource_cache().resource<font_t>("fonts/font_medium.mfo");
    background_m          = engine.resource_cache().resource<texture_t>("main-menu-background.tex");
}
void state_title_screen_t::handle_event(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        press_any_key_timer_m   = 0.0f;
        press_any_key_visible_m = false;
        engine_m.state_manager().push(std::make_unique<state_main_menu_t>(engine_m));
    }
}
void state_title_screen_t::fixed_update(float delta_time)
{
}
void state_title_screen_t::variable_update(float delta_time)
{
    press_any_key_timer_m += delta_time;
    if(press_any_key_timer_m >= 1.0f / press_any_key_frequency_m)
    {
        press_any_key_visible_m = !press_any_key_visible_m;
        press_any_key_timer_m   = 0.0f;
    }
}
render_target_handle_t state_title_screen_t::render(float delta_time)
{
    renderer_t& renderer = engine_m.renderer();

    renderer.bind_render_target(scene_render_target_m);
    renderer.clear({0.0f, 0.0f, 0.0f, 1.0f});

    renderer.bind_shader(ubershader_m);

    depth_guard_t lock{renderer, depth_test_t::disabled_k};
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, false);
    ubershader_m->set_uniform_mat4(shader_uniform_t::model_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::view_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::projection_matrix_k, math::ortho_matrix(engine_m.viewport_size()));

    renderer.bind_texture(background_m, 0);

    // Scope vertex batch. Background image.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, {0.0f, 0.0f}, engine_m.viewport_size(), glm::vec4(1, 1, 1, 1));
    }

    // Scope vertex batch. Info and copyright text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        constexpr std::string_view text = "https://staniks.github.io";
        // float                      y    = engine_m.viewport_size().y - font_small_m->character_size.y;

        renderer.batch_string(batch, font_small_m, {0, 0}, text, glm::vec4{0.75, 0, 0, 1});
    }

    // Scope vertex batch. Press any key text.
    if(press_any_key_visible_m)
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        constexpr std::string_view text = "PRESS ANY KEY";
        float x = engine_m.viewport_size().x / 2.0f - font_medium_m->character_size.x * text.length() / 2.0f;

        renderer.batch_string(batch, font_medium_m, {x, 180}, text, glm::vec4{1, 0, 0, 1});
    }

    return scene_render_target_m;
}

} // namespace mau
