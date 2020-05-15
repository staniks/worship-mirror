#pragma once

#include "mau/base/types.hpp"

#include <SDL.h>

namespace mau {

// SDL context wrapper.
class sdl_context_t : non_copyable_t, non_movable_t
{
public:
    explicit sdl_context_t();
    ~sdl_context_t();
};

// SDL mixer context wrapper.
class sdl_mixer_context_t : non_copyable_t, non_movable_t
{
public:
    explicit sdl_mixer_context_t();
    ~sdl_mixer_context_t();
};

} // namespace mau
