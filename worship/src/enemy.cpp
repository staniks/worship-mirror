#include "worship/gameplay/entities/enemy.hpp"

#include "worship/gameplay/entities/projectile.hpp"
#include "worship/gameplay/world/world.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

#include <fmt/format.h>

#include <cmath>

static constexpr float enemy_bounding_height_k = 0.75f;

namespace mau {
enemy_t::enemy_t(world_t& world, glm::vec3 position, const enemy_descriptor_t& descriptor) :
    sprite_entity_t(world, {position.x, enemy_bounding_height_k / 2.0f, position.z}), descriptor_m(descriptor)
{
    bounding_box_m = {0.25f, enemy_bounding_height_k, 0.25f};
    sprite_scale_m = bounding_box_m.y;
    set_blocking(true);

    health_m        = descriptor_m.health;
    fire_range_m    = descriptor_m.fire_range;
    fire_duration_m = descriptor_m.fire_duration;

    set_collision_mask(collision_layers::player_k);
    set_collision_layer(collision_layers::enemy_k);
}
void enemy_t::fixed_update(float delta_time)
{
    if(!dead_m)
    {
        glm::vec3 player_vector = world_m.player()->position() - position();
        glm::vec3 player_normal = glm::normalize(player_vector);
        player_normal.y         = 0;
        float player_distance   = glm::length(player_vector);

        switch(state_m)
        {
            case enemy_state_t::unaware_k: {
                if(state_timer_m >= unaware_interval_k)
                {
                    state_timer_m = 0.0f;

                    // Raycast and acquire player.
                    entity_t* player{nullptr};
                    world_m.raycast(position(), player_normal, collision_layers::player_k, nullptr, &player);

                    if (player)
                    {
                        world_m.engine().audio().play_clip(descriptor_m.detect_sound, position());
                        state_m = enemy_state_t::acquiring_player_k;
                    }
                }
                else
                    state_timer_m += delta_time;
            }
            break;
            case enemy_state_t::acquiring_player_k: {
                if(state_timer_m >= acquiring_player_interval_k)
                {
                    state_timer_m = 0.0f;

                    // Raycast for the player.
                    entity_t* entity_hit{nullptr};
                    world_m.raycast(
                        position(), player_normal, collision_layers::player_k | collision_layers::enemy_k, this, &entity_hit);

                    player_t* player = dynamic_cast<player_t*>(entity_hit);

                    // If nothing is obstructing the player and they are alive, fire.
                    if(player && !player->dead())
                    {
                        if(player_distance < fire_range_m)
                        {
                            state_m       = enemy_state_t::shooting_k;
                            orientation_m = glm::quatLookAt(player_normal, {0, 1, 0});
                        }
                        else
                        {
                            desired_direction_m = player_normal;
                            state_m             = enemy_state_t::moving_k;
                            change_animation(enemy_descriptor_t::animation_t::walking_k);
                        }
                    }
                    else
                    // If player obstructed or dead, move somewhere.
                    {
                        desired_direction_m   = glm::sphericalRand(1.0f);
                        desired_direction_m.y = 0;

                        state_m = enemy_state_t::moving_k;
                        change_animation(enemy_descriptor_t::animation_t::walking_k);
                    }
                }
                else
                    state_timer_m += delta_time;
            }
            break;
            case enemy_state_t::moving_k: {
                if(state_timer_m >= move_duration_k)
                {
                    state_timer_m = 0.0f;
                    state_m       = enemy_state_t::acquiring_player_k;
                    velocity_m    = {};
                }
                else
                {
                    state_timer_m += delta_time;
                    velocity_m    = desired_direction_m;
                    orientation_m = glm::quatLookAt(glm::normalize(desired_direction_m), {0, 1, 0});
                }
            }
            break;
            case enemy_state_t::shooting_k: {
                orientation_m = glm::quatLookAt(glm::normalize(player_normal), {0, 1, 0});
                if(state_timer_m >= fire_duration_m)
                {
                    state_timer_m = 0.0f;
                    state_m       = enemy_state_t::acquiring_player_k;
                    change_animation(enemy_descriptor_t::animation_t::shooting_k);

                    auto projectile =
                        std::make_unique<projectile_t>(world_m,
                                                       position() + glm::vec3{0.0f, 0.1f, 0.0f},
                                                       orientation(),
                                                       projectile_owner_t::monster_k,
                                                       world_m.projectile_descriptors().at(descriptor_m.projectile_type));
                    world_m.add_entity(std::move(projectile));

                    world_m.engine().audio().play_clip(descriptor_m.attack_sound);
                }
                else
                    state_timer_m += delta_time;
            }
            break;
            default: break;
        }
    }

    entity_t::fixed_update(delta_time);
}
void enemy_t::variable_update(float delta_time)
{
    current_frame_m += delta_time * 10;
    if(current_frame_m >= descriptor_m.animation_frame_indices.at(animation_m).size())
    {
        if(!dead_m)
            current_frame_m = 0.0f;
        else
            current_frame_m = descriptor_m.animation_frame_indices.at(animation_m).size() - 1;
    }

    glm::vec3 player_vector = world_m.player()->position() - position();
    player_vector.y         = 0;
    player_vector           = glm::normalize(player_vector);

    glm::vec3 sprite_vector = direction();
    float     angle1        = std::atan2(player_vector.z, player_vector.x) + glm::pi<float>();
    float     angle2        = std::atan2(sprite_vector.z, sprite_vector.x) + glm::pi<float>();

    // Adjust for half a rotation for animation centering.
    float angle = angle1 - angle2 + glm::pi<float>() / descriptor_m.frames.size();
    if(angle < 0)
        angle += glm::two_pi<float>();

    current_direction_index_m =
        (int32_t)((angle / glm::two_pi<float>()) * descriptor_m.frames.size()) % descriptor_m.frames.size();

    entity_t::variable_update(delta_time);
}
texture_handle_t enemy_t::sprite_texture_diffuse()
{
    auto current_frame_index = descriptor_m.animation_frame_indices.at(animation_m)[(size_t)current_frame_m];
    return descriptor_m.frames.at(current_direction_index_m).at(current_frame_index).texture_diffuse;
}
texture_handle_t enemy_t::sprite_texture_emission()
{
    auto current_frame_index = descriptor_m.animation_frame_indices.at(animation_m)[(size_t)current_frame_m];
    return descriptor_m.frames.at(current_direction_index_m).at(current_frame_index).texture_emission;
}
void enemy_t::on_collision(entity_t& entity)
{
    entity_t::on_collision(entity);
}
float enemy_t::health() const
{
    return health_m;
}
void enemy_t::take_damage(float damage)
{
    health_m -= damage;
    if(health_m <= 0.0f)
    {
        die();
    }
    else
        world_m.engine().audio().play_clip(descriptor_m.hurt_sound, position());
}
void enemy_t::change_animation(enemy_descriptor_t::animation_t animation)
{
    animation_m     = animation;
    current_frame_m = 0.0f;
}
void enemy_t::die()
{
    if(dead_m)
        return;

    dead_m = true;
    velocity_m = {};
    set_collision_layer(0);
    set_collision_mask(0);
    change_animation(enemy_descriptor_t::animation_t::dying_k);

    world_m.engine().audio().play_clip(descriptor_m.death_sound, position());
}
} // namespace mau
