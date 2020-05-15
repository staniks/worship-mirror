#pragma once

#include "mau/base/types.hpp"
#include "mau/math/quaternion.hpp"
#include "mau/math/vector.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace mau {
namespace math {

// Creates an orthographic matrix for the specified viewport size.
glm::mat4 ortho_matrix(glm::ivec2 viewport_size);

// Creates a perspective matrix with a specified horizontal field of view, viewport size and clipping planes.
glm::mat4 perspective_matrix(float fov, glm::ivec2 viewport_size, float z_near, float z_far);

// Create a view matrix with camera position and orientation.
glm::mat4 view_matrix(glm::vec3 position, glm::quat orientation);

} // namespace math
} // namespace mau
