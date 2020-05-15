#pragma once

#include "mau/math/vector.hpp"

#include <algorithm>

namespace mau {
namespace math {

// Signed modulo.
template<typename T>
inline T signed_mod(T a, T b)
{
    return (a % b + b) % b;
}

template<typename T>
T clamp(T value)
{
    return std::max(static_cast<T>(0), std::min(value, static_cast<T>(1)));
}

template<typename T>
T clamp(T value, T lower, T upper)
{
    return std::max(lower, std::min(value, upper));
}

template<typename T, typename F>
T lerp(T lower, T upper, F gradient)
{
    return lower + (upper - lower) * clamp(gradient);
}

// Returns true if axis-aligned bounding boxes intersect.
inline bool aabb_intersect(glm::vec3 a_position, glm::vec3 a_size, glm::vec3 b_position, glm::vec3 b_size)
{
    for(auto i = 0; i < 3; ++i)
    {
        if(std::abs(a_position[i] - b_position[i]) * 2 >= (a_size[i] + b_size[i]))
            return false;
    }
    return true;
}

// Returns true if axis-aligned bounding box and ray intersect.
inline bool aabb_ray_intersect(const glm::vec3 aabb_position,
                               const glm::vec3 aabb_size,
                               const glm::vec3 ray_origin,
                               const glm::vec3 ray_direction,
                               glm::vec3&      point)
{
    glm::vec3 tmin, tmax;
    glm::vec3 bounds[2] = {aabb_position - aabb_size / 2.0f, aabb_position + aabb_size / 2.0f};

    glm::vec3 inverse_direction = 1.0f / ray_direction;

    tmin.x = (bounds[inverse_direction.x < 0].x - ray_origin.x) * inverse_direction.x;
    tmax.x = (bounds[inverse_direction.x >= 0].x - ray_origin.x) * inverse_direction.x;
    tmin.y = (bounds[inverse_direction.y < 0].y - ray_origin.y) * inverse_direction.y;
    tmax.y = (bounds[inverse_direction.y >= 0].y - ray_origin.y) * inverse_direction.y;

    if((tmin.x > tmax.y) || (tmin.y > tmax.x))
        return false;
    if(tmin.y > tmin.x)
        tmin.x = tmin.y;
    if(tmax.y < tmax.x)
        tmax.x = tmax.y;

    tmin.z = (bounds[inverse_direction.z < 0].z - ray_origin.z) * inverse_direction.z;
    tmax.z = (bounds[inverse_direction.z >= 0].z - ray_origin.z) * inverse_direction.z;

    if((tmin.x > tmax.z) || (tmin.z > tmax.x))
        return false;
    if(tmin.z > tmin.x)
        tmin.x = tmin.z;
    if(tmax.z < tmax.x)
        tmax.x = tmax.z;

    float t = tmin.x;

    if(t < 0)
    {
        t = tmax.x;
        if(t < 0)
            return false;
    }

    point = ray_origin + ray_direction * t;

    return true;
}

} // namespace math
} // namespace mau
