#pragma once

#include "worship/gameplay/entity.hpp"
#include "worship/gameplay/world/world_info.hpp"

#include <mau/audio/audio_clip.hpp>

#include <frozen/map.h>

#include <functional>
#include <map>
#include <optional>

namespace mau {

// Forward declarations.
class player_t;
class world_t;

enum class weapon_type_t
{
    shotgun_k,
    grenade_launcher_k,
    plasma_rifle_k
};

enum class ammo_type_t
{
    shells_k,
    grenades_k,
    cells_k
};

enum class enemy_type_t
{
    light_k,
    medium_k,
    heavy_k
};

namespace detail {
inline static constexpr std::pair<ammo_type_t, int64_t> max_ammo_list_k[] = {
    {ammo_type_t::shells_k, 100},
    {ammo_type_t::grenades_k, 50},
    {ammo_type_t::cells_k, 200}
};

inline static constexpr auto max_ammo_type_mapping_k = frozen::make_map(detail::max_ammo_list_k);
} // namespace detail

inline static constexpr int64_t max_ammo_for_type(ammo_type_t type)
{
    return detail::max_ammo_type_mapping_k.at(type);
}

enum class projectile_type_t
{
    grenade_k,
    enemy_plasma_k,
    enemy_plasma_medium_k,
    enemy_plasma_heavy_k,
    player_plasma_k
};

enum class particle_type_t
{
    bullet_hit_k,
    explosion_k,
    blood_splash_k
};

using pickup_callback_t = std::function<bool(player_t&, event_callback_t& event_callback)>;
struct pickup_descriptor_t
{
    texture_handle_t         texture_diffuse;  // Pickup diffuse texture.
    texture_handle_t         texture_emission; // Pickup emission texture.
    float                    sprite_scale;     // Sprite scale factor.
    glm::vec3                bounding_box;     // Bounding box size.
    pickup_callback_t        callback;         // Handle pickup event. Return true if picked up, false otherwise.
    std::optional<glm::vec3> light;            // Light color, if any.
    audio_clip_handle_t      sound;            // Pickup sound.
};

using weapon_fire_callback_t = std::function<void(world_t&)>;
struct weapon_descriptor_t
{
    struct animation_frame_t
    {
        texture_handle_t    texture_diffuse;  // Diffuse texture.
        texture_handle_t    texture_emission; // Emission texture.
        float               duration;         // Seconds.
        audio_clip_handle_t sound;            // Clip played when this frame is reached.
    };

    ammo_type_t                    ammo_type;            // Ammo type which the weapon consumes.
    uint32_t                       ammo_on_first_pickup; // Ammo yield on first pickup.
    uint32_t                       ammo_per_pickup;      // Ammo on each consecutive pickup after first.
    uint8_t                        slot;                 // Weapon slot number.
    std::vector<animation_frame_t> frames;               // Weapon animation frames. Firing rate is this combined.
    uint64_t                       inactive_frame;       // Index of frame which is displayed when weapon is empty.
    weapon_fire_callback_t         fire_callback;        // Fire event callback.
    std::optional<glm::vec3>       muzzle_flash_light;   // Muzzle flash light color.
};

struct projectile_descriptor_t
{
    texture_handle_t         texture_diffuse;       // Projectile diffuse texture.
    texture_handle_t         texture_emission;      // Projectile emission texture.
    float                    sprite_scale;          // Sprite scale multiplier.
    float                    sprite_rotation_speed; // Sprite rotation speed.
    float                    damage;                // Damage it does to enemies. Or the player.
    float                    lifetime;              // Lifetime in seconds.
    float                    speed;                 // Initial movement speed.
    float                    friction;              // Air dampening.
    float                    bounciness;            // Bounciness.
    glm::vec3                bounding_box;          // Bounding box size.
    bool                     gravity;               // Affected by gravity.
    std::optional<glm::vec3> light;                 // Light color, if any.
    std::optional<glm::vec3> explosion_light;       // Light emitted on explosion.
    particle_type_t          explosion_particle;    // Explosion particle.
    audio_clip_handle_t      death_sound;           // Played when projectile is destroyed.
    audio_clip_handle_t      bounce_sound;          // Played when projectile bounces off level geometry.
};

struct particle_descriptor_t
{
    texture_handle_t   texture_diffuse;  // Diffuse texture.
    texture_handle_t   texture_emission; // Emission texture.
    float              particle_size;    // Scale multiplier.
    range_t<uint16_t>  count;            // How many particles, min and max.
    range_t<glm::vec4> color;            // Color range.
    float              radius;           // Spawn radius.
    range_t<float>     lifetime;         // Lifetime range.
    range_t<float>     velocity;         // Velocity range.
    glm::vec3          gravity;          // Gravity vector.
};

struct enemy_descriptor_t
{
    struct animation_frame_t
    {
        texture_handle_t texture_diffuse;
        texture_handle_t texture_emission;
    };

    enum class animation_t
    {
        walking_k,
        shooting_k,
        dying_k
    };

    std::vector<std::vector<animation_frame_t>>  frames;                  // Animation frames.
    std::map<animation_t, std::vector<uint32_t>> animation_frame_indices; // Animation and frame mapping.

    float fire_range;
    float health;
    float fire_duration;

    projectile_type_t   projectile_type; // Projectile the enemy fires.
    audio_clip_handle_t attack_sound;    // Played when firing projectile.
    audio_clip_handle_t hurt_sound;      // Played when damaged (but not dying).
    audio_clip_handle_t detect_sound;    // Played when player enters line of sight.
    audio_clip_handle_t death_sound;     // Played upon death.
};

std::map<object_type_t, pickup_descriptor_t>         create_pickup_descriptors(engine_context_t& engine);
std::map<weapon_type_t, weapon_descriptor_t>         create_weapon_descriptors(engine_context_t& engine);
std::map<projectile_type_t, projectile_descriptor_t> create_projectile_descriptors(engine_context_t& engine);
std::map<particle_type_t, particle_descriptor_t>     create_particle_descriptors(engine_context_t& engine);
std::map<ammo_type_t, texture_handle_t>              create_ammo_icons(engine_context_t& engine);
std::map<enemy_type_t, enemy_descriptor_t>           create_enemy_descriptors(engine_context_t& engine);

} // namespace mau
