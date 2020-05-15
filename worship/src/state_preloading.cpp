#include "worship/states/state_preloading.hpp"

#include "worship/states/state_title_screen.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

state_preloading_t::state_preloading_t(engine_context_t& engine) : state_t(engine, "preloading_screen")
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_small_m          = engine.resource_cache().resource<font_t>("fonts/font_small.mfo");
    background_m          = engine.resource_cache().resource<texture_t>("main-menu-background.tex");
    white_m               = engine.resource_cache().resource<texture_t>("white.tex");

    preloading_context_m = engine.resource_cache().create_preloading_context();

    // Since we're not doing multithreaded loading, we load a single resource per frame. This
    // is bad, but it's even worse when vsync is enabled. So disable it, and restore it to
    // value from configuration in the destructor.
    engine_m.renderer().set_vsync(false);
}
state_preloading_t::~state_preloading_t()
{
    engine_m.renderer().restore_vsync();
}
void state_preloading_t::handle_event(const SDL_Event& event)
{
}
void state_preloading_t::fixed_update(float delta_time)
{
}
void state_preloading_t::variable_update(float delta_time)
{
    if(preloading_context_m.current())
        preloading_descriptor_m = *preloading_context_m.current();

    preloading_result_m = engine_m.resource_cache().preload_next(preloading_context_m);

    if(preloading_result_m.finished)
        engine_m.state_manager().push_clear(std::make_unique<state_title_screen_t>(engine_m));
}
render_target_handle_t state_preloading_t::render(float delta_time)
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
    renderer.bind_texture(white_m, 1);

    // Scope vertex batch. Background image.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, {0.0f, 0.0f}, engine_m.viewport_size(), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    }

    // Scope vertex batch. Loading text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        std::string_view text = preloading_descriptor_m.path;
        float x = engine_m.viewport_size().x / 2.0f - font_small_m->character_size.x * text.length() / 2.0f;

        renderer.batch_string(batch, font_small_m, {x, 180}, text, glm::vec4{0.65, 0, 0, 1});
    }

    renderer.bind_texture(white_m, 0);
    renderer.bind_texture(white_m, 1);
    // Scope vertex batch. Loading bars..
    {
        static constexpr float loading_bar_size_k = 200.0f;
        float loading_bar_offset = engine_m.viewport_size().x / 2.0f - loading_bar_size_k / 2;

        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, glm::vec2{loading_bar_offset, 190.0f}, glm::vec2{loading_bar_size_k, 2.0f}, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        renderer.batch_sprite(batch,
                              glm::vec2{loading_bar_offset, 190.0f},
                              glm::vec2{loading_bar_size_k * preloading_result_m.progress, 2.0f},
                              glm::vec4{0.75f, 0.0f, 0.0f, 1.0f});
    }

    return scene_render_target_m;
}

} // namespace mau
