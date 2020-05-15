#include "mau/math/matrix.hpp"

namespace mau {
namespace math {

glm::mat4 ortho_matrix(glm::ivec2 viewport_size)
{
    return glm::ortho(0.0f, (float)viewport_size.x, (float)viewport_size.y, 0.0f);
}
glm::mat4 perspective_matrix(float fov, glm::ivec2 viewport_size, float z_near, float z_far)
{
    const float aspect = (float)viewport_size.x / viewport_size.y;
    const float fov_y  = glm::atan(glm::tan(fov / 2.0f) / aspect) * 2.0f;
    return glm::perspective(fov_y, aspect, z_near, z_far);
}
glm::mat4 view_matrix(glm::vec3 position, glm::quat orientation)
{
    glm::mat4 rotation_matrix    = glm::inverse(glm::mat4_cast(orientation));
    glm::mat4 translation_matrix = glm::translate(-position);
    return rotation_matrix * translation_matrix;
}

} // namespace math
} // namespace mau
