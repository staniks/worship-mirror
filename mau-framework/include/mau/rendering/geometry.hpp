#pragma once

#include "mau/containers/span.hpp"
#include "mau/math/vector.hpp"

#include <array>

namespace mau {

struct vertex_t
{
    vertex_t(glm::vec3 position);
    vertex_t(glm::vec3 position, glm::vec2 uv);
    vertex_t(glm::vec3 position, glm::vec2 uv, glm::vec4 color);

    glm::vec3 position;
    glm::vec2 uv;
    glm::vec4 color;
};

} // namespace mau
