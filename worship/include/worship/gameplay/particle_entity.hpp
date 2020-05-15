#pragma once

#include "worship/gameplay/entity.hpp"

#include "worship/gameplay/entity_descriptor.hpp"

namespace mau {

class particle_entity_t : public entity_t
{
public:
    struct particle_info_t
    {
        float     current_life;
        float     max_life;
        glm::vec3 velocity;
        glm::vec4 original_color;
    };

    particle_entity_t(world_t& world, glm::vec3 position, particle_descriptor_t descriptor);
    virtual ~particle_entity_t();

    void variable_update(float delta_time);

    void render(renderer_t& renderer, shader_handle_t ubershader, float delta_time) override;

protected:
    particle_descriptor_t  descriptor_m;
    vertex_object_handle_t vertex_object_m;

    std::vector<vertex_t>        particle_vertices_m;
    std::vector<particle_info_t> particle_info_m;

private:
    void update_vertex_object();
};

} // namespace mau
