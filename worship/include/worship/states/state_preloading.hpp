#pragma once

#include <mau/base/state.hpp>
#include <mau/base/timer.hpp>
#include <mau/rendering/font.hpp>
#include <mau/rendering/render_target.hpp>
#include <mau/rendering/shader.hpp>

#include <mau/io/resource_cache.hpp>

namespace mau {

class state_preloading_t : public state_t
{
public:
    explicit state_preloading_t(engine_context_t& engine);
    ~state_preloading_t();

    virtual void                   handle_event(const SDL_Event& event) override;
    virtual void                   fixed_update(float delta_time) override;
    virtual void                   variable_update(float delta_time) override;
    virtual render_target_handle_t render(float delta_time);

private:
    render_target_handle_t scene_render_target_m;
    shader_handle_t        ubershader_m;
    font_handle_t          font_small_m;
    texture_handle_t       background_m;
    texture_handle_t       white_m;

    resource_preloading_context_t preloading_context_m;
    resource_preloading_result_t  preloading_result_m;
    resource_preloading_context_t::resource_descriptor_t preloading_descriptor_m;
};

} // namespace mau
