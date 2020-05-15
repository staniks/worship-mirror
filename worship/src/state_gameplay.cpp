#include "worship/states/state_gameplay.hpp"

#include "worship/constants.hpp"
#include "worship/gameplay/entities/projectile.hpp"
#include "worship/states/state_main_menu.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <GL/gl3w.h>

namespace mau {

state_gameplay_t::state_gameplay_t(engine_context_t& engine, difficulty_t difficulty) :
    state_t(engine, "gameplay"),
    event_callback_m(*this),
    effect_bloom_m(engine),
    world_m(engine, event_callback_m, difficulty),
    player_m(world_m.player()),
    ammo_icons_m(create_ammo_icons(engine))
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_small_m          = engine.resource_cache().resource<font_t>("fonts/font_small.mfo");
    font_medium_m         = engine.resource_cache().resource<font_t>("fonts/font_medium.mfo");
    font_large_m          = engine.resource_cache().resource<font_t>("fonts/font_large.mfo");

    gui_indicator_health_m     = engine.resource_cache().resource<texture_t>("pickups/health-diffuse-0000.tex");
    gui_indicator_armor_m      = engine.resource_cache().resource<texture_t>("pickups/armor-diffuse-0000.tex");
    gui_screen_flash_texture_m = engine.resource_cache().resource<texture_t>("white.tex");
}
void state_gameplay_t::handle_event(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.scancode)
        {
            case SDL_SCANCODE_ESCAPE: engine_m.state_manager().push(std::make_unique<state_main_menu_t>(engine_m)); break;
            case SDL_SCANCODE_1: player_m->select_weapon_slot(1); break;
            case SDL_SCANCODE_2: player_m->select_weapon_slot(2); break;
            case SDL_SCANCODE_3: player_m->select_weapon_slot(3); break;
            default: break;
        }
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
    }
}
void state_gameplay_t::fixed_update(float delta_time)
{
    glm::vec3 look_direction{};

    if(engine_m.is_key_down(SDL_SCANCODE_W) || engine_m.is_key_down(SDL_SCANCODE_UP))
        look_direction += player_m->direction();
    if(engine_m.is_key_down(SDL_SCANCODE_S) || engine_m.is_key_down(SDL_SCANCODE_DOWN))
        look_direction -= player_m->direction();
    if(engine_m.is_key_down(SDL_SCANCODE_A))
        look_direction -= player_m->right();
    if(engine_m.is_key_down(SDL_SCANCODE_D))
        look_direction += player_m->right();

    look_direction.y = 0;

    if(glm::length(look_direction))
        look_direction = glm::normalize(look_direction);

    player_m->walk(look_direction, delta_time);

    world_m.fixed_update(delta_time);
}
void state_gameplay_t::variable_update(float delta_time)
{
    static constexpr float turn_dampening_mouse_k = 128.0f;
    static constexpr float turn_dampening_keyboard_k = 16.0f;

    const glm::vec2 mouse_motion = engine_m.mouse_motion() * 0.01f;

    player_m->rotate(-mouse_motion.x / turn_dampening_mouse_k, glm::vec3{0.0f, 1.0f, 0.0f});

    float desired_rotation = 0.0f;
    if(engine_m.is_key_down(SDL_SCANCODE_LEFT))
        desired_rotation = 1.0f;
    if(engine_m.is_key_down(SDL_SCANCODE_RIGHT))
        desired_rotation = -1.0f;

    if(desired_rotation)
        player_m->rotate(desired_rotation * delta_time / turn_dampening_keyboard_k, glm::vec3{0.0f, 1.0f, 0.0f});

    if((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) ||
        engine_m.is_key_down(SDL_SCANCODE_LCTRL))
    {
        player_m->fire();
    }

    world_m.variable_update(delta_time);

    audio_t& audio = engine_m.audio();
    audio.update_listener(player_m->position(), player_m->orientation());
}
render_target_handle_t state_gameplay_t::render(float delta_time)
{
    renderer_t& renderer = engine_m.renderer();

    renderer.bind_render_target(scene_render_target_m);
    renderer.clear({0.0f, 0.0f, 0.0f, 1.0f});
    renderer.set_depth_test(depth_test_t::enabled_k);

    //*****************************************************
    // RENDER WORLD
    //*****************************************************
    glm::mat4 view       = math::view_matrix(player_m->camera_position(), player_m->orientation());
    glm::mat4 projection = math::perspective_matrix(player_m->fov(), engine_m.viewport_size(), 0.1f, 1024.0f);

    renderer.bind_shader(ubershader_m);
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, true);
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_fog_k, false);
    ubershader_m->set_uniform_float(shader_uniform_t::fog_density_k, 0.10f);

    world_m.render(delta_time, ubershader_m, projection, view);

    ubershader_m->set_uniform_bool(shader_uniform_t::enable_fog_k, false);

    renderer.set_fill_mode(fill_mode_t::fill_k);

    //*****************************************************
    // GUI STARTS RENDERING
    //*****************************************************

    depth_guard_t lock{renderer, depth_test_t::disabled_k};
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, false);
    ubershader_m->set_uniform_mat4(shader_uniform_t::model_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::view_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::projection_matrix_k, math::ortho_matrix(engine_m.viewport_size()));

    //*****************************************************
    // RENDER WEAPON
    //*****************************************************

    // Weapon.
    weapon_t* weapon = player_m->current_weapon();
    if(weapon != nullptr)
    {
        static constexpr float weapon_knockback_multiplier_k = 16.0f;

        glm::vec2 knockback = glm::vec2{1, 1} * weapon_knockback_multiplier_k * weapon->knockback_factor();

        ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, true);
        renderer.bind_texture(weapon->texture_diffuse(), 0);
        renderer.bind_texture(weapon->texture_emission(), 1);
        {
            glm::vec4 lighting = world_m.light({player_m->position().x, player_m->position().z});

            vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
            renderer.batch_sprite(
                batch, glm::vec2{130, 110} + player_m->weapon_bob() + knockback, weapon->texture_diffuse()->size(), lighting);
        }
        ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, false);
    }

    //*****************************************************
    // RENDER SCREEN FLASH
    //*****************************************************
    renderer.bind_texture(gui_screen_flash_texture_m, 0);

    float factor =
        (float)((screen_flash_timer_m.microseconds() * timer_t::microseconds_to_seconds_k) / screen_flash_duration_k);
    factor = glm::clamp(factor, 0.0f, 1.0f);

    // Scope vertex batch.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch,
                              {0.0f, 0.0f},
                              engine_m.viewport_size(),
                              glm::vec4{screen_flashes_k.at(screen_flash_m), (1.0f - factor) * screen_flash_alpha});
    }

    //*****************************************************
    // RENDER STATUS INDICATORS (HEALTH, ARMOR, AMMO)
    //*****************************************************

    static constexpr float indicator_padding_k = 2.0f;
    glm::vec2              indicator_size      = font_large_m->character_size * 2.0f;
    glm::vec2              indicator_text_offset =
        glm::vec2{indicator_size.x + indicator_padding_k, font_large_m->character_size.y / 2 + indicator_padding_k};

    glm::vec2 indicator_position_health =
        glm::vec2{indicator_padding_k, engine_m.viewport_size().y - indicator_size.y - indicator_padding_k};
    glm::vec2 indicator_position_armor = indicator_position_health + glm::vec2{100, 0};
    glm::vec2 indicator_position_ammo  = glm::vec2{engine_m.viewport_size().x - indicator_size.x - indicator_padding_k,
                                                  engine_m.viewport_size().y - indicator_size.y - indicator_padding_k};

    // Health status icon.
    renderer.bind_texture(gui_indicator_health_m, 0);
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, indicator_position_health, indicator_size, color_white_k);
    }

    // Armor status icon.
    renderer.bind_texture(gui_indicator_armor_m, 0);
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, indicator_position_armor, indicator_size, color_white_k);
    }

    if(weapon != nullptr)
    {
        // Ammo status icon.
        renderer.bind_texture(ammo_icons_m[weapon->descriptor().ammo_type], 0);
        {
            vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
            renderer.batch_sprite(batch, indicator_position_ammo, indicator_size, color_white_k);
        }
    }

    // Status text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        int32_t health = (int32_t)player_m->health();
        int32_t armor  = (int32_t)player_m->armor();
        int32_t ammo   = (int32_t)player_m->ammo();

        fmt::memory_buffer health_buffer;
        format_to(health_buffer, "{}", health);

        fmt::memory_buffer armor_buffer;
        format_to(armor_buffer, "{}", armor);

        fmt::memory_buffer ammo_buffer;
        format_to(ammo_buffer, "{}", ammo);

        std::string_view health_view{health_buffer.data(), health_buffer.size()};
        std::string_view armor_view{armor_buffer.data(), armor_buffer.size()};
        std::string_view ammo_view{ammo_buffer.data(), ammo_buffer.size()};

        // Health
        renderer.batch_string(
            batch, font_large_m, indicator_position_health + indicator_text_offset, health_view, text_color_k);
        // Armor
        renderer.batch_string(batch, font_large_m, indicator_position_armor + indicator_text_offset, armor_view, text_color_k);

        if(weapon != nullptr)
        {
            // Ammo
            renderer.batch_string(batch,
                                  font_large_m,
                                  indicator_position_ammo -
                                      glm::vec2{indicator_padding_k + font_large_m->character_size.x * ammo_view.length(),
                                                -indicator_text_offset.y},
                                  ammo_view,
                                  text_color_k);
        }
    }

    // Game message.
    if(game_message_timer_m.seconds() < game_message_duration_k)
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        // Player position.
        renderer.batch_string(batch, font_medium_m, {4, 4}, messages_k.at(game_message_m), text_color_k);
    }

#ifdef SHOW_DEBUG_INFO
    // Debug text.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        fmt::memory_buffer player_position_buffer;
        format_to(player_position_buffer, "Player: {:.4f}, {:.4f}", player_m->position().x, player_m->position().z);
        std::string_view player_position_view{player_position_buffer.data(), player_position_buffer.size()};

        fmt::memory_buffer fps_buffer;
        format_to(fps_buffer, "FPS: {}", (int)engine_m.framerate());
        std::string_view fps_view{fps_buffer.data(), fps_buffer.size()};

        const float delta = font_small_m->character_size.y;

        // Player position.
        renderer.batch_string(batch, font_small_m, {0, delta}, player_position_view, text_color_k);

        // Framerate.
        renderer.batch_string(batch, font_small_m, {0, delta * 2}, fps_view, text_color_k);
    }
#endif

    if(engine_m.config().bloom)
        return effect_bloom_m.apply(scene_render_target_m);
    else
        return scene_render_target_m;
}
void state_gameplay_t::display_message(message_t message)
{
    game_message_m = message;
    game_message_timer_m.reset();
}

void state_gameplay_t::flash_screen(screen_flash_t screen_flash)
{
    screen_flash_m = screen_flash;
    screen_flash_timer_m.reset();
}

} // namespace mau
