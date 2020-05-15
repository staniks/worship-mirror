#pragma once

#include "worship/gameplay/entity_descriptor.hpp"

#include <mau/math/quaternion.hpp>
#include <mau/math/vector.hpp>
#include <mau/rendering/texture.hpp>

namespace mau {

class weapon_t
{
public:
    weapon_t(world_t& world, player_t& player, weapon_descriptor_t descriptor);

    void fixed_update(float delta_time);
    void variable_update(float delta_time);

    void fire();

    texture_handle_t texture_diffuse();  // Returns the current frame.
    texture_handle_t texture_emission(); // Returns the current frame emission.

    weapon_descriptor_t& descriptor();

    float knockback_factor() const;

private:
    world_t&            world_m;
    player_t&           player_m;
    weapon_descriptor_t descriptor_m;

    size_t current_frame_m{0};
    size_t last_frame_m{0};

    float reload_duration_m{0.0f};
    float reload_progress_m{0.0f};
};

} // namespace mau
