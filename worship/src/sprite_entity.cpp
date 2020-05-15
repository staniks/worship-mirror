#include "worship/gameplay/sprite_entity.hpp"

#include "worship/gameplay/world/world.hpp"

namespace mau {
sprite_entity_t::sprite_entity_t(world_t& world, glm::vec3 position) : entity_t(world, position)
{
}
sprite_entity_t::~sprite_entity_t()
{
}
void sprite_entity_t::render(renderer_t& renderer, shader_handle_t ubershader, float delta_time)
{
    renderer.bind_texture(sprite_texture_diffuse(), 0);
    renderer.bind_texture(sprite_texture_emission(), 1);

    glm::mat4 model = glm::translate(position());

    ubershader->set_uniform_mat4(shader_uniform_t::model_matrix_k, model);
    ubershader->set_uniform_bool(shader_uniform_t::enable_sprite_billboarding_k, true);

    glm::mat4 rotation = glm::rotate(sprite_rotation(), glm::vec3(0, 0, -1));

    glm::vec3 vertices_k[] = {rotation * glm::vec4{glm::vec3{-0.5f, 0.5f, 0.0f} * sprite_scale(), 1.0f},
                              rotation * glm::vec4{glm::vec3{-0.5f, -0.5f, 0.0f} * sprite_scale(), 1.0f},
                              rotation * glm::vec4{glm::vec3{0.5f, 0.5f, 0.0f} * sprite_scale(), 1.0f},
                              rotation * glm::vec4{glm::vec3{0.5f, -0.5f, 0.0f} * sprite_scale(), 1.0f}};

    static constexpr glm::vec2 tex_coords_k[] = {
        glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 1.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}};

    const glm::vec4 lighting = world_m.light({position().x, position().z});

    // Scope vertex batch.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        batch.insert(vertex_t{vertices_k[0], tex_coords_k[0], lighting});
        batch.insert(vertex_t{vertices_k[1], tex_coords_k[1], lighting});
        batch.insert(vertex_t{vertices_k[2], tex_coords_k[2], lighting});

        batch.insert(vertex_t{vertices_k[2], tex_coords_k[2], lighting});
        batch.insert(vertex_t{vertices_k[1], tex_coords_k[1], lighting});
        batch.insert(vertex_t{vertices_k[3], tex_coords_k[3], lighting});
    }

    ubershader->set_uniform_bool(shader_uniform_t::enable_sprite_billboarding_k, false);
}
texture_handle_t sprite_entity_t::sprite_texture_diffuse()
{
    return nullptr;
}
texture_handle_t sprite_entity_t::sprite_texture_emission()
{
    return nullptr;
}
float sprite_entity_t::sprite_scale()
{
    return sprite_scale_m;
}
float sprite_entity_t::sprite_rotation()
{
    return sprite_rotation_m;
}
} // namespace mau
