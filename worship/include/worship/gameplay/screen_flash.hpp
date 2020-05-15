#pragma once

#include "mau/base/types.hpp"
#include "mau/math/vector.hpp"

#include <frozen/map.h>

namespace mau {

enum class screen_flash_t
{
    none_k,
    pickup_k,
    pain_k
};

namespace detail {
inline static constexpr std::pair<screen_flash_t, glm::vec3> flash_mapping_array_k[] = {
    {screen_flash_t::none_k, glm::vec3{0.0f, 0.0f, 0.0f}},
    {screen_flash_t::pickup_k, glm::vec3{1.0f, 1.0f, 1.0f}},
    {screen_flash_t::pain_k, glm::vec3{1.0f, 0.0f, 0.0f}}};
}

inline static constexpr auto screen_flashes_k = frozen::make_map(detail::flash_mapping_array_k);

} // namespace mau
