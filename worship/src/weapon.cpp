#include "worship/gameplay/weapon.hpp"

#include "worship/gameplay/entities/player.hpp"
#include "worship/gameplay/entities/projectile.hpp"
#include "worship/gameplay/world/world.hpp"

namespace mau {
weapon_t::weapon_t(world_t& world, player_t& player, weapon_descriptor_t descriptor) :
    world_m(world), player_m(player), descriptor_m(descriptor)
{
    for(auto& frame: descriptor.frames)
    {
        reload_duration_m += frame.duration;
    }
}
void weapon_t::fixed_update(float delta_time)
{
    reload_progress_m = glm::clamp(reload_progress_m - delta_time, 0.0f, reload_duration_m);
}
void weapon_t::variable_update(float delta_time)
{
    last_frame_m = current_frame_m;

    if (player_m.ammo(descriptor_m.ammo_type) == 0)
    {
        current_frame_m = descriptor_m.inactive_frame;
        if (current_frame_m != last_frame_m)
        {
            auto& frame = descriptor_m.frames[descriptor_m.inactive_frame];
            world_m.play_sound(frame.sound);
        }
    }
    else
    {
        float inverted_progress = reload_duration_m - reload_progress_m;
        if (inverted_progress == reload_duration_m)
            inverted_progress = 0.0f;

        float it_frame_time{ 0.0f };
        for (size_t i = 0; i < descriptor_m.frames.size(); ++i)
        {
            auto& frame = descriptor_m.frames[i];
            it_frame_time += frame.duration;

            if (inverted_progress > it_frame_time)
                continue;

            current_frame_m = i;

            if (current_frame_m != last_frame_m)
            {
                world_m.play_sound(frame.sound);
            }

            break;
        }
    }
}
void weapon_t::fire()
{
    if(reload_progress_m == 0.0f && player_m.ammo(descriptor_m.ammo_type))
    {
        reload_progress_m = reload_duration_m;

        if(descriptor_m.muzzle_flash_light)
            world_m.add_dynamic_light(dynamic_light_t{player_m.position(), *descriptor_m.muzzle_flash_light, 3, true});

        descriptor_m.fire_callback(world_m);

        player_m.consume_ammo(descriptor_m.ammo_type);
    }
}
texture_handle_t weapon_t::texture_diffuse()
{
    return descriptor_m.frames.at(current_frame_m).texture_diffuse;
}
texture_handle_t weapon_t::texture_emission()
{
    return descriptor_m.frames.at(current_frame_m).texture_emission;
}
weapon_descriptor_t& weapon_t::descriptor()
{
    return descriptor_m;
}
float weapon_t::knockback_factor() const
{
    return reload_progress_m / reload_duration_m;
}
} // namespace mau
