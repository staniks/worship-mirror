#pragma once

#include "mau/base/types.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/random.hpp>

namespace std {

template<>
struct hash<glm::ivec2>
{
    std::size_t operator()(const glm::ivec2& k) const
    {
        using std::hash;

        // TODO: Revisit this, pretty sure this is a shit hash function.
        return ((hash<int32_t>()(k.x) ^ (hash<int32_t>()(k.y) << 1)) >> 1);
    }
};

} // namespace std
