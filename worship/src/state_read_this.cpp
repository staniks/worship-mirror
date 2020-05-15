#include "worship/states/state_read_this.hpp"

#include "worship/states/state_title_screen.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

state_read_this_t::state_read_this_t(engine_context_t& engine, std::string_view text_path, std::string_view texture_path, read_this_behavior_t behavior)
    : state_t(engine, "read_this"), behavior_m(behavior)
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_small_m          = engine.resource_cache().resource<font_t>("fonts/font_small.mfo");
    background_m          = engine.resource_cache().resource<texture_t>(texture_path);

    auto text = engine.resource_cache().load_file(text_path);

    lines_m.push_back(std::string{});
    for(uint64_t i=0; i<text->size(); ++i)
    {
        char character = text->data()[i];
        if(character != '\n')
            lines_m.back().push_back(character);
        else
            lines_m.push_back(std::string{});
    }
}
void state_read_this_t::handle_event(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(behavior_m == read_this_behavior_t::pop_k)
            engine_m.state_manager().pop();
        else
            engine_m.state_manager().push_clear(std::make_unique<state_title_screen_t>(engine_m));
    }
}
void state_read_this_t::fixed_update(float delta_time)
{
}
void state_read_this_t::variable_update(float delta_time)
{
}
render_target_handle_t state_read_this_t::render(float delta_time)
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
        renderer.batch_sprite(batch, {0.0f, 0.0f}, engine_m.viewport_size(), glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    }

    // Scope vertex batch. Main text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        static constexpr glm::vec2 offset{8,8};

        for(size_t i=0; i<lines_m.size(); ++i)
        {
            glm::vec2 line_offset{0, font_small_m->character_size.y * i};
            renderer.batch_string(batch, font_small_m, offset + line_offset, lines_m[i], glm::vec4{0.75, 0, 0, 1});
        }

    }

    return scene_render_target_m;
}

} // namespace mau
