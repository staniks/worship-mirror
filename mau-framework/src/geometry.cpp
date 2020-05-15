#include "mau/rendering/geometry.hpp"

namespace mau {
vertex_t::vertex_t(glm::vec3 position) : position(position), uv(glm::vec2{0, 0}), color(glm::vec4{1, 1, 1, 1})
{
}
vertex_t::vertex_t(glm::vec3 position, glm::vec2 uv) : position(position), uv(uv), color(glm::vec4{1, 1, 1, 1})
{
}
vertex_t::vertex_t(glm::vec3 position, glm::vec2 uv, glm::vec4 color) : position(position), uv(uv), color(color)
{
}

} // namespace mau
