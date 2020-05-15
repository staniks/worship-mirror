#pragma once

#include "mau/rendering/vertex_object.hpp"

#include <vector>

namespace mau {

// Forward declaration.
class renderer_t;

// Vertex batch abstraction.
class vertex_batch_t : non_movable_t, non_copyable_t
{
public:
    vertex_batch_t(renderer_t& renderer, vertex_primitive_t primitive);
    ~vertex_batch_t();

    void insert(const vertex_t& vertex);

private:
    renderer_t&            renderer_m;
    vertex_primitive_t     primitive_m;
    std::vector<vertex_t>& vertices_m;
    vertex_object_handle_t vertex_object_m;
};

} // namespace mau
