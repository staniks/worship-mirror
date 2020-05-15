#pragma once

#include "mau/SDL/SDL.hpp"
#include "mau/audio/audio.hpp"
#include "mau/base/configuration.hpp"
#include "mau/base/log.hpp"
#include "mau/base/state_manager.hpp"
#include "mau/base/timer.hpp"
#include "mau/logging/log_file.hpp"
#include "mau/logging/log_stdout.hpp"
#include "mau/rendering/renderer.hpp"

#include <array>
#include <memory>

namespace mau {

// Forward declarations.
class resource_cache_t;

// Engine context is the root of the game. Contains modules, game loop and state management.
class engine_context_t : non_movable_t, non_copyable_t
{
public:
    // Create the engine instance with specified name and log.
    explicit engine_context_t(const char* name, log_t& log);
    ~engine_context_t();

    // Return references to various modules.
    config_t&         config();
    log_t&            log();
    renderer_t&       renderer();
    audio_t&          audio();
    state_manager_t&  state_manager();
    resource_cache_t& resource_cache();

    // Returns drawable area size. This is scaled to fill the screen with aspect ratio correction.
    glm::ivec2 viewport_size() const;

    // Returns actual screen or window resolution.
    glm::ivec2 resolution() const;

    // Returns true if specified key is being held down.
    bool      is_key_down(SDL_Scancode pScancode) const;

    // Returns difference in mouse position since last frame.
    glm::vec2 mouse_motion() const;

    // Returns average framerate.
    double    framerate() const;

    // Returns SDL window handle.
    SDL_Window* sdl_window();

    // Run engine with specified state.
    template<class StateType>
    void run()
    {
        state_manager_m->push(std::make_unique<StateType>(*this));
        runImpl();
    }

    bool restart_requested();

    // Clear the state stack and request re-run of the engine. The client application must handle this.
    void request_restart();

    void load_config();
    void save_config();

private:
    void runImpl();

    config_t config_m;
    log_t&   log_m;

    glm::ivec2 resolution_m;

    sdl_context_t                                             sdl_context_m;
    sdl_mixer_context_t                                       sdl_mixer_context_m;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdl_window_m;

    std::unique_ptr<resource_cache_t> resource_cache_m;
    std::unique_ptr<renderer_t>       renderer_m;
    std::unique_ptr<audio_t>          audio_m;
    std::unique_ptr<state_manager_t>  state_manager_m;

    glm::vec2      mouse_motion_m;
    const uint8_t* sdl_keyboard_state_m;

    std::array<double, 64> framerate_array_m = {0};
    uint64_t               framerate_tick_m{0};

    bool restart_requested_m{false};
};

} // namespace mau
