#include "mau/base/engine_context.hpp"

#include "mau/io/archive.hpp"
#include "mau/io/image.hpp"
#include "mau/io/resource_cache.hpp"
#include "mau/rendering/GL/common.hpp"

namespace mau {

engine_context_t::engine_context_t(const char* name, log_t& log) : log_m(log), sdl_window_m(nullptr, SDL_DestroyWindow)
{
    log.log("loading configuration");
    load_config();

    // Window scale is powers of 2. Viewport size is fixed, so set resolution accordingly.
    resolution_m = viewport_size() * (1 << (int32_t)config_m.window_scale);

    log.log("configuring main window");
    uint32_t flags = SDL_WINDOW_OPENGL;
    if(config_m.fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);

        resolution_m.x = dm.w;
        resolution_m.y = dm.h;
    }

    // Set expected attributes before GL context creation.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, target_gl_version_major_k);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, target_gl_version_minor_k);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    sdl_window_m.reset(
        SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_m.x, resolution_m.y, flags));

    if(sdl_window_m.get() == nullptr)
        throw exception_t{"unable to create SDL window"};

    resource_cache_m = std::make_unique<resource_cache_t>(*this);
    renderer_m       = std::make_unique<renderer_t>(*this);
    audio_m          = std::make_unique<audio_t>(*this);
    state_manager_m  = std::make_unique<state_manager_t>(*this);

    sdl_keyboard_state_m = SDL_GetKeyboardState(nullptr);

    SDL_SetRelativeMouseMode(SDL_TRUE);
}

engine_context_t::~engine_context_t()
{
}

void engine_context_t::runImpl()
{
    timer_t timer{};
    double  accumulator{};

    uint64_t frameTimeCurrent = timer.microseconds();
    uint64_t frameTimeLast    = frameTimeCurrent;

    static constexpr double fixed_framerate_k               = 60.0;
    static constexpr double fixed_delta_time_microseconds_k = (1.0 / fixed_framerate_k) / timer_t::microseconds_to_seconds_k;
    static constexpr double fixed_delta_time_seconds_k = fixed_delta_time_microseconds_k * timer_t::microseconds_to_seconds_k;

    while(!state_manager_m->empty())
    {
        frameTimeCurrent              = timer.microseconds();
        const uint64_t frameTimeDelta = frameTimeCurrent - frameTimeLast;
        frameTimeLast                 = frameTimeCurrent;

        if(frameTimeDelta)
            framerate_array_m[framerate_tick_m] = 1000000.0 / frameTimeDelta;
        framerate_tick_m                    = (framerate_tick_m + 1) % framerate_array_m.size();

        accumulator += frameTimeDelta;

        while(accumulator >= fixed_delta_time_microseconds_k)
        {
            state_manager_m->fixed_update((float)fixed_delta_time_seconds_k);
            accumulator -= fixed_delta_time_microseconds_k;
        }

        const double variableDeltaTimeSeconds = frameTimeDelta * timer_t::microseconds_to_seconds_k;
        state_manager_m->variable_update((float)variableDeltaTimeSeconds);

        state_manager_m->render((float)variableDeltaTimeSeconds);

        renderer_m->present();

        SDL_Event sdlEvent;
        while(SDL_PollEvent(&sdlEvent))
        {
            if(sdlEvent.type == SDL_QUIT)
            {
                state_manager_m->clear();
            }
            state_manager_m->handle_event(sdlEvent);
        }

        int32_t mouseDx{}, mouseDy{};
        SDL_GetRelativeMouseState(&mouseDx, &mouseDy);
        mouse_motion_m = glm::vec2{mouseDx, mouseDy};
    }
}

config_t& engine_context_t::config()
{
    return config_m;
}

log_t& engine_context_t::log()
{
    return log_m;
}

renderer_t& engine_context_t::renderer()
{
    return *renderer_m.get();
}

audio_t& engine_context_t::audio()
{
    return *audio_m;
}

state_manager_t& engine_context_t::state_manager()
{
    return *state_manager_m;
}

resource_cache_t& engine_context_t::resource_cache()
{
    return *resource_cache_m.get();
}

glm::ivec2 engine_context_t::viewport_size() const
{
    return glm::ivec2{320, 240};
}

glm::ivec2 engine_context_t::resolution() const
{
    return resolution_m;
}

bool engine_context_t::is_key_down(SDL_Scancode pScancode) const
{
    return sdl_keyboard_state_m[pScancode];
}

glm::vec2 engine_context_t::mouse_motion() const
{
    return mouse_motion_m;
}

double engine_context_t::framerate() const
{
    double framerate{};

    for(auto value: framerate_array_m)
        framerate += value;

    return framerate / framerate_array_m.size();
}

SDL_Window* engine_context_t::sdl_window()
{
    return sdl_window_m.get();
}

bool engine_context_t::restart_requested()
{
    return restart_requested_m;
}
void engine_context_t::request_restart()
{
    restart_requested_m = true;
    state_manager_m->clear();
}

void engine_context_t::load_config()
{
    bool config_failed{false};

    std::fstream config_stream{"config.bin", std::ios_base::in | std::ios_base::binary};
    if(config_stream)
    {
        if(!config_stream.read(reinterpret_cast<char*>(&config_m), sizeof(config_m)))
        {
            log_m.log(log_severity_t::error_k, "configuration file seems malformed");
            config_failed = true;
        }
    }
    else
    {
        log_m.log(log_severity_t::warning_k, "configuration file not found");
        config_failed = true;
    }

    if(config_failed)
    {
        config_m                  = config_t{};
        config_m.window_scale     = 1;
        config_m.fullscreen       = false;
        config_m.vsync            = true;
        config_m.bloom            = true;
        config_m.dynamic_lighting = true;
    }
}

void engine_context_t::save_config()
{
    std::fstream config_stream{"config.bin", std::ios_base::out | std::ios_base::binary};
    if(config_stream)
    {
        if(!config_stream.write(reinterpret_cast<char*>(&config_m), sizeof(config_m)))
            log_m.log(log_severity_t::error_k, "error while writing into configuration file");
    }
    else
        log_m.log(log_severity_t::error_k, "unable to open configuration file for writing");
}

} // namespace mau
