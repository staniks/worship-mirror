#pragma once

#include "mau/io/image.hpp"
#include "mau/io/resource.hpp"
#include "mau/math/vector.hpp"

namespace mau {

// Forward declarations.
class engine_context_t;
class texture_t;
using texture_handle_t = std::shared_ptr<texture_t>;

// Texture container. Provides raw GL handle access.
class texture_t : public resource_t
{
public:
    static texture_handle_t create(glm::ivec2 size);
    static texture_handle_t create(image_handle_t image);

    explicit texture_t(glm::ivec2 size);
    explicit texture_t(image_handle_t image);
    ~texture_t();

    glm::ivec2  size() const;
    gl_handle_t gl_handle() const;

    static texture_handle_t create_resource(engine_context_t& engine, file_handle_t file);

private:
    glm::ivec2  size_m;
    gl_handle_t gl_handle_m;
};

} // namespace mau
