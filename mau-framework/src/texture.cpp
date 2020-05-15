#include "mau/rendering/texture.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/math/algorithms.hpp"
#include "mau/rendering/GL/common.hpp"

namespace mau {

texture_handle_t texture_t::create(glm::ivec2 size)
{
    return std::make_shared<texture_t>(size);
}
texture_handle_t texture_t::create(image_handle_t image)
{
    return std::make_shared<texture_t>(image);
}

texture_t::texture_t(glm::ivec2 size) : size_m(size)
{
    glGenTextures(1, &gl_handle_m);
    glBindTexture(GL_TEXTURE_2D, gl_handle_m);

    // These are necessary when sampling near the edges of the textures in shader.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_m.x, size_m.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

texture_t::texture_t(image_handle_t image) : size_m(image->size())
{
    const auto& pixels = image->rgba();

    glGenTextures(1, &gl_handle_m);
    glBindTexture(GL_TEXTURE_2D, gl_handle_m);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_m.x, size_m.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}

texture_t::~texture_t()
{
    glDeleteTextures(1, &gl_handle_m);
}

glm::ivec2 texture_t::size() const
{
    return size_m;
}

gl_handle_t texture_t::gl_handle() const
{
    return gl_handle_m;
}

texture_handle_t texture_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    return create(image_t::create(file));
}

} // namespace mau
