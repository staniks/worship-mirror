#include "mau/rendering/render_target.hpp"

#include "mau/rendering/GL/common.hpp"

namespace mau {
render_target_handle_t render_target_t::create(glm::ivec2 size, uint32_t color_attachment_count)
{
    return std::make_shared<render_target_t>(size, color_attachment_count);
}
render_target_t::render_target_t(glm::ivec2 size, uint32_t color_attachment_count) : size_m(size)
{
    glGenFramebuffers(1, &gl_handle_m);
    glBindFramebuffer(GL_FRAMEBUFFER, gl_handle_m);

    std::vector<uint32_t> color_attachments;

    for(uint32_t i = 0; i < color_attachment_count; ++i)
    {
        texture_handle_t texture = texture_t::create(size);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->gl_handle(), 0);
        textures_m.push_back(texture);
        color_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers((GLsizei)color_attachments.size(), color_attachments.data());

    glGenRenderbuffers(1, &rbo_handle_m);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_handle_m);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_handle_m);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
render_target_t::~render_target_t()
{
    glDeleteRenderbuffers(1, &rbo_handle_m);
    glDeleteFramebuffers(1, &gl_handle_m);
}
glm::ivec2 render_target_t::size() const
{
    return size_m;
}
gl_handle_t render_target_t::gl_handle() const
{
    return gl_handle_m;
}
texture_handle_t render_target_t::texture(uint32_t pIndex) const
{
    return textures_m.at(pIndex);
}
} // namespace mau
