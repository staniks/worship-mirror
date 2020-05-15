#pragma once

#include "worship/gameplay/sprite_entity.hpp"

#include "worship/gameplay/entity_descriptor.hpp"

#include <mau/base/timer.hpp>

namespace mau {

struct dynamic_light_t;

enum class projectile_owner_t
{
    player_k,
    monster_k
};

class projectile_t : public sprite_entity_t
{
public:
    projectile_t(world_t&                world,
                 glm::vec3               position,
                 glm::quat               orientation,
                 projectile_owner_t      owner,
                 projectile_descriptor_t descriptor);

    void fixed_update(float delta_time) override;
    void variable_update(float delta_time) override;

    texture_handle_t sprite_texture_diffuse() override;
    texture_handle_t sprite_texture_emission() override;

    void on_collision(entity_t& entity) override;
    void on_wall_collision() override;

    void destroy() override;

protected:
    projectile_owner_t owner_m;

    texture_handle_t texture_diffuse_m;
    texture_handle_t texture_emission_m;

    std::optional<glm::vec3> explosion_light_m;
    particle_type_t          explosion_particle_m;

    float life_m;
    float sprite_rotation_speed_m;

    audio_clip_handle_t death_sound_m;
    audio_clip_handle_t bounce_sound_m;

    float damage_m;

    timer_t bounce_sound_timer_m;

    dynamic_light_t* light_m{nullptr};
};

} // namespace mau
