#include "mau/SDL/SDL.hpp"

#include <SDL_mixer.h>

namespace mau {

sdl_context_t::sdl_context_t()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw exception_t{SDL_GetError()};
    }
}
sdl_context_t::~sdl_context_t()
{
    SDL_Quit();
}

sdl_mixer_context_t::sdl_mixer_context_t()
{
    auto flags  = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
    auto result = Mix_Init(flags);
    if((result & flags) != flags)
    {
        throw exception_t{Mix_GetError()};
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))
    {
        Mix_Quit();
        throw exception_t{Mix_GetError()};
    }
}

sdl_mixer_context_t::~sdl_mixer_context_t()
{
    // force a quit
    while(Mix_Init(0))
        Mix_Quit();

    Mix_CloseAudio();
}

} // namespace mau
