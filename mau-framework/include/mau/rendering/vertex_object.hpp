#pragma once

#include "mau/rendering/geometry.hpp"

#include <memory>

namespace mau {

// TODO: Double-check whether these comments are correct. It's hard to test since I'm seeing virtually no difference in dynamic
// and stream on my driver.
enum class vertex_object_mode_t
{
    static_k,  // Mesh data will never change.
    dynamic_k, // Mesh data will change sometimes, but not every frame.
    stream_k   // Mesh data will change every frame.
};

enum class vertex_primitive_t
{
    point_k,
    line_k,
    triangle_k
};

// Forward declarations.
class vertex_object_t;
using vertex_object_handle_t = std::shared_ptr<vertex_object_t>;

// Vertex object abstraction.
class vertex_object_t : non_copyable_t, non_movable_t
{
public:
    static vertex_object_handle_t create(vertex_primitive_t primitive, vertex_object_mode_t mode, span_t<vertex_t> vertices);

    vertex_object_t(vertex_primitive_t primitive, vertex_object_mode_t mode, span_t<vertex_t> vertices);
    ~vertex_object_t();

    // Replace vertex object vertices with provided vertices. Note: this is an expensive operation.
    void update(span_t<vertex_t> vertices);

    size_t   vertex_count() const;
    uint32_t gl_handle() const;

    void set_primitive(vertex_primitive_t primitive);

    vertex_primitive_t   primitive() const;
    vertex_object_mode_t mode() const;

    static gl_handle_t gl_primitive(vertex_primitive_t pPrimitive);
    static gl_handle_t gl_mode(vertex_object_mode_t pMode);

private:
    vertex_primitive_t   primitive_m;
    const vertex_object_mode_t mode_m;

    gl_handle_t gl_vao_handle_m{0};
    gl_handle_t gl_vbo_handle_m{0};

    size_t vertex_count_m{0};
};

} // namespace mau
