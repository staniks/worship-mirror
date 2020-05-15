#include "mau/rendering/depth_test_lock.hpp"

#include "mau/rendering/renderer.hpp"

namespace mau {

depth_guard_t::depth_guard_t(renderer_t& renderer, depth_test_t depth_test) :
    renderer_m(renderer), previous_value_m(renderer.depth_test())
{
    renderer_m.set_depth_test(depth_test);
}
depth_guard_t::~depth_guard_t()
{
    renderer_m.set_depth_test(previous_value_m);
}

} // namespace mau
