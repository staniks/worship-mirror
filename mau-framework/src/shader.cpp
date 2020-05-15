#include "mau/rendering/shader.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/io/file_reader.hpp"
#include "mau/io/resource_cache.hpp"
#include "mau/rendering/GL/common.hpp"

#include <array>
#include <string>
#include <fmt/format.h>

namespace mau {

inline static constexpr uint64_t error_buffer_size_k = 4096;

enum class shader_part_type_t
{
    vertex_t,
    fragment_t
};

class shader_part_t : non_copyable_t, non_movable_t
{
public:
    shader_part_t(shader_part_type_t type, file_handle_t file)
    {
        const char*   text      = reinterpret_cast<const char*>(file->data());
        const int32_t text_size = static_cast<int32_t>(file->size());

        gl_handle_m = glCreateShader(type == shader_part_type_t::vertex_t ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
        glShaderSource(gl_handle_m, 1, &text, &text_size);
        glCompileShader(gl_handle_m);

        int32_t success{false};
        glGetShaderiv(gl_handle_m, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::array<GLchar, error_buffer_size_k> error_buffer = {0};
            glGetShaderInfoLog(gl_handle_m, static_cast<uint32_t>(error_buffer.size()), nullptr, error_buffer.data());

            glDeleteShader(gl_handle_m);

            throw exception_t{fmt::format("shader compilation error:\n{}", error_buffer.data())};
        }
    }
    ~shader_part_t() { glDeleteShader(gl_handle_m); }

    uint32_t gl_handle() const { return gl_handle_m; };

private:
    uint32_t gl_handle_m{0};
};

shader_handle_t shader_t::create(file_handle_t vertex_file, file_handle_t fragment_file)
{
    return std::make_shared<shader_t>(vertex_file, fragment_file);
}
shader_t::shader_t(file_handle_t vertex_file, file_handle_t fragment_file)
{
    shader_part_t vertexShader{shader_part_type_t::vertex_t, vertex_file};
    shader_part_t fragmentShader{shader_part_type_t::fragment_t, fragment_file};

    gl_handle_m = glCreateProgram();

    glAttachShader(gl_handle_m, vertexShader.gl_handle());
    glAttachShader(gl_handle_m, fragmentShader.gl_handle());

    glLinkProgram(gl_handle_m);

    int32_t success{false};
    glGetProgramiv(gl_handle_m, GL_LINK_STATUS, &success);
    if(!success)
    {
        std::array<GLchar, error_buffer_size_k> error_buffer = {0};
        glGetProgramInfoLog(gl_handle_m, static_cast<uint32_t>(error_buffer.size()), nullptr, error_buffer.data());

        glDeleteProgram(gl_handle_m);

        throw exception_t{fmt::format("shader linking error:\n{}", error_buffer.data())};
    }

    for(auto& pair: shader_uniform_mapping_k)
    {
        shader_uniform_t uniform        = pair.first;
        const char*      uniform_string = pair.second;

        gl_uniform_mapping_m[uniform] = glGetUniformLocation(gl_handle_m, uniform_string);
    }
}
shader_t::~shader_t()
{
    glDeleteProgram(gl_handle_m);
}
void shader_t::set_uniform_mat4(shader_uniform_t uniform, const glm::mat4& value)
{
    glUniformMatrix4fv(gl_uniform(uniform), 1, GL_FALSE, glm::value_ptr(value));
}
void shader_t::set_uniform_vec2(shader_uniform_t uniform, glm::vec2 value)
{
    glUniform2fv(gl_uniform(uniform), 1, glm::value_ptr(value));
}
void shader_t::set_uniform_int(shader_uniform_t uniform, int32_t value)
{
    glUniform1i(gl_uniform(uniform), value);
}
void shader_t::set_uniform_bool(shader_uniform_t uniform, bool value)
{
    glUniform1i(gl_uniform(uniform), value);
}
void shader_t::set_uniform_float(shader_uniform_t uniform, float value)
{
    glUniform1f(gl_uniform(uniform), value);
}
void shader_t::set_uniform_vec3_array(shader_uniform_t uniform, span_t<glm::vec3> values)
{
    if(values.size() == 0)
        return;
    glUniform3fv(gl_uniform(uniform), (GLsizei)values.size(), (GLfloat*)values.data());
}
void shader_t::set_uniform_float_array(shader_uniform_t uniform, span_t<float> values)
{
    if(values.size() == 0)
        return;
    glUniform1fv(gl_uniform(uniform), (GLsizei)values.size(), values.data());
}
void shader_t::bind_texture_units()
{
    for(int i = 0; i < 4; ++i)
        set_uniform_int((shader_uniform_t)((int)shader_uniform_t::texture0_k + i), i);
}
uint32_t shader_t::gl_handle()
{
    return gl_handle_m;
}
shader_handle_t shader_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    std::string_view data{reinterpret_cast<char*>(file->data()), file->size()};

    // TODO: improve robustness, this isn't very resilient.

    auto delimited_index = data.find_first_of(';');
    if(delimited_index == data.length())
        throw exception_t{"no shader delimited found"};

    std::string_view vertex_shader_filename = std::string_view{data.data(), delimited_index};
    std::string_view fragment_shader_filename =
        std::string_view{data.data() + delimited_index + 1, data.size() - delimited_index - 1};

    return create(engine.resource_cache().load_file(vertex_shader_filename),
                  engine.resource_cache().load_file(fragment_shader_filename));
}
gl_uniform_handle_t shader_t::gl_uniform(shader_uniform_t uniform)
{
    return gl_uniform_mapping_m.at(uniform);
}
} // namespace mau
