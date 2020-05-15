#include "mau/rendering/renderer.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/io/file_reader.hpp"
#include "mau/io/resource_cache.hpp"
#include "mau/math/algorithms.hpp"
#include "mau/rendering/GL/common.hpp"

// OpenGL diagnostics callback.
void gl_message_callback(GLenum        source,
                         GLenum        type,
                         GLuint        id,
                         GLenum        severity,
                         GLsizei       length,
                         const GLchar* message,
                         const void*   user_param)
{
    if(severity == GL_DEBUG_SEVERITY_HIGH)
    {
        mau::engine_context_t* engine = (mau::engine_context_t*)user_param;
        engine->log().log(mau::log_severity_t::error_k, std::string_view{message, (size_t)length});
    }
}

namespace mau {

renderer_t::renderer_t(engine_context_t& engine) :
    module_t(engine, "renderer"), gl_context_m(SDL_GL_CreateContext(engine.sdl_window()), SDL_GL_DeleteContext)

{
    if(gl_context_m.get() == NULL)
        throw exception_t{"unable to create OpenGL context"};

    if(gl3wInit())
    {
        throw exception_t("failed to initialize OpenGL");
    }

    if(!gl3wIsSupported(target_gl_version_major_k, target_gl_version_minor_k))
    {
        throw exception_t("openGL version not supported");
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_message_callback, &engine);

    // Set VSync setting.
    SDL_GL_SetSwapInterval(engine.config().vsync ? 1 : 0);

    glViewport(0, 0, engine_m.resolution().x, engine_m.resolution().y);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    batch_vertex_object_m =
        vertex_object_t::create(vertex_primitive_t::triangle_k, vertex_object_mode_t::dynamic_k, span_t<vertex_t>{});

    passthrough_shader_m = engine_m.resource_cache().resource<shader_t>("passthrough.sha");
}

renderer_t::~renderer_t()
{
    glDisable(GL_DEBUG_OUTPUT);
}

void renderer_t::clear(glm::vec4 color)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(color.r, color.g, color.b, color.a);
}

void renderer_t::clear_depth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void renderer_t::clear_color(glm::vec4 color)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(color.r, color.g, color.b, color.a);
}

void renderer_t::set_depth_test(depth_test_t depth_test)
{
    if(depth_test == depth_test_t::enabled_k)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

depth_test_t renderer_t::depth_test()
{
    return glIsEnabled(GL_DEPTH_TEST) ? depth_test_t::enabled_k : depth_test_t::disabled_k;
}

void renderer_t::set_fill_mode(fill_mode_t fill_mode)
{
    GLenum gl_mode = fill_mode == fill_mode_t::fill_k ? GL_FILL : GL_LINE;
    glPolygonMode(GL_FRONT_AND_BACK, gl_mode);
}

fill_mode_t renderer_t::fill_mode()
{
    GLint previous[2];
    glGetIntegerv(GL_POLYGON_MODE, previous);

    return previous[1] == GL_FILL ? fill_mode_t::fill_k : fill_mode_t::wireframe_k;
}

void renderer_t::set_vsync(bool enabled)
{
    SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

void renderer_t::restore_vsync()
{
    SDL_GL_SetSwapInterval(engine_m.config().vsync ? 1 : 0);
}

void renderer_t::bind_shader(shader_handle_t shader)
{
    glUseProgram(shader->gl_handle());

    shader->bind_texture_units();
}

void renderer_t::bind_render_target(render_target_handle_t render_target)
{
    glBindFramebuffer(GL_FRAMEBUFFER, render_target->gl_handle());

    auto size = render_target->size();
    glViewport(0, 0, size.x, size.y);
}

void renderer_t::bind_texture(texture_handle_t texture, uint32_t texture_unit)
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->gl_handle() : 0);
}

void renderer_t::clear_render_target()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, engine_m.resolution().x, engine_m.resolution().y);
}

void renderer_t::present()
{
    SDL_GL_SwapWindow(engine_m.sdl_window());
}

void renderer_t::render_vertex_object(vertex_object_handle_t vertex_object)
{
    glBindVertexArray(vertex_object->gl_handle());
    glDrawArrays(
        vertex_object_t::gl_primitive(vertex_object->primitive()), 0, static_cast<GLsizei>(vertex_object->vertex_count()));
}

std::vector<vertex_t>& renderer_t::batch_vertices()
{
    return batch_vertices_m;
}

vertex_object_handle_t renderer_t::batch_vertex_object()
{
    return batch_vertex_object_m;
}

shader_handle_t renderer_t::passthrough_shader()
{
    return passthrough_shader_m;
}

void renderer_t::batch_sprite(vertex_batch_t& batch, glm::vec2 position, glm::vec2 size, glm::vec4 color)
{
    batch.insert(vertex_t{glm::vec3{position, 0.0f}, glm::vec2{0.0f, 0.0f}, color});
    batch.insert(vertex_t{glm::vec3{position, 0.0f} + glm::vec3{0.0f, size.y, 0.0f}, glm::vec2{0.0f, 1.0f}, color});
    batch.insert(vertex_t{glm::vec3{position, 0.0f} + glm::vec3{size.x, 0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, color});
    batch.insert(vertex_t{glm::vec3{position, 0.0f} + glm::vec3{size.x, 0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, color});
    batch.insert(vertex_t{glm::vec3{position, 0.0f} + glm::vec3{0.0f, size.y, 0.0f}, glm::vec2{0.0f, 1.0f}, color});
    batch.insert(vertex_t{glm::vec3{position, 0.0f} + glm::vec3{size.x, size.y, 0.0f}, glm::vec2{1.0f, 1.0f}, color});
}

void renderer_t::batch_character(vertex_batch_t& batch, font_handle_t font, glm::vec2 position, char character, glm::vec4 color)
{
    const glm::ivec2 count = font->character_count;
    const glm::vec2  size  = font->character_size;

    const glm::vec2 uv_size = glm::vec2{1.0f / count.x, 1.0f / count.y};
    const glm::vec2 uv_coord{(float)(character % count.x) / count.x, (float)(character / count.x) / count.y};

    const int passes = font->shadow ? 2 : 1;

    for(int pass = 0; pass < passes; ++pass)
    {
        glm::vec2 temp_position = position;
        glm::vec4 color_top     = color;
        glm::vec4 color_bottom  = font->gradient ? color_top * 0.675f : color_top;

        if(font->shadow && pass == 0)
        {
            temp_position += glm::vec2{1, 1};
            color_top    = glm::vec4(0, 0, 0, 1);
            color_bottom = glm::vec4(0, 0, 0, 1);
        }

        batch.insert(vertex_t{glm::vec3{temp_position, 0.0f}, uv_coord, color_top});
        batch.insert(vertex_t{
            glm::vec3{temp_position + glm::vec2{0.0f, size.y}, 0.0f}, uv_coord + glm::vec2{0.0f, uv_size.y}, color_bottom});
        batch.insert(vertex_t{
            glm::vec3{temp_position + glm::vec2{size.x, 0.0f}, 0.0f}, uv_coord + glm::vec2{uv_size.x, 0.0f}, color_top});
        batch.insert(vertex_t{
            glm::vec3{temp_position + glm::vec2{size.x, 0.0f}, 0.0f}, uv_coord + glm::vec2{uv_size.x, 0.0f}, color_top});
        batch.insert(vertex_t{
            glm::vec3{temp_position + glm::vec2{0.0f, size.y}, 0.0f}, uv_coord + glm::vec2{0.0f, uv_size.y}, color_bottom});
        batch.insert(vertex_t{glm::vec3{temp_position + glm::vec2{size.x, size.y}, 0.0f},
                              uv_coord + glm::vec2{uv_size.x, uv_size.y},
                              color_bottom});
    }
}

void renderer_t::batch_string(vertex_batch_t&  batch,
                              font_handle_t    font,
                              glm::vec2        position,
                              std::string_view string,
                              glm::vec4        color)
{
    bind_texture(font->texture(), 0);
    glm::vec2 delta{font->character_size.x, 0.0f};

    for(size_t i = 0; i < string.size(); ++i)
    {
        char c = string[i];
        batch_character(batch, font, position + delta * (float)i, c, color);
    }
}

void renderer_t::batch_framebuffer_fullscreen(vertex_batch_t& batch, framebuffer_render_mode_t mode)
{
    // Since NDC has (-1, 1) in upper left, careful with vertex order - it's opposite of the orthographic vertex order for front
    // faces. Also, the FBO textures have origin at (-1, -1), so keep that in mind.

    float actual_aspect = engine_m.resolution().x / (float)engine_m.resolution().y;
    float target_aspect = engine_m.viewport_size().x / (float)engine_m.viewport_size().y;

    float correction = mode == framebuffer_render_mode_t::fill_ndc_k ? 1.0f : target_aspect / actual_aspect;

    batch.insert(vertex_t{glm::vec3{-1 * correction, 1, 0}, glm::vec2{0, 1}});
    batch.insert(vertex_t{glm::vec3{-1 * correction, -1, 0}, glm::vec2{0, 0}});
    batch.insert(vertex_t{glm::vec3{1 * correction, 1, 0}, glm::vec2{1, 1}});

    batch.insert(vertex_t{glm::vec3{-1 * correction, -1, 0}, glm::vec2{0, 0}});
    batch.insert(vertex_t{glm::vec3{1 * correction, -1, 0}, glm::vec2{1, 0}});
    batch.insert(vertex_t{glm::vec3{1 * correction, 1, 0}, glm::vec2{1, 1}});
}

glm::vec2 renderer_t::viewport_size() const
{
    return engine_m.viewport_size();
}

} // namespace mau
