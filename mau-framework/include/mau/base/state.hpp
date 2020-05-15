#pragma once

#include "mau/base/types.hpp"
#include "mau/rendering/render_target.hpp"

#include <SDL.h>

namespace mau {

// Forward declarations.
class engine_context_t;

// Base game state class.
class state_t : non_copyable_t, non_movable_t
{
public:
    // States are given names for easier diagnostics.
    explicit state_t(engine_context_t& engine, std::string_view name);
    virtual ~state_t();

    virtual void               handle_event(const SDL_Event& event) = 0;
    virtual void               fixed_update(float delta_time)       = 0;
    virtual void               variable_update(float delta_time)    = 0;
    virtual render_target_handle_t render(float delta_time)         = 0;

    std::string_view name();

protected:
    engine_context_t& engine_m;
    std::string name_m;
};

} // namespace mau
