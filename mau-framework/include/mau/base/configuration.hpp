#pragma once

#include "mau/base/types.hpp"

namespace mau {

#pragma pack(push, 1)
// Configuration struct. Packed for serialization/deserialization.
struct config_t
{
    uint8_t window_scale;
    bool    fullscreen;
    bool    vsync;
    bool    bloom;
    bool    dynamic_lighting;
};
#pragma pack(pop)

} // namespace mau
