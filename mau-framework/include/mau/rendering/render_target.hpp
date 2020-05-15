#pragma once

#include "mau/base/types.hpp"
#include "mau/rendering/texture.hpp"

#include <vector>

namespace mau {

// Forward declarations.
class renderer_t;
class render_target_t;
using render_target_handle_t = std::shared_ptr<render_target_t>;

// Render target abstraction.
class render_target_t : non_movable_t, non_copyable_t
{
public:
    static render_target_handle_t create(glm::ivec2 size, uint32_t color_attachment_count);

    // Create render target with specified resolution and color attachment count.
    render_target_t(glm::ivec2 size, uint32_t color_attachment_count);
    ~render_target_t();

    glm::ivec2    size() const;
    gl_handle_t   gl_handle() const;
    texture_handle_t texture(uint32_t pIndex) const;

private:
    glm::ivec2                    size_m;
    gl_handle_t                   gl_handle_m;
    gl_handle_t                   rbo_handle_m;
    std::vector<texture_handle_t> textures_m;
};

} // namespace mau
