#include "mau/base/timer.hpp"

namespace mau {

    timer_t::timer_t()
    {
        reset();
    }

    void timer_t::reset()
    {
        start_time_m = clock_m.now();
    }

} // namespace mau
