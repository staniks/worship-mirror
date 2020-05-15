#pragma once

#include "mau/base/module.hpp"
#include "mau/base/state.hpp"

#include <SDL.h>

#include <memory>
#include <vector>

namespace mau {

class state_manager_t : public module_t
{
public:
    explicit state_manager_t(engine_context_t& engine);

    void push(std::unique_ptr<state_t> new_state);
    void push_clear(std::unique_ptr<state_t> new_state);
    void pop();
    void clear();

    void handle_event(const SDL_Event& event);
    void fixed_update(float delta_time);
    void variable_update(float delta_time);
    void render(float delta_time);

    bool empty();

private:
    std::vector<std::unique_ptr<state_t>> state_stack_m;
};

} // namespace mau
