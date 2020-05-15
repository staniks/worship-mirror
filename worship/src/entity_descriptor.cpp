#include "worship/gameplay/entity_descriptor.hpp"

#include "worship/gameplay/entities/enemy.hpp"
#include "worship/gameplay/entities/player.hpp"
#include "worship/gameplay/entities/projectile.hpp"
#include "worship/gameplay/particle_entity.hpp"
#include "worship/gameplay/world/world.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/io/resource_cache.hpp"

#include <fmt/format.h>

namespace mau {

std::map<object_type_t, pickup_descriptor_t> create_pickup_descriptors(engine_context_t& engine)
{
    std::map<object_type_t, pickup_descriptor_t> descriptors;

    /////////////////////////////////////////////////////////
    // ARMOR
    ////////////////////////////////////////////////////////
    pickup_descriptor_t armor{};
    armor.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/armor-diffuse-0000.tex");
    armor.texture_emission = engine.resource_cache().resource<texture_t>("white.tex");
    armor.sprite_scale     = 0.45f;
    armor.bounding_box     = glm::vec3{0.25f, 0.35f, 0.25f};
    armor.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_armor(50))
        {
            event_callback.display_message(message_t::pickup_armor_k);
            return true;
        }
        return false;
    };
    armor.light                         = glm::vec3{0.1f, 0.5f, 0.1f};
    armor.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/armor.wav");
    descriptors[object_type_t::armor_k] = armor;

    /////////////////////////////////////////////////////////
    // HEALTH
    ////////////////////////////////////////////////////////
    pickup_descriptor_t health{};
    health.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/health-diffuse-0000.tex");
    health.texture_emission = engine.resource_cache().resource<texture_t>("pickups/health-emission-0000.tex");
    health.sprite_scale     = 0.25f;
    health.bounding_box     = glm::vec3{0.25f, 0.25f, 0.25f};
    health.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_health(10))
        {
            event_callback.display_message(message_t::pickup_health_k);
            return true;
        }
        return false;
    };
    health.light                         = glm::vec3{0.5f, 0.1f, 0.1f};
    health.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/health.wav");
    descriptors[object_type_t::health_k] = health;

    /////////////////////////////////////////////////////////
    // SHELLS
    ////////////////////////////////////////////////////////
    pickup_descriptor_t shells{};
    shells.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/shells-diffuse-0000.tex");
    shells.texture_emission = engine.resource_cache().resource<texture_t>("pickups/shells-emission-0000.tex");
    shells.sprite_scale     = 0.25f;
    shells.bounding_box     = glm::vec3{0.25f, 0.25f, 0.25f};
    shells.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_ammo(ammo_type_t::shells_k, 3))
        {
            event_callback.display_message(message_t::pickup_shells_k);
            return true;
        }
        return false;
    };
    shells.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/ammo-pickup.wav");
    descriptors[object_type_t::shells_k] = shells;

    /////////////////////////////////////////////////////////
    // CELLS
    ////////////////////////////////////////////////////////
    pickup_descriptor_t cells{};
    cells.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/cells-diffuse-0000.tex");
    cells.texture_emission = engine.resource_cache().resource<texture_t>("pickups/cells-emission-0000.tex");
    cells.sprite_scale     = 0.25f;
    cells.bounding_box     = glm::vec3{0.25f, 0.25f, 0.25f};
    cells.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_ammo(ammo_type_t::cells_k, 10))
        {
            event_callback.display_message(message_t::pickup_cells_k);
            return true;
        }
        return false;
    };
    cells.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/ammo-pickup.wav");
    cells.light = glm::vec3{ 0.1f, 0.2f, 0.5f };
    descriptors[object_type_t::cells_k] = cells;

    /////////////////////////////////////////////////////////
    // GRENADES
    ////////////////////////////////////////////////////////
    pickup_descriptor_t grenades{};
    grenades.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/grenades-diffuse-0000.tex");
    grenades.texture_emission = engine.resource_cache().resource<texture_t>("pickups/grenades-emission-0000.tex");
    grenades.sprite_scale     = 0.4f;
    grenades.bounding_box     = glm::vec3{0.25f, 0.4f, 0.25f};
    grenades.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_ammo(ammo_type_t::grenades_k, 5))
        {
            event_callback.display_message(message_t::pickup_grenades_k);
            return true;
        }
        return false;
    };
    grenades.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/ammo-pickup.wav");
    grenades.light = glm::vec3{ 0.5f, 0.1f, 0.1f };
    descriptors[object_type_t::grenades_k] = grenades;

    /////////////////////////////////////////////////////////
    // SHOTGUN
    ////////////////////////////////////////////////////////
    pickup_descriptor_t shotgun{};
    shotgun.texture_diffuse  = engine.resource_cache().resource<texture_t>("pickups/shotgun-pickup-diffuse-0000.tex");
    shotgun.texture_emission = engine.resource_cache().resource<texture_t>("pickups/shotgun-pickup-emission-0000.tex");
    shotgun.sprite_scale     = 0.75f;
    shotgun.bounding_box     = glm::vec3{0.25f, 0.6f, 0.25f};
    shotgun.callback         = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_weapon(weapon_type_t::shotgun_k))
        {
            event_callback.display_message(message_t::pickup_shotgun_k);
            return true;
        }
        return false;
    };
    shotgun.sound                         = engine.resource_cache().resource<audio_clip_t>("sounds/modified/weapon-pickup.wav");
    descriptors[object_type_t::shotgun_k] = shotgun;

    /////////////////////////////////////////////////////////
    // PLASMA RIFLE
    ////////////////////////////////////////////////////////
    pickup_descriptor_t plasma_rifle{};
    plasma_rifle.texture_diffuse = engine.resource_cache().resource<texture_t>("pickups/plasma-rifle-pickup-diffuse-0000.tex");
    plasma_rifle.texture_emission =
        engine.resource_cache().resource<texture_t>("pickups/plasma-rifle-pickup-emission-0000.tex");
    plasma_rifle.sprite_scale = 0.75f;
    plasma_rifle.bounding_box = glm::vec3{0.25f, 0.75f, 0.25f};
    plasma_rifle.callback     = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_weapon(weapon_type_t::plasma_rifle_k))
        {
            event_callback.display_message(message_t::pickup_plasma_rifle_k);
            return true;
        }
        return false;
    };
    plasma_rifle.sound = engine.resource_cache().resource<audio_clip_t>("sounds/modified/weapon-pickup.wav");
    descriptors[object_type_t::plasma_rifle_k] = plasma_rifle;

    /////////////////////////////////////////////////////////
    // GRENADE LAUNCHER
    ////////////////////////////////////////////////////////
    pickup_descriptor_t grenade_launcher{};
    grenade_launcher.texture_diffuse =
        engine.resource_cache().resource<texture_t>("pickups/rocket-launcher-pickup-diffuse-0000.tex");
    grenade_launcher.texture_emission =
        engine.resource_cache().resource<texture_t>("pickups/rocket-launcher-pickup-emission-0000.tex");
    grenade_launcher.sprite_scale = 0.75f;
    grenade_launcher.bounding_box = glm::vec3{0.25f, 0.75f, 0.25f};
    grenade_launcher.callback     = [](player_t& player, event_callback_t& event_callback) {
        if(player.add_weapon(weapon_type_t::grenade_launcher_k))
        {
            event_callback.display_message(message_t::pickup_grenade_launcher_k);
            return true;
        }
        return false;
    };
    grenade_launcher.sound = engine.resource_cache().resource<audio_clip_t>("sounds/modified/weapon-pickup.wav");
    descriptors[object_type_t::grenade_launcher_k] = grenade_launcher;

    return descriptors;
}

std::map<weapon_type_t, weapon_descriptor_t> create_weapon_descriptors(engine_context_t& engine)
{
    std::map<weapon_type_t, weapon_descriptor_t> descriptors;

    /////////////////////////////////////////////////////////
    // GRENADE LAUNCHER
    ////////////////////////////////////////////////////////
    weapon_descriptor_t grenade_launcher{};
    grenade_launcher.ammo_type            = ammo_type_t::grenades_k;
    grenade_launcher.ammo_on_first_pickup = 6;
    grenade_launcher.ammo_per_pickup      = 3;
    grenade_launcher.slot                 = 3;
    grenade_launcher.frames         = {{engine.resource_cache().resource<texture_t>("weapons/rocket-launcher-diffuse-0000.tex"),
                                engine.resource_cache().resource<texture_t>("weapons/rocket-launcher-emission-0000.tex"),
                                0.0f},
                               {engine.resource_cache().resource<texture_t>("weapons/rocket-launcher-diffuse-0001.tex"),
                                engine.resource_cache().resource<texture_t>("weapons/rocket-launcher-emission-0001.tex"),
                                0.50f,
                                engine.resource_cache().resource<audio_clip_t>("sounds/modified/grenade-launcher.wav")}};
    grenade_launcher.inactive_frame = 1;
    grenade_launcher.fire_callback  = [](world_t& world) {
        player_t* player = world.player();

        auto projectile = std::make_unique<projectile_t>(world,
                                                         player->camera_position(),
                                                         player->orientation(),
                                                         projectile_owner_t::player_k,
                                                         world.projectile_descriptors().at(projectile_type_t::grenade_k));
        world.add_entity(std::move(projectile));
    };
    grenade_launcher.muzzle_flash_light            = glm::vec3{2.0, 1.5, 1.0};
    descriptors[weapon_type_t::grenade_launcher_k] = grenade_launcher;

    /////////////////////////////////////////////////////////
    // PLASMA RIFLE
    ////////////////////////////////////////////////////////
    weapon_descriptor_t plasma_rifle{};
    plasma_rifle.ammo_type            = ammo_type_t::cells_k;
    plasma_rifle.ammo_on_first_pickup = 25;
    plasma_rifle.ammo_per_pickup      = 10;
    plasma_rifle.slot                 = 2;
    plasma_rifle.frames               = {{engine.resource_cache().resource<texture_t>("weapons/plasma-rifle-diffuse-0000.tex"),
                            engine.resource_cache().resource<texture_t>("weapons/plasma-rifle-emission-0000.tex"),
                            0.0f},
                           {engine.resource_cache().resource<texture_t>("weapons/plasma-rifle-diffuse-0001.tex"),
                            engine.resource_cache().resource<texture_t>("weapons/plasma-rifle-emission-0001.tex"),
                            0.10f,
                            engine.resource_cache().resource<audio_clip_t>("sounds/modified/plasma1.wav")}};
    plasma_rifle.inactive_frame       = 1;
    plasma_rifle.fire_callback        = [](world_t& world) {
        player_t* player = world.player();

        auto position =
            world.raycast(player->camera_position(), player->direction(), collision_layers::enemy_k, nullptr, nullptr);

        glm::vec3 projectile_position = player->camera_position() - glm::vec3{0.0f, 0.05f, 0.0f};
        if(position)
        {
            glm::vec3 vector         = *position - projectile_position;
            float     length         = glm::length(vector);
            float     desired_length = glm::min(length, 0.75f);

            projectile_position = projectile_position + glm::normalize(vector) * desired_length;
        }

        auto projectile = std::make_unique<projectile_t>(world,
                                                         projectile_position,
                                                         player->orientation(),
                                                         projectile_owner_t::player_k,
                                                         world.projectile_descriptors().at(projectile_type_t::player_plasma_k));
        world.add_entity(std::move(projectile));
    };
    plasma_rifle.muzzle_flash_light            = glm::vec3{0.5, 1.0, 2.0};
    descriptors[weapon_type_t::plasma_rifle_k] = plasma_rifle;

    /////////////////////////////////////////////////////////
    // SHOTGUN
    ////////////////////////////////////////////////////////
    weapon_descriptor_t shotgun{};
    shotgun.ammo_type            = ammo_type_t::shells_k;
    shotgun.ammo_on_first_pickup = 5;
    shotgun.ammo_per_pickup      = 3;
    shotgun.slot                 = 1;
    shotgun.frames               = {
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0000.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0000.tex"),
        0.0f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0001.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0001.tex"),
        0.5f,
        engine.resource_cache().resource<audio_clip_t>("sounds/modified/shotgun.wav")},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0002.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0002.tex"),
        0.1f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0003.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0003.tex"),
        0.1f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0004.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0004.tex"),
        0.1f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0005.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0005.tex"),
        0.1f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0002.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0002.tex"),
        0.1f},
        {engine.resource_cache().resource<texture_t>("weapons/shotgun-diffuse-0001.tex"),
        engine.resource_cache().resource<texture_t>("weapons/shotgun-emission-0001.tex"),
                       0.35f}
    };
    shotgun.inactive_frame       = 1;
    shotgun.fire_callback        = [](world_t& world) {
        player_t* player = world.player();

        static constexpr int32_t pellet_count_k   = 16;
        static constexpr float   spread_radians_k = 0.6f;
        for(int32_t i = 0; i < pellet_count_k; ++i)
        {
            float angle            = glm::linearRand(-0.5f, 0.5f) * spread_radians_k;
            auto  pellet_direction = glm::rotate(player->direction(), angle, {0, 1, 0});

            entity_t* hit_entity;
            auto      position =
                world.raycast(player->position(), pellet_direction, collision_layers::enemy_k, nullptr, &hit_entity);

            particle_type_t particle_type = hit_entity ? particle_type_t::blood_splash_k : particle_type_t::bullet_hit_k;

            if(position)
            {
                auto particle =
                    std::make_unique<particle_entity_t>(world, *position, world.particle_descriptors().at(particle_type));
                world.add_entity(std::move(particle));
            }

            if(particle_type == particle_type_t::bullet_hit_k)
                world.add_dynamic_light(dynamic_light_t{*position, {0.25f, 0.125f, 0.005f}, 3, true});

            enemy_t* hit_enemy = dynamic_cast<enemy_t*>(hit_entity);
            if(hit_enemy)
            {
                hit_enemy->take_damage(10);
            }
        }
    };
    shotgun.muzzle_flash_light            = glm::vec3{2.0, 1.5, 1.0};
    descriptors[weapon_type_t::shotgun_k] = shotgun;

    return descriptors;
}

std::map<projectile_type_t, projectile_descriptor_t> create_projectile_descriptors(engine_context_t& engine)
{
    std::map<projectile_type_t, projectile_descriptor_t> descriptors;

    /////////////////////////////////////////////////////////
    // GRENADE
    ////////////////////////////////////////////////////////
    projectile_descriptor_t grenade{};
    grenade.texture_diffuse       = engine.resource_cache().resource<texture_t>("projectiles/grenade-diffuse.tex");
    grenade.texture_emission      = engine.resource_cache().resource<texture_t>("projectiles/grenade-emission.tex");
    grenade.sprite_scale          = 0.125f;
    grenade.sprite_rotation_speed = glm::two_pi<float>();
    grenade.damage                = 100.0f;
    grenade.lifetime              = 2.0f;
    grenade.speed                 = 10.0f;
    grenade.friction              = 0.02f;
    grenade.bounciness            = 0.9f;
    grenade.bounding_box          = glm::vec3{0.125f, 0.125f, 0.125f};
    grenade.gravity               = true;
    grenade.light                 = glm::vec3{1.0, 0.25, 0.05};
    grenade.death_sound           = engine.resource_cache().resource<audio_clip_t>("sounds/modified/explosion1.wav");
    grenade.bounce_sound          = engine.resource_cache().resource<audio_clip_t>("sounds/grenade-bounce.wav");
    grenade.explosion_light       = glm::vec3(4.0f, 2.0f, 1.0f);
    grenade.explosion_particle    = particle_type_t::explosion_k;

    descriptors[projectile_type_t::grenade_k] = grenade;

    /////////////////////////////////////////////////////////
    // ENEMY PLASMA
    ////////////////////////////////////////////////////////
    projectile_descriptor_t enemy_plasma{};
    enemy_plasma.texture_diffuse    = engine.resource_cache().resource<texture_t>("projectiles/plasma-diffuse.tex");
    enemy_plasma.texture_emission   = engine.resource_cache().resource<texture_t>("projectiles/plasma-emission.tex");
    enemy_plasma.sprite_scale       = 0.25f;
    enemy_plasma.damage             = 5.0f;
    enemy_plasma.lifetime           = 2.0f;
    enemy_plasma.speed              = 10.0f;
    enemy_plasma.friction           = 0.0f;
    enemy_plasma.bounciness         = 0.0f;
    enemy_plasma.bounding_box       = glm::vec3{0.125f, 0.125f, 0.125f};
    enemy_plasma.gravity            = false;
    enemy_plasma.light              = glm::vec3{1.0f, 0.2f, 0.0f};
    enemy_plasma.explosion_light    = glm::vec3(2.0f, 0.1f, 0.0f);
    enemy_plasma.explosion_particle = particle_type_t::bullet_hit_k;
    enemy_plasma.death_sound        = engine.resource_cache().resource<audio_clip_t>("sounds/modified/hit1.wav");

    descriptors[projectile_type_t::enemy_plasma_k] = enemy_plasma;

    /////////////////////////////////////////////////////////
    // MEDIUM ENEMY PLASMA
    ////////////////////////////////////////////////////////
    projectile_descriptor_t medium_enemy_plasma = enemy_plasma;
    medium_enemy_plasma.texture_diffuse    = engine.resource_cache().resource<texture_t>("projectiles/plasma-diffuse-green.tex");
    medium_enemy_plasma.damage             = 10.0f;
    medium_enemy_plasma.speed              = 7.0f;
    medium_enemy_plasma.lifetime           = 5.0f;
    medium_enemy_plasma.light              = glm::vec3{0.2f, 1.0f, 0.0f};
    medium_enemy_plasma.explosion_light    = glm::vec3(0.1f, 2.0f, 0.0f);

    descriptors[projectile_type_t::enemy_plasma_medium_k] = medium_enemy_plasma;

    /////////////////////////////////////////////////////////
    // HEAVY ENEMY PLASMA
    ////////////////////////////////////////////////////////
    projectile_descriptor_t heavy_enemy_plasma = enemy_plasma;
    heavy_enemy_plasma.texture_diffuse    = engine.resource_cache().resource<texture_t>("projectiles/plasma-diffuse-blue.tex");
    heavy_enemy_plasma.damage             = 20.0f;
    heavy_enemy_plasma.speed              = 5.0f;
    heavy_enemy_plasma.lifetime           = 10.0f;
    heavy_enemy_plasma.light              = glm::vec3{0.1f, 0.25f, 1.0f};
    heavy_enemy_plasma.explosion_light    = glm::vec3(0.1f, 0.2f, 2.0f);

    descriptors[projectile_type_t::enemy_plasma_heavy_k] = heavy_enemy_plasma;

    /////////////////////////////////////////////////////////
    // PLAYER PLASMA
    ////////////////////////////////////////////////////////
    projectile_descriptor_t player_plasma{};
    player_plasma.texture_diffuse    = engine.resource_cache().resource<texture_t>("projectiles/plasma-diffuse-blue.tex");
    player_plasma.texture_emission   = engine.resource_cache().resource<texture_t>("projectiles/plasma-emission.tex");
    player_plasma.sprite_scale       = 0.25f;
    player_plasma.damage             = 20.0f;
    player_plasma.lifetime           = 4.0f;
    player_plasma.speed              = 10.0f;
    player_plasma.friction           = 0.0f;
    player_plasma.bounciness         = 0.0f;
    player_plasma.bounding_box       = glm::vec3{0.125f, 0.125f, 0.125f};
    player_plasma.gravity            = false;
    player_plasma.light              = glm::vec3{0.0f, 0.1f, 0.5f};
    player_plasma.explosion_light    = glm::vec3(0.1f, 0.1f, 1.0f);
    player_plasma.explosion_particle = particle_type_t::bullet_hit_k;
    player_plasma.death_sound        = engine.resource_cache().resource<audio_clip_t>("sounds/modified/hit1.wav");

    descriptors[projectile_type_t::player_plasma_k] = player_plasma;

    return descriptors;
}

std::map<particle_type_t, particle_descriptor_t> create_particle_descriptors(engine_context_t& engine)
{
    std::map<particle_type_t, particle_descriptor_t> descriptors;

    /////////////////////////////////////////////////////////
    // EXPLOSION
    ////////////////////////////////////////////////////////
    particle_descriptor_t explosion{};
    explosion.texture_diffuse                 = engine.resource_cache().resource<texture_t>("white.tex");
    explosion.particle_size                   = 0.005f;
    explosion.radius                          = 0.25f;
    explosion.color                           = {glm::vec4{0.5f, 0.5f, 0.25f, 1.0f}, glm::vec4{1.0f, 0.75f, 0.5f, 1.0f}};
    explosion.count                           = {256, 256};
    explosion.lifetime                        = {0.5f, 1.0f};
    explosion.velocity                        = {2.0f, 4.0f};
    explosion.gravity                         = glm::vec3{0.0f, -8.0f, 0.0f};
    descriptors[particle_type_t::explosion_k] = explosion;

    /////////////////////////////////////////////////////////
    // BULLET HIT
    ////////////////////////////////////////////////////////
    particle_descriptor_t bullet_hit{};
    bullet_hit.texture_diffuse                 = engine.resource_cache().resource<texture_t>("white.tex");
    bullet_hit.particle_size                   = 0.0025f;
    bullet_hit.radius                          = 0.05f;
    bullet_hit.color                           = {glm::vec4{0.5f, 0.5f, 0.25f, 1.0f}, glm::vec4{1.0f, 0.75f, 0.5f, 1.0f}};
    bullet_hit.count                           = {4, 8};
    bullet_hit.lifetime                        = {0.05f, 0.5f};
    bullet_hit.velocity                        = {0.5f, 1.0f};
    bullet_hit.gravity                         = glm::vec3{0.0f, -4.0f, 0.0f};
    descriptors[particle_type_t::bullet_hit_k] = bullet_hit;

    /////////////////////////////////////////////////////////
    // BLOOD SPLASH
    ////////////////////////////////////////////////////////
    particle_descriptor_t blood_splash{};
    blood_splash.texture_diffuse                 = engine.resource_cache().resource<texture_t>("white.tex");
    blood_splash.particle_size                   = 0.0025f;
    blood_splash.radius                          = 0.05f;
    blood_splash.color                           = {glm::vec4{0.1f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.75f, 0.0f, 0.0f, 1.0f}};
    blood_splash.count                           = {16, 32};
    blood_splash.lifetime                        = {0.1f, 0.5f};
    blood_splash.velocity                        = {0.1f, 0.25f};
    blood_splash.gravity                         = glm::vec3{0.0f, -1.0f, 0.0f};
    descriptors[particle_type_t::blood_splash_k] = blood_splash;

    return descriptors;
}

std::map<ammo_type_t, texture_handle_t> create_ammo_icons(engine_context_t& engine)
{
    std::map<ammo_type_t, texture_handle_t> icons;

    icons[ammo_type_t::shells_k]   = engine.resource_cache().resource<texture_t>("pickups/shells-diffuse-0000.tex");
    icons[ammo_type_t::grenades_k] = engine.resource_cache().resource<texture_t>("pickups/grenades-diffuse-0000.tex");
    icons[ammo_type_t::cells_k]    = engine.resource_cache().resource<texture_t>("pickups/cells-diffuse-0000.tex");

    return icons;
}

std::map<enemy_type_t, enemy_descriptor_t> create_enemy_descriptors(engine_context_t& engine)
{
    std::map<enemy_type_t, enemy_descriptor_t> descriptors;

    // TODO: Refactor this, it's error-prone and hard to maintain.
    for(int32_t enemy_index = 0; enemy_index < 3; ++enemy_index)
    {
        enemy_descriptor_t enemy{};

        std::string frame_names[] = {"0000", "0002", "0004", "0006", "0008", "0010", "0020", "0022", "0024", "0026",
                                    "0028", "0030", "0050", "0052", "0054", "0056", "0058", "0060", "0062", "0064"};

        std::string base_string = fmt::format("enemies/enemy{}/enemy{}-", enemy_index + 1, enemy_index + 1);
        for(uint32_t i = 0; i < 16; ++i)
        {
            enemy.frames.push_back({});

            std::string base_diffuse_string  = base_string + fmt::format("diffuse-{}-", i);
            std::string base_emission_string = base_string + fmt::format("emission-{}-", i);

            for(auto frame_name: frame_names)
            {
                std::string diffuse_string  = base_diffuse_string + frame_name + ".tex";
                std::string emission_string = base_emission_string + frame_name + ".tex";

                enemy.frames.back().push_back({engine.resource_cache().resource<texture_t>(diffuse_string),
                                                    engine.resource_cache().resource<texture_t>(emission_string)});
            }
        }

        enemy.health = 100 + 100 * enemy_index;
        enemy.fire_range = 4.0f + 2 * enemy_index;
        enemy.fire_duration = 0.5f + 0.5f * enemy_index;

        enemy.animation_frame_indices[enemy_descriptor_t::animation_t::walking_k]  = {0, 1, 2, 3, 4, 5};
        enemy.animation_frame_indices[enemy_descriptor_t::animation_t::shooting_k] = {6, 7, 8, 9, 10, 11};
        enemy.animation_frame_indices[enemy_descriptor_t::animation_t::dying_k]    = {12, 13, 14, 15, 16, 17, 18, 19};

        if(enemy_index == 0)
            enemy.projectile_type = projectile_type_t::enemy_plasma_k;
        else if(enemy_index == 1)
            enemy.projectile_type = projectile_type_t::enemy_plasma_medium_k;
        else
            enemy.projectile_type = projectile_type_t::enemy_plasma_heavy_k;

        enemy.attack_sound    = engine.resource_cache().resource<audio_clip_t>("sounds/modified/plasma1.wav");
        enemy.death_sound     = engine.resource_cache().resource<audio_clip_t>("sounds/modified/monster-pain1.wav");
        enemy.hurt_sound      = engine.resource_cache().resource<audio_clip_t>("sounds/modified/monster-pain2.wav");
        enemy.detect_sound    = engine.resource_cache().resource<audio_clip_t>("sounds/modified/monster-detect1.wav");

        if(enemy_index == 0)
            descriptors[enemy_type_t::light_k] = enemy;
        else if(enemy_index == 1)
            descriptors[enemy_type_t::medium_k] = enemy;
        else
            descriptors[enemy_type_t::heavy_k] = enemy;
    }

    return descriptors;
}

} // namespace mau
