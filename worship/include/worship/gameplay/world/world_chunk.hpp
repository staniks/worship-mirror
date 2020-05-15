#pragma once

#include <mau/math/vector.hpp>
#include <mau/rendering/vertex_object.hpp>

#include "worship/gameplay/world/world_info.hpp"

namespace mau {

class world_t;

struct chunk_t
{
public:
    chunk_t(world_t& world, glm::ivec2 position);

    vertex_object_handle_t vertex_object() const;

private:
    world_t&               world_m;
    glm::ivec2             position_m;
    vertex_object_handle_t vertex_object_m;
};

} // namespace mau
