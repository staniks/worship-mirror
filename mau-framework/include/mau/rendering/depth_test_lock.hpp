#pragma once

#include "mau/base/types.hpp"

namespace mau {

// Forward declarations.
class renderer_t;

enum class depth_test_t
{
    enabled_k,
    disabled_k
};

// Upon creation, set the depth test to specified value. Upon destruction, restore old value.
// TODO: Maybe create a generalized class for this kind of thing, not just for depth test?
class depth_guard_t : non_copyable_t, non_movable_t
{
public:
    depth_guard_t(renderer_t& renderer, depth_test_t depth_test);
    ~depth_guard_t();

private:
    renderer_t&  renderer_m;
    depth_test_t previous_value_m;
};
} // namespace mau
