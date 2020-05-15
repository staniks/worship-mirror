#include "mau/rendering/vertex_batch.hpp"

#include "mau/rendering/renderer.hpp"

namespace mau {

vertex_batch_t::vertex_batch_t(renderer_t& renderer, vertex_primitive_t primitive) :
    renderer_m(renderer), primitive_m(primitive), vertices_m(renderer_m.batch_vertices()), vertex_object_m(renderer_m.batch_vertex_object())
{
    vertices_m.clear();
}
vertex_batch_t::~vertex_batch_t()
{
    vertex_object_m->set_primitive(primitive_m);
    vertex_object_m->update(vertices_m);
    renderer_m.render_vertex_object(vertex_object_m);
}
void vertex_batch_t::insert(const vertex_t& vertex)
{
    vertices_m.push_back(vertex);
}

} // namespace mau
