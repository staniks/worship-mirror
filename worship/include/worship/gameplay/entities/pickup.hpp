#pragma once

#include "worship/gameplay/sprite_entity.hpp"

#include "worship/gameplay/entity_descriptor.hpp"

namespace mau {

struct dynamic_light_t;

class pickup_t : public sprite_entity_t
{
public:
    pickup_t(world_t& world, glm::vec3 position, pickup_descriptor_t descriptor);

    void fixed_update(float delta_time) override;
    void variable_update(float delta_time) override;

    texture_handle_t sprite_texture_diffuse() override;
    texture_handle_t sprite_texture_emission() override;

    void on_collision(entity_t& entity) override;

private:
    pickup_descriptor_t descriptor_m;
    texture_handle_t    texture_diffuse_m;
    texture_handle_t    texture_emission_m;

    pickup_callback_t on_pickup;

    dynamic_light_t* light_m{nullptr};
};

} // namespace mau
