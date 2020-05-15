#pragma once

#include "mau/base/types.hpp"

#include <string_view>

#include <frozen/map.h>

namespace mau {

enum class message_t
{
    none_k,
    pickup_armor_k,
    pickup_health_k,
    pickup_shells_k,
    pickup_shotgun_k,
    pickup_grenades_k,
    pickup_grenade_launcher_k,
    pickup_plasma_rifle_k,
    pickup_cells_k,
    death_k,
};

namespace detail {
inline static constexpr std::pair<message_t, std::string_view> message_mapping_array_k[] = {
    {message_t::none_k, ""},
    {message_t::pickup_armor_k, "You pick up body armor."},
    {message_t::pickup_health_k, "You pick up health elixir."},
    {message_t::pickup_shells_k, "You pick up shotgun shells."},
    {message_t::pickup_shotgun_k, "You pick up the shotgun."},
    {message_t::pickup_grenades_k, "You pick up grenades."},
    {message_t::pickup_grenade_launcher_k, "You pick up the grenade launcher."},
    {message_t::pickup_plasma_rifle_k, "You pick up the plasma rifle."},
    {message_t::pickup_cells_k, "You pick up plasma cells."},
    {message_t::death_k, "You died."},
};
}

inline static constexpr auto messages_k = frozen::make_map(detail::message_mapping_array_k);

} // namespace mau
