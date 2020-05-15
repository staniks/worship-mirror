#include "mau/rendering/vertex_object.hpp"

#include "mau/rendering/GL/common.hpp"

namespace mau {

vertex_object_handle_t vertex_object_t::create(vertex_primitive_t primitive, vertex_object_mode_t mode, span_t<vertex_t> vertices)
{
    return std::make_shared<vertex_object_t>(primitive, mode, vertices);
}

vertex_object_t::vertex_object_t(vertex_primitive_t primitive, vertex_object_mode_t mode, span_t<vertex_t> vertices) :
    primitive_m(primitive), mode_m(mode), vertex_count_m(vertices.size())
{
    glGenVertexArrays(1, &gl_vao_handle_m);
    glBindVertexArray(gl_vao_handle_m);

    glGenBuffers(1, &gl_vbo_handle_m);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo_handle_m);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), vertices.data(), gl_mode(mode_m));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, vertex_t::color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, vertex_t::uv));

    glBindVertexArray(0);
}
vertex_object_t::~vertex_object_t()
{
    glDeleteBuffers(1, &gl_vbo_handle_m);
    glDeleteVertexArrays(1, &gl_vao_handle_m);
}
void vertex_object_t::update(span_t<vertex_t> vertices)
{
    glBindVertexArray(gl_vao_handle_m);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo_handle_m);

    if(vertices.size() <= vertex_count_m)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertex_t), vertices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), vertices.data(), gl_mode(mode_m));
    }

    vertex_count_m = vertices.size();

    glBindVertexArray(0);
}
size_t vertex_object_t::vertex_count() const
{
    return vertex_count_m;
}
uint32_t vertex_object_t::gl_handle() const
{
    return gl_vao_handle_m;
}
void vertex_object_t::set_primitive(vertex_primitive_t primitive)
{
    primitive_m = primitive;
}
vertex_primitive_t vertex_object_t::primitive() const
{
    return primitive_m;
}
vertex_object_mode_t vertex_object_t::mode() const
{
    return mode_m;
}
gl_handle_t vertex_object_t::gl_primitive(vertex_primitive_t primitive)
{
    switch(primitive)
    {
        case vertex_primitive_t::point_k: return GL_POINTS;
        case vertex_primitive_t::triangle_k: return GL_TRIANGLES;
        case vertex_primitive_t::line_k: return GL_LINES;
        default: assert(false);
    }
}
gl_handle_t vertex_object_t::gl_mode(vertex_object_mode_t mode)
{
    switch(mode)
    {
        case vertex_object_mode_t::static_k: return GL_STATIC_DRAW;
        case vertex_object_mode_t::dynamic_k: return GL_DYNAMIC_DRAW;
        case vertex_object_mode_t::stream_k: return GL_STREAM_DRAW;
        default: assert(false);
    }
}
} // namespace mau
