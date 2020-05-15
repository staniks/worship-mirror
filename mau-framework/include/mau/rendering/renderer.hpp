#pragma once

#include "mau/base/module.hpp"
#include "mau/base/types.hpp"
#include "mau/math/matrix.hpp"
#include "mau/memory/value_container.hpp"
#include "mau/rendering/depth_test_lock.hpp"
#include "mau/rendering/font.hpp"
#include "mau/rendering/geometry.hpp"
#include "mau/rendering/render_target.hpp"
#include "mau/rendering/shader.hpp"
#include "mau/rendering/texture.hpp"
#include "mau/rendering/vertex_batch.hpp"
#include "mau/rendering/vertex_object.hpp"

#include <SDL.h>

#include <array>
#include <string>
#include <vector>

namespace mau {

using renderer_exception_t = exception_t;

enum class framebuffer_render_mode_t
{
    fill_ndc_k,
    perform_aspect_correction_k
};

enum class fill_mode_t
{
    wireframe_k,
    fill_k
};

// renderer_t class.
class renderer_t : public module_t
{
public:
    explicit renderer_t(engine_context_t& engine);
    ~renderer_t();

    // Buffer clearing.
    void clear(glm::vec4 color);
    void clear_depth();
    void clear_color(glm::vec4 color);

    void         set_depth_test(depth_test_t depth_test);
    depth_test_t depth_test();

    void        set_fill_mode(fill_mode_t fill_mode);
    fill_mode_t fill_mode();

    // Set vsync regardless of configuration.
    void set_vsync(bool enabled);
    // Restore vsync setting to configuration-specified.
    void restore_vsync();

    void bind_shader(shader_handle_t shader);
    void bind_render_target(render_target_handle_t render_target);
    void bind_texture(texture_handle_t texture, uint32_t texture_unit);

    void clear_render_target();

    void present();

    void render_vertex_object(vertex_object_handle_t vertex_array);

    // Batching.
    void batch_sprite(vertex_batch_t& batch, glm::vec2 position, glm::vec2 size, glm::vec4 color);
    void batch_character(vertex_batch_t& batch, font_handle_t font, glm::vec2 position, char character, glm::vec4 color);
    void batch_string(vertex_batch_t& batch, font_handle_t font, glm::vec2 position, std::string_view string, glm::vec4 color);
    void batch_framebuffer_fullscreen(vertex_batch_t& batch, framebuffer_render_mode_t mode);

    std::vector<vertex_t>& batch_vertices();
    vertex_object_handle_t batch_vertex_object();

    shader_handle_t passthrough_shader();

    glm::vec2 viewport_size() const;

private:
    value_container_t<SDL_GLContext, decltype(&SDL_GL_DeleteContext)> gl_context_m;

    shader_handle_t passthrough_shader_m;

    std::vector<vertex_t>  batch_vertices_m;
    vertex_object_handle_t batch_vertex_object_m;
};

} // namespace mau
