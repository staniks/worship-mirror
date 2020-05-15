#pragma once

#include "worship/gameplay/entity.hpp"

namespace mau {

class sprite_entity_t : public entity_t
{
public:
    sprite_entity_t(world_t& world, glm::vec3 position);
    virtual ~sprite_entity_t();

    void render(renderer_t& renderer, shader_handle_t ubershader, float delta_time) override;

    virtual texture_handle_t sprite_texture_diffuse();
    virtual texture_handle_t sprite_texture_emission();
    float                    sprite_scale();
    float                    sprite_rotation();

protected:
    float sprite_rotation_m{0.0f};
    float sprite_scale_m{1.0f};
};

} // namespace mau
