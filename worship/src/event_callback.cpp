#include "worship/gameplay/event_callback.hpp"

#include "worship/states/state_gameplay.hpp"

namespace mau {

event_callback_t::event_callback_t(state_gameplay_t& state) : state_m(state)
{
}
void event_callback_t::display_message(message_t message)
{
    state_m.display_message(message);
}

void event_callback_t::flash_screen(screen_flash_t screen_flash)
{
    state_m.flash_screen(screen_flash);
}

} // namespace mau
