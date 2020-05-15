#include "worship/states/state_main_menu.hpp"

#include "worship/states/state_loading.hpp"
#include "worship/states/state_read_this.hpp"
#include "worship/states/state_gameplay.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>

namespace mau {

state_main_menu_t::state_main_menu_t(engine_context_t& engine) : state_t(engine, "main_menu")
{
    scene_render_target_m = render_target_t::create(engine.viewport_size(), 2);
    ubershader_m          = engine.resource_cache().resource<shader_t>("ubershader.sha");
    font_small_m          = engine.resource_cache().resource<font_t>("fonts/font_small.mfo");
    font_medium_m         = engine.resource_cache().resource<font_t>("fonts/font_medium.mfo");
    indicator_m           = engine.resource_cache().resource<texture_t>("main-menu-indicator.tex");

    auto menu_main = std::make_unique<menu_t>(engine, font_medium_m, indicator_m, "");
    menu_main->add_function_entry("NEW GAME", [&] { change_submenu(submenu_t::new_game_k); });
    menu_main->add_function_entry("OPTIONS", [&] { change_submenu(submenu_t::options_k); });
    menu_main->add_function_entry("READ THIS!",
                                  [&] { engine_m.state_manager().push(std::make_unique<state_read_this_t>(engine_m,
                                                                      "readthis.txt",
                                                                      "main-menu-background.tex",
                                                                      read_this_behavior_t::pop_k
                                                                      ));
                                      });
    menu_main->add_function_entry("QUIT GAME", [&] { engine_m.state_manager().clear(); });

    auto menu_options = std::make_unique<menu_t>(engine, font_medium_m, indicator_m, "OPTIONS");
    menu_options->add_option_entry("VSYNC", {"OFF", "ON"}, engine.config().vsync);
    menu_options->add_option_entry("BLOOM", {"OFF", "ON"}, engine.config().bloom);
    menu_options->add_option_entry("SCREEN MODE", {"WINDOW", "FULLSCREEN"}, engine.config().fullscreen);
    menu_options->add_option_entry("WINDOW SCALE", {"x1", "x2", "x4"}, engine.config().window_scale);
    menu_options->add_option_entry("DYNAMIC LIGHTING", {"OFF", "ON"}, engine.config().dynamic_lighting);
    menu_options->add_function_entry("APPLY", [&] {
        config_t& config = engine.config();

        auto* menu_options = submenus_m[submenu_t::options_k].get();

        config.vsync            = menu_options->get_option_entry("VSYNC")->selected_option;
        config.bloom            = menu_options->get_option_entry("BLOOM")->selected_option;
        config.fullscreen       = menu_options->get_option_entry("SCREEN MODE")->selected_option;
        config.dynamic_lighting = menu_options->get_option_entry("DYNAMIC LIGHTING")->selected_option;
        config.window_scale     = menu_options->get_option_entry("WINDOW SCALE")->selected_option;

        engine.save_config();
        change_submenu(submenu_t::changes_require_restart_k);
    });
    menu_options->add_function_entry("CANCEL", [&] {
        auto* menu_options = submenus_m[submenu_t::options_k].get();
        menu_options->reset_options();
        change_submenu(submenu_t::main_k);
    });

    auto menu_changes_dialog =
        std::make_unique<menu_t>(engine, font_medium_m, indicator_m, "RESTART REQUIRED TO APPLY CHANGES.");
    menu_changes_dialog->add_function_entry("RESTART NOW", [&] { engine.request_restart(); });
    menu_changes_dialog->add_function_entry("RESTART LATER", [&] { change_submenu(submenu_t::main_k); });

    auto menu_new_game = std::make_unique<menu_t>(engine, font_medium_m, indicator_m, "SELECT DIFFICULTY");
    menu_new_game->add_function_entry("POWER FANTASY", [&] {
        engine_m.state_manager().push_clear(std::make_unique<state_loading_t>(engine_m, difficulty_t::easy_k));
    });
    menu_new_game->add_function_entry("VANILLA EXPERIENCE", [&] {
        engine_m.state_manager().push_clear(std::make_unique<state_loading_t>(engine_m, difficulty_t::normal_k));
    });
    menu_new_game->add_function_entry("HARDER, DADDY!", [&] {
        engine_m.state_manager().push_clear(std::make_unique<state_loading_t>(engine_m, difficulty_t::hard_k));
    });
    menu_new_game->add_function_entry("BACK", [&] { change_submenu(submenu_t::main_k); });

    submenus_m[submenu_t::main_k]                    = std::move(menu_main);
    submenus_m[submenu_t::options_k]                 = std::move(menu_options);
    submenus_m[submenu_t::changes_require_restart_k] = std::move(menu_changes_dialog);
    submenus_m[submenu_t::new_game_k]                = std::move(menu_new_game);

    blip_m = engine_m.resource_cache().resource<audio_clip_t>("sounds/modified/menu-blip.wav");
}
void state_main_menu_t::handle_event(const SDL_Event& event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            engine_m.state_manager().pop();
        }
        else
        {
            engine_m.audio().play_clip(blip_m);

            auto& submenu = submenus_m[current_submenu_m];
            switch(event.key.keysym.scancode)
            {
                case SDL_SCANCODE_UP: submenu->select_previous(); break;
                case SDL_SCANCODE_DOWN: submenu->select_next(); break;
                case SDL_SCANCODE_RETURN:
                case SDL_SCANCODE_RETURN2:
                case SDL_SCANCODE_SPACE: submenu->execute_entry(); break;
                default: break;
            }
        }
    }
}
void state_main_menu_t::fixed_update(float delta_time)
{
}
void state_main_menu_t::variable_update(float delta_time)
{
}
render_target_handle_t state_main_menu_t::render(float delta_time)
{
    renderer_t& renderer = engine_m.renderer();

    renderer.bind_render_target(scene_render_target_m);

    renderer.bind_shader(ubershader_m);

    depth_guard_t lock{renderer, depth_test_t::disabled_k};
    ubershader_m->set_uniform_bool(shader_uniform_t::enable_emission_k, false);
    ubershader_m->set_uniform_mat4(shader_uniform_t::model_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::view_matrix_k, glm::identity<glm::mat4>());
    ubershader_m->set_uniform_mat4(shader_uniform_t::projection_matrix_k, math::ortho_matrix(engine_m.viewport_size()));

    renderer.bind_texture(nullptr, 0);
    // Scope vertex batch. Dark overlay.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch, {0.0f, 0.0f}, engine_m.viewport_size(), glm::vec4(0, 0, 0, 0.5));
    }

    submenus_m[current_submenu_m]->render(delta_time);

    return scene_render_target_m;
}

void state_main_menu_t::change_submenu(submenu_t submenu)
{
    current_submenu_m = submenu;
}

} // namespace mau
