#pragma once

#include "worship/gameplay/messages.hpp"
#include "worship/gameplay/screen_flash.hpp"

namespace mau {
class state_gameplay_t;

class event_callback_t
{
public:
    explicit event_callback_t(state_gameplay_t& state);

    void display_message(message_t message);
    void flash_screen(screen_flash_t screen_flash);

private:
    state_gameplay_t& state_m;
};
} // namespace mau
