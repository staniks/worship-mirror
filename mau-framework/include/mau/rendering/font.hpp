#pragma once

#include "mau/io/resource.hpp"
#include "mau/math/vector.hpp"
#include "mau/rendering/texture.hpp"

namespace mau {

// Forward declarations.
class font_t;
using font_handle_t = std::shared_ptr<font_t>;

// Font container. Consists of a font texture and rendering info.
class font_t : public resource_t
{
public:
    static font_handle_t create_resource(engine_context_t& engine, file_handle_t file);

    font_t(texture_handle_t texture, glm::ivec2 character_count, glm::vec2 character_size, bool shadow, bool gradient);

    texture_handle_t texture();

    // Horizontal and vertical character count.
    const glm::ivec2 character_count;

    // Character size in pixels.
    const glm::vec2 character_size;

    // Whether to apply font shadow.
    const bool shadow;

    // Whether to apply font gradient.
    const bool gradient;

private:
    texture_handle_t texture_m;
};

} // namespace mau
