#include "worship/states/state_loading.hpp"

#include "worship/states/state_gameplay.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

state_loading_t::state_loading_t(engine_context_t& engine, difficulty_t difficulty) : state_t(engine, "loading_screen")
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_medium_m         = engine.resource_cache().resource<font_t>("fonts/font_medium.mfo");
    background_m          = engine.resource_cache().resource<texture_t>("main-menu-background.tex");
    difficulty_m          = difficulty;
}
void state_loading_t::handle_event(const SDL_Event& event)
{
}
void state_loading_t::fixed_update(float delta_time)
{
}
void state_loading_t::variable_update(float delta_time)
{
    ++delay_counter_m;
    if(delay_counter_m > 1ULL)
    {
        engine_m.state_manager().push_clear(std::make_unique<state_gameplay_t>(engine_m, difficulty_m));
    }
}
render_target_handle_t state_loading_t::render(float delta_time)
{
    renderer_t& renderer = engine_m.renderer();

    renderer.bind_render_target(scene_render_target_m);

    renderer.bind_shader(ubershader_m);

    depth_guard_t lock{renderer, depth_test_t::disabled_k};
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, false);
    ubershader_m->set_uniform_mat4(shader_uniform_t::model_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::view_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::projection_matrix_k, math::ortho_matrix(engine_m.viewport_size()));

    renderer.bind_texture(background_m, 0);
    renderer.bind_texture(nullptr, 1);

    // Scope vertex batch. Background image.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, {0.0f, 0.0f}, engine_m.viewport_size(), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    }

    // Scope vertex batch. Loading text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        constexpr std::string_view text = "LOADING...";
        float x = engine_m.viewport_size().x / 2.0f - font_medium_m->character_size.x * text.length() / 2.0f;

        renderer.batch_string(batch, font_medium_m, {x, 180}, text, glm::vec4{0.65, 0, 0, 1});
    }

    return scene_render_target_m;
}

} // namespace mau
