#pragma once

#include <mau/io/resource.hpp>
#include <mau/math/vector.hpp>

#include <set>

namespace mau {

class world_info_t;
using world_info_handle_t = std::shared_ptr<world_info_t>;

enum class tile_type_t : uint8_t
{
    air_k = 0,
    wall_k
};

struct tile_light_level_t
{
    tile_light_level_t() = default;
    tile_light_level_t(int8_t r, int8_t g, int8_t b);

    tile_light_level_t operator+(const tile_light_level_t& rhs) const;
    bool               operator==(const tile_light_level_t& rhs) const;

    tile_light_level_t create_decreased() const;
    glm::vec4          to_color() const;

    int8_t r, g, b;
};

inline static constexpr uint64_t world_header_magic_k = 7815552959266505037ULL;

#pragma pack(push, 1)
struct header_t
{
    uint64_t magic;
    uint32_t width;
    uint32_t height;
    uint32_t object_count;
};
struct tile_info_serialized_t
{
    uint8_t  type;
    uint16_t texture_wall;
    uint16_t texture_floor;
    uint16_t texture_ceiling;
};
struct object_info_serialized_t
{
    uint8_t type;
    float   x, z;
};
#pragma pack(pop)

using tile_texture_t = uint16_t;

class entity_t;

struct tile_info_t
{
    tile_type_t    type;
    tile_texture_t texture_wall;
    tile_texture_t texture_floor;
    tile_texture_t texture_ceiling;
    glm::vec3      lighting;

    std::set<entity_t*> entities;
};

enum class object_type_t : uint8_t
{
    player_spawn_k,
    light_source_white_k,
    light_source_red_k,
    light_source_green_k,
    light_source_blue_k,
    light_source_orange_k,
    armor_k,
    armor_shard_k,
    health_k,
    shells_k,
    shotgun_k,
    grenades_k,
    grenade_launcher_k,
    enemy_light_k,
    plasma_rifle_k,
    cells_k,
    enemy_medium_k,
    enemy_heavy_k
};

struct object_info_t
{
    object_type_t type;
    glm::vec2     position;
};

inline constexpr bool is_tile_type_opaque(tile_type_t tile_type)
{
    return tile_type != tile_type_t::air_k;
}

inline static constexpr uint64_t chunk_size_k   = 16;
inline static constexpr uint64_t tileset_size_k = 16;

class world_info_t : public resource_t
{
public:
    static world_info_handle_t create_resource(engine_context_t& engine, file_handle_t file);

    world_info_t(engine_context_t& engine, file_handle_t file);

    const std::vector<object_info_t>& object_info();

    tile_info_t* tile(glm::ivec2 tile_position);
    glm::vec3    tile_light(glm::ivec2 tile_position);

    void add_entity(entity_t* entity);
    void remove_entity(entity_t* entity);

    glm::ivec2 size() const;

    void perform_radiosity(glm::ivec2 tile_position, glm::vec3 light_source);

private:
    glm::ivec2 size_m;

    std::vector<tile_info_t>   tile_info_m;
    std::vector<object_info_t> object_info_m;
};

} // namespace mau
