#include "worship/gameplay/entities/player.hpp"

#include "worship/gameplay/world/world.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

static float max_health_k = 100.0f;
static float max_armor_k  = 100.0f;

player_t::player_t(world_t& world, glm::vec3 position, difficulty_t difficulty) :
    entity_t(world, position), move_acceleration_m(50.0f), difficulty_m(difficulty)
{
    set_friction(0.2f);
    gravity_m      = false;
    bounding_box_m = {0.5f, 0.99f, 0.5f};
    blocking_m     = true;

    set_collision_mask(collision_layers::enemy_k);
    set_collision_layer(collision_layers::player_k);

    health_m = 100;
    armor_m  = 0;

    grunts_m.push_back(world.engine().resource_cache().resource<audio_clip_t>("sounds/modified/grunt1.wav"));
    grunts_m.push_back(world.engine().resource_cache().resource<audio_clip_t>("sounds/modified/grunt2.wav"));
    grunts_m.push_back(world.engine().resource_cache().resource<audio_clip_t>("sounds/modified/grunt3.wav"));
    death_sound_m = world.engine().resource_cache().resource<audio_clip_t>("sounds/modified/death.wav");
}

void player_t::fixed_update(float delta_time)
{
    static constexpr float weapon_switch_speed = 4.0f;

    if(!dead_m)
    {
        static constexpr float head_bob_speed_k   = 4.0f;
        static constexpr float weapon_bob_speed_k = 2.0f;

        head_bob_m = std::remainder(head_bob_m + delta_time * head_bob_speed_k * glm::length(velocity_m), glm::two_pi<float>());
        weapon_bob_m =
            std::remainder(weapon_bob_m + delta_time * weapon_bob_speed_k * glm::length(velocity_m), glm::two_pi<float>());

        weapon_t* weapon = current_weapon();
        if(weapon)
            weapon->fixed_update(delta_time);

        if(switching_weapons_m != weapon_switch_t::none_k)
        {
            switching_weapons_progress_m =
                glm::clamp(switching_weapons_progress_m + delta_time * weapon_switch_speed, 0.0f, 1.0f);

            if(switching_weapons_progress_m >= 1.0f)
            {
                if(switching_weapons_m == weapon_switch_t::lowering_k)
                {
                    if(!dead_m)
                        switching_weapons_m = weapon_switch_t::raising_k;
                    else
                        switching_weapons_m = weapon_switch_t::none_k;

                    switching_weapons_progress_m = 0.0f;
                    current_weapon_m             = selected_weapon_m;
                }
                else
                {
                    switching_weapons_m = weapon_switch_t::none_k;
                }
            }
        }
    }
    else
    {
        head_bob_m = 0.0f;
        if(position().y > 0.2f)
            set_position(position() - glm::vec3{0, 1, 0} * delta_time * 0.5f);

        switching_weapons_progress_m = glm::clamp(switching_weapons_progress_m + delta_time * weapon_switch_speed, 0.0f, 1.0f);
    }

    entity_t::fixed_update(delta_time);
}

void player_t::variable_update(float delta_time)
{
    if(!dead_m)
    {
        weapon_t* weapon = current_weapon();
        if(weapon)
        {
            weapon->variable_update(delta_time);
        }
    }

    entity_t::variable_update(delta_time);
}

void player_t::fire()
{
    if(dead_m)
        return;

    weapon_t* weapon = current_weapon();
    if(weapon && switching_weapons_m == weapon_switch_t::none_k)
    {
        weapon->fire();
    }
}

void player_t::walk(glm::vec3 direction, float delta_time)
{
    if(dead_m)
        return;

    apply_force(direction * move_acceleration_m, delta_time);
}

glm::vec3 player_t::camera_position() const
{
    return position() + glm::vec3{0, 1, 0} * head_bob();
}

float player_t::move_acceleration() const
{
    return move_acceleration_m;
}

float player_t::head_bob() const
{
    static constexpr float head_bob_amplitude_k = 0.05f;
    return std::sin(head_bob_m) * head_bob_amplitude_k;
}

glm::vec2 player_t::weapon_bob() const
{
    static constexpr float amplitude        = 8;
    static constexpr float switch_amplitude = 128;

    float bob = std::sin(weapon_bob_m);

    float switch_offset = 0.0f;
    if(switching_weapons_m == weapon_switch_t::raising_k)
        switch_offset = 1.0f - switching_weapons_progress_m;
    else if(switching_weapons_m == weapon_switch_t::lowering_k)
        switch_offset = switching_weapons_progress_m;

    return glm::vec2(bob * amplitude, std::abs(bob * amplitude) + switch_offset * switch_amplitude);
}

float player_t::fov() const
{
    // TODO: maybe add FOV slider in options
    return glm::radians(90.0f);
}

float player_t::health() const
{
    return health_m;
}

float player_t::armor() const
{
    return armor_m;
}

uint32_t player_t::ammo()
{
    weapon_t* weapon = current_weapon();
    if(weapon)
    {
        return (uint32_t)glm::max((int64_t)0LL, ammo_m[weapon->descriptor().ammo_type]);
    }

    return 0;
}

bool player_t::dead() const
{
    return dead_m;
}

bool player_t::add_weapon(weapon_type_t weapon_type)
{
    auto& weapon_descriptor = world_m.weapon_descriptors().at(weapon_type);

    auto it = weapons_m.find(weapon_type);
    if(it != weapons_m.end())
    {
        return add_ammo(weapon_descriptor.ammo_type, weapon_descriptor.ammo_per_pickup);
    }

    weapons_m[weapon_type] = std::make_unique<weapon_t>(world_m, *this, weapon_descriptor);
    add_ammo(weapon_descriptor.ammo_type, weapon_descriptor.ammo_on_first_pickup);
    select_weapon_slot(weapon_descriptor.slot);
    return true;
}

void player_t::select_weapon_slot(uint8_t slot)
{
    bool weapon_exists{false};
    for(auto& [type, weapon]: weapons_m)
    {
        if(weapon->descriptor().slot == slot)
        {
            weapon_exists = true;
            break;
        }
    }

    if(!weapon_exists)
        return;

    selected_weapon_m = slot;

    if(selected_weapon_m != current_weapon_m)
    {
        switching_weapons_m          = weapon_switch_t::lowering_k;
        switching_weapons_progress_m = 0.0f;
    }
}

bool player_t::add_health(float amount)
{
    if(health_m < max_health_k)
    {
        health_m = glm::min(health_m + amount, max_health_k);
        return true;
    }
    return false;
}

bool player_t::add_armor(float amount)
{
    if(armor_m < max_armor_k)
    {
        armor_m = glm::min(armor_m + amount, max_armor_k);
        return true;
    }
    return false;
}

bool player_t::add_ammo(ammo_type_t type, int64_t amount)
{
    auto it = ammo_m.find(type);
    if(it == ammo_m.end())
    {
        ammo_m.insert({type, amount});
        return true;
    }

    auto& current_ammo = it->second;
    auto  max_ammo     = max_ammo_for_type(type);
    if(current_ammo < max_ammo)
    {
        current_ammo = glm::min(current_ammo + amount, max_ammo);
        return true;
    }
    return false;
}

int64_t player_t::ammo(ammo_type_t type)
{
    return ammo_m[type];
}

void player_t::consume_ammo(ammo_type_t type)
{
    auto& ammo = ammo_m[type];
    ammo       = glm::max((int64_t)0LL, ammo - 1);
}

weapon_t* player_t::current_weapon()
{
    for(auto& [type, weapon]: weapons_m)
    {
        if(weapon->descriptor().slot == current_weapon_m)
            return weapon.get();
    }
    return nullptr;
}

void player_t::take_damage(float damage)
{
    float factor = 1.0f;
    if(difficulty_m == difficulty_t::easy_k)
        factor = 0.5f;
    else if(difficulty_m == difficulty_t::hard_k)
        factor = 2.0f;

    if(armor_m > 0)
    {
        armor_m  = glm::clamp(armor_m - damage * factor, 0.0f, max_armor_k);
        health_m = glm::clamp(health_m - damage * factor / 2.0f, 0.0f, max_health_k);
    }
    else
    {
        health_m = glm::clamp(health_m - damage * factor, 0.0f, max_health_k);
    }
    world_m.event_callback().flash_screen(screen_flash_t::pain_k);

    if(health_m <= 0.0f)
    {
        die();
    }
    else
    {
        world_m.engine().audio().play_clip(grunts_m[rand() % grunts_m.size()]);
    }
}

void player_t::die()
{
    if(dead_m)
        return;

    velocity_m = {};
    dead_m     = true;
    world_m.event_callback().display_message(message_t::death_k);
    switching_weapons_m          = weapon_switch_t::lowering_k;
    switching_weapons_progress_m = 0.0f;

    world_m.engine().audio().play_clip(death_sound_m);
}

} // namespace mau
