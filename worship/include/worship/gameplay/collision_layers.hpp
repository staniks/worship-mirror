#pragma once

#include <cstdint>

namespace mau {

using collision_mask_t = uint64_t;

namespace collision_layers {

inline static constexpr collision_mask_t player_k     = 1ULL;
inline static constexpr collision_mask_t pickup_k     = 1ULL << 1;
inline static constexpr collision_mask_t enemy_k      = 1ULL << 2;
inline static constexpr collision_mask_t projectile_k = 1ULL << 3;

} // namespace collision_layers

} // namespace mau
