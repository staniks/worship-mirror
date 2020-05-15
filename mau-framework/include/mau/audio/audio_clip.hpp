#pragma once

#include "mau/base/types.hpp"
#include "mau/io/resource.hpp"

#include <SDL_mixer.h>

namespace mau {

// Forward declarations.
class audio_clip_t;
using audio_clip_handle_t = std::shared_ptr<audio_clip_t>;

// Audio clip container.
class audio_clip_t : public resource_t
{
public:
    static audio_clip_handle_t create(file_handle_t file);

    explicit audio_clip_t(file_handle_t file);
    ~audio_clip_t();

    // Returns a SDL_mixer handle.
    Mix_Chunk* sdl_handle() const;

    static audio_clip_handle_t create_resource(engine_context_t& engine, file_handle_t file);

private:
    Mix_Chunk* sdl_handle_m;
};

} // namespace mau
