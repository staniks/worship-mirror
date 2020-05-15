#include "mau/base/state_manager.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/rendering/depth_test_lock.hpp"

namespace mau {
state_manager_t::state_manager_t(engine_context_t& engine) : module_t(engine, "state_manager")
{
}
void state_manager_t::push(std::unique_ptr<state_t> new_state)
{
    engine_m.audio().push();
    state_stack_m.push_back(std::move(new_state));
}

void state_manager_t::push_clear(std::unique_ptr<state_t> new_state)
{
    clear();
    push(std::move(new_state));
}

void state_manager_t::pop()
{
    if(state_stack_m.empty())
        return;

    engine_m.audio().pop();
    state_stack_m.pop_back();
}

void state_manager_t::clear()
{
    engine_m.audio().clear();
    state_stack_m.clear();
}

void state_manager_t::handle_event(const SDL_Event& event)
{
    if(state_stack_m.empty())
        return;

    state_stack_m.back()->handle_event(event);
}

void state_manager_t::fixed_update(float delta_time)
{
    if(state_stack_m.empty())
        return;

    state_stack_m.back()->fixed_update(delta_time);
}

void state_manager_t::variable_update(float delta_time)
{
    if(state_stack_m.empty())
        return;

    state_stack_m.back()->variable_update(delta_time);
}

void state_manager_t::render(float delta_time)
{
    if(state_stack_m.empty())
        return;

    renderer_t&     renderer           = engine_m.renderer();
    shader_handle_t passthrough_shader = renderer.passthrough_shader();

    renderer.clear({0.0f, 0.0f, 0.0f, 1.0f});

    for(auto& state: state_stack_m)
    {
        render_target_handle_t stateResult = state->render(delta_time);

        depth_guard_t depth_lock{renderer, depth_test_t::disabled_k};

        renderer.clear_render_target();
        renderer.bind_shader(passthrough_shader);
        passthrough_shader->set_uniform_int(shader_uniform_t::texture0_k, 0);
        renderer.bind_texture(stateResult->texture(0), 0);
        {
            vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
            renderer.batch_framebuffer_fullscreen(batch, framebuffer_render_mode_t::perform_aspect_correction_k);
        }
    }
}

bool state_manager_t::empty()
{
    return state_stack_m.empty();
}

} // namespace mau
