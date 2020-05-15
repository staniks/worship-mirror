#include "worship/gameplay/entities/projectile.hpp"

#include "worship/gameplay/entities/enemy.hpp"
#include "worship/gameplay/particle_entity.hpp"
#include "worship/gameplay/world/world.hpp"

namespace mau {
projectile_t::projectile_t(world_t&                world,
                           glm::vec3               position,
                           glm::quat               orientation,
                           projectile_owner_t      owner,
                           projectile_descriptor_t descriptor) :
    sprite_entity_t(world, position),
    owner_m(owner),
    explosion_particle_m(descriptor.explosion_particle),
    explosion_light_m(descriptor.explosion_light)
{
    orientation_m           = orientation;
    velocity_m              = orientation * glm::vec3(0, 0, -descriptor.speed);
    friction_m              = descriptor.friction;
    bounding_box_m          = descriptor.bounding_box;
    bounciness_m            = descriptor.bounciness;
    gravity_m               = descriptor.gravity;
    life_m                  = descriptor.lifetime;
    sprite_scale_m          = descriptor.sprite_scale;
    sprite_rotation_speed_m = descriptor.sprite_rotation_speed;
    death_sound_m           = descriptor.death_sound;
    bounce_sound_m          = descriptor.bounce_sound;
    damage_m                = descriptor.damage;

    texture_diffuse_m  = descriptor.texture_diffuse;
    texture_emission_m = descriptor.texture_emission;

    set_collision_mask(owner == projectile_owner_t::player_k ? collision_layers::enemy_k : collision_layers::player_k);
    set_collision_layer(collision_layers::projectile_k);

    if(descriptor.light)
        light_m = world_m.add_dynamic_light(dynamic_light_t{this->position(), *descriptor.light, 1.5f, false});
}
void projectile_t::fixed_update(float delta_time)
{
    life_m -= delta_time;
    if(life_m <= 0.0f)
        destroy();

    entity_t::fixed_update(delta_time);
}
void projectile_t::variable_update(float delta_time)
{
    if(light_m)
        light_m->position = position();

    sprite_rotation_m += sprite_rotation_speed_m * delta_time;

    entity_t::variable_update(delta_time);
}
texture_handle_t projectile_t::sprite_texture_diffuse()
{
    return texture_diffuse_m;
}
texture_handle_t projectile_t::sprite_texture_emission()
{
    return texture_emission_m;
}
void projectile_t::on_collision(entity_t& entity)
{
    player_t* player = dynamic_cast<player_t*>(&entity);
    enemy_t*  enemy  = dynamic_cast<enemy_t*>(&entity);

    if(player && owner_m == projectile_owner_t::monster_k)
    {
        player->take_damage(damage_m);
        destroy();
    }
    else if(enemy && owner_m == projectile_owner_t::player_k)
    {
        enemy->take_damage(damage_m);
        destroy();
    }
}
void projectile_t::on_wall_collision()
{
    // Projectiles which don't bounce are destroyed upon wall collision.
    if(bounciness_m == 0.0f)
        destroy();
    else
    {
        if(bounce_sound_timer_m.milliseconds() > 100)
        {
            // TODO: attenuate with how much it bounced
            // world_m.play_sound_at(bounce_sound_m, position());
            bounce_sound_timer_m.reset();
        }
    }
}
void projectile_t::destroy()
{
    if(destroyed_m)
        return;

    destroyed_m = true;

    if(light_m)
        light_m->destroyed = true;

    if(explosion_light_m)
        world_m.add_dynamic_light(dynamic_light_t{this->position(), *explosion_light_m, 4.0f, true});

    auto particle =
        std::make_unique<particle_entity_t>(world_m, position(), world_m.particle_descriptors().at(explosion_particle_m));
    world_m.add_entity(std::move(particle));

    world_m.play_sound_at(death_sound_m, position());
}

} // namespace mau
