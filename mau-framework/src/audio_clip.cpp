#include "mau/audio/audio_clip.hpp"

#include <SDL.h>

namespace mau {

audio_clip_handle_t audio_clip_t::create(file_handle_t file)
{
    return std::make_shared<audio_clip_t>(file);
}
audio_clip_t::audio_clip_t(file_handle_t file)
{
    SDL_RWops* rwops = SDL_RWFromMem(file->data(), (int32_t)file->size());

    sdl_handle_m = Mix_LoadWAV_RW(rwops, 0);
    if(!sdl_handle_m)
    {
        throw exception_t{Mix_GetError()};
    }
}
audio_clip_t::~audio_clip_t()
{
    Mix_FreeChunk(sdl_handle_m);
}

Mix_Chunk* audio_clip_t::sdl_handle() const
{
    return sdl_handle_m;
}
audio_clip_handle_t audio_clip_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    return create(file);
}

} // namespace mau
