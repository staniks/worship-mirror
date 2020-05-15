#include "worship/gameplay/entities/pickup.hpp"

#include "worship/gameplay/world/world.hpp"

namespace mau {

pickup_t::pickup_t(world_t& world, glm::vec3 position, pickup_descriptor_t descriptor) :
    sprite_entity_t(world, {position.x, descriptor.bounding_box.y / 2, position.z}),
    descriptor_m(descriptor),
    texture_diffuse_m(descriptor.texture_diffuse),
    texture_emission_m(descriptor.texture_emission),
    on_pickup(descriptor.callback)
{
    bounding_box_m = descriptor.bounding_box;
    sprite_scale_m = descriptor.sprite_scale;

    set_collision_layer(collision_layers::pickup_k);
    set_collision_mask(collision_layers::player_k);

    if(descriptor.light)
        light_m = world_m.add_dynamic_light(dynamic_light_t{this->position(), *descriptor.light, 1.5f, false});
}
void pickup_t::fixed_update(float delta_time)
{
}
void pickup_t::variable_update(float delta_time)
{
    if(light_m)
        light_m->position = position();
}
texture_handle_t pickup_t::sprite_texture_diffuse()
{
    return texture_diffuse_m;
}

texture_handle_t pickup_t::sprite_texture_emission()
{
    return texture_emission_m;
}

void pickup_t::on_collision(entity_t& entity)
{
    // Collision layers ensure the player is the only entity we can collide with, so this is safe.
    if(on_pickup(dynamic_cast<player_t&>(entity), world_m.event_callback()))
    {
        destroyed_m = true;

        if(light_m)
            light_m->destroyed = true;

        world_m.event_callback().flash_screen(screen_flash_t::pickup_k);
        world_m.play_sound(descriptor_m.sound);
    }
}

} // namespace mau
