#include "worship/effects/effect_bloom.hpp"

namespace mau {

effect_bloom_t::effect_bloom_t(engine_context_t& engine) :
    effect_t(engine),
    blur_shader_m(engine.resource_cache().resource<shader_t>("blur.sha")),
    bloom_shader_m(engine.resource_cache().resource<shader_t>("bloom.sha")),
    blur_render_target_m(
        {render_target_t::create(engine_m.viewport_size(), 1), render_target_t::create(engine_m.viewport_size(), 1)}),
    result_render_target_m(render_target_t::create(engine_m.viewport_size(), 1))
{
}
effect_bloom_t::~effect_bloom_t()
{
}
render_target_handle_t effect_bloom_t::apply(render_target_handle_t pInput)
{
    depth_guard_t lock{renderer_m, depth_test_t::disabled_k};

    renderer_m.bind_render_target(blur_render_target_m[0]);
    renderer_m.bind_shader(blur_shader_m);
    renderer_m.bind_texture(pInput->texture(1), 0);
    blur_shader_m->set_uniform_int(shader_uniform_t::pass_k, 0);
    {
        vertex_batch_t batch{renderer_m, vertex_primitive_t::triangle_k};
        renderer_m.batch_framebuffer_fullscreen(batch, framebuffer_render_mode_t::fill_ndc_k);
    }
    renderer_m.bind_render_target(blur_render_target_m[1]);
    renderer_m.bind_texture(blur_render_target_m[0]->texture(0), 0);
    blur_shader_m->set_uniform_int(shader_uniform_t::pass_k, 1);
    {
        vertex_batch_t batch{renderer_m, vertex_primitive_t::triangle_k };
        renderer_m.batch_framebuffer_fullscreen(batch, framebuffer_render_mode_t::fill_ndc_k);
    }

    static constexpr auto blur_count_k = 16;

    for(int i = 0; i < blur_count_k; ++i)
    {
        for(int pass = 0; pass < 2; ++pass)
        {
            renderer_m.bind_render_target(blur_render_target_m[pass]);
            renderer_m.bind_texture(blur_render_target_m[1 - pass]->texture(0), 0);
            blur_shader_m->set_uniform_int(shader_uniform_t::pass_k, pass);
            {
                vertex_batch_t batch{renderer_m, vertex_primitive_t::triangle_k };
                renderer_m.batch_framebuffer_fullscreen(batch, framebuffer_render_mode_t::fill_ndc_k);
            }
        }
    }

    renderer_m.bind_render_target(result_render_target_m);
    renderer_m.bind_shader(bloom_shader_m);
    bloom_shader_m->set_uniform_int(shader_uniform_t::texture0_k, 0);
    bloom_shader_m->set_uniform_int(shader_uniform_t::texture1_k, 1);
    renderer_m.bind_texture(pInput->texture(0), 0);
    renderer_m.bind_texture(blur_render_target_m[1]->texture(0), 1);
    {
        vertex_batch_t batch{renderer_m, vertex_primitive_t::triangle_k };
        renderer_m.batch_framebuffer_fullscreen(batch, framebuffer_render_mode_t::fill_ndc_k);
    }

    return result_render_target_m;
}

} // namespace mau
