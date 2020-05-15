#include "worship/gameplay/particle_entity.hpp"

namespace mau {
particle_entity_t::particle_entity_t(world_t& world, glm::vec3 position, particle_descriptor_t descriptor) :
    entity_t(world, position),
    descriptor_m(descriptor),
    vertex_object_m(vertex_object_t::create(vertex_primitive_t::point_k, vertex_object_mode_t::dynamic_k, {}))
{
    uint16_t count = glm::linearRand(descriptor.count.first, descriptor.count.second);

    for(uint16_t i = 0; i < count; ++i)
    {
        glm::vec3 position = glm::sphericalRand(descriptor.radius);

        particle_info_t particle_info;
        particle_info.max_life       = glm::linearRand(descriptor.lifetime.first, descriptor.lifetime.second);
        particle_info.current_life   = particle_info.max_life;
        particle_info.original_color = glm::linearRand(descriptor.color.first, descriptor.color.second);

        float velocity_magnitude = glm::linearRand(descriptor.velocity.first, descriptor.velocity.second);
        particle_info.velocity   = glm::normalize(position) * velocity_magnitude;

        particle_info_m.push_back(particle_info);

        vertex_t vertex{position, {}, particle_info.original_color};
        particle_vertices_m.push_back(vertex);
    }

    update_vertex_object();
}
particle_entity_t::~particle_entity_t()
{
}
void particle_entity_t::variable_update(float delta_time)
{

    bool alive_particle_present{false};
    for(size_t i = 0; i < particle_info_m.size(); ++i)
    {
        auto& position = particle_vertices_m[i].position;
        auto& color    = particle_vertices_m[i].color;
        auto& info     = particle_info_m[i];

        position += particle_info_m[i].velocity * delta_time;
        info.velocity += descriptor_m.gravity * delta_time;

        info.current_life = glm::clamp(info.current_life - delta_time, 0.0f, info.max_life);
        if(info.current_life > 0.0f)
            alive_particle_present = true;

        if (info.current_life <= 0.0f)
            color.a = 0.0f;
    }

    if(!alive_particle_present)
        destroy();
}
void particle_entity_t::render(renderer_t& renderer, shader_handle_t ubershader, float delta_time)
{
    update_vertex_object();

    renderer.bind_texture(descriptor_m.texture_diffuse, 0);
    renderer.bind_texture(descriptor_m.texture_emission, 1);

    glm::mat4 model = glm::translate(position());

    ubershader->set_uniform_mat4(shader_uniform_t::model_matrix_k, model);
    ubershader->set_uniform_bool(shader_uniform_t::enable_particle_mode_k, true);
    ubershader->set_uniform_float(shader_uniform_t::particle_size_k, descriptor_m.particle_size);
    ubershader->set_uniform_vec2(shader_uniform_t::resolution_k, renderer.viewport_size());

    renderer.render_vertex_object(vertex_object_m);

    ubershader->set_uniform_bool(shader_uniform_t::enable_particle_mode_k, false);
}

void particle_entity_t::update_vertex_object()
{
    vertex_object_m->update(particle_vertices_m);
}

} // namespace mau
