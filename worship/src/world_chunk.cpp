#include "worship/gameplay/world/world_chunk.hpp"

#include "worship/gameplay/world/world.hpp"

namespace mau {

enum class tile_side_t
{
    top_k,
    bottom_k,
    forward_k,
    back_k,
    left_k,
    right_k
};

enum class tile_vertex_id_t
{
    bottom_northwest_k,
    bottom_northeast_k,
    bottom_southwest_k,
    bottom_southeast_k,

    top_northwest_k,
    top_northeast_k,
    top_southwest_k,
    top_southeast_k
};

// Tile vertex positions which then make up sides. References tile_vertex_id_t.
inline static constexpr std::array<glm::vec3, 8> tile_vertex_positions_k{
    glm::vec3{0.0f, 0.0f, 0.0f},
    glm::vec3{1.0f, 0.0f, 0.0f},
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{1.0f, 0.0f, 1.0f},

    glm::vec3{0.0f, 1.0f, 0.0f},
    glm::vec3{1.0f, 1.0f, 0.0f},
    glm::vec3{0.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
};

// Tile texture coordinates. Order is always 0->1->2->3.
inline static constexpr std::array<glm::vec2, 4> tile_vertex_uvs_k{
    glm::vec2{0.0f, 0.0f},
    glm::vec2{1.0f, 0.0f},
    glm::vec2{0.0f, 1.0f},
    glm::vec2{1.0f, 1.0f},
};

// Indices reference tile_side_t.
inline static constexpr std::array<std::array<int32_t, 6>, 6> side_vertex_indices_k = {
    std::array<int32_t, 6>{6, 4, 7, 4, 5, 7},
    std::array<int32_t, 6>{0, 2, 1, 2, 3, 1},
    std::array<int32_t, 6>{6, 2, 7, 2, 3, 7},
    std::array<int32_t, 6>{5, 1, 4, 1, 0, 4},
    std::array<int32_t, 6>{4, 0, 6, 0, 2, 6},
    std::array<int32_t, 6>{7, 3, 5, 3, 1, 5},
};

std::array<int32_t, 6> vertex_uv_indices = {0, 2, 1, 2, 3, 1};

chunk_t::chunk_t(world_t& world, glm::ivec2 position) : world_m(world), position_m(position)
{
    std::vector<vertex_t> vertex_data;

    for(auto y = position_m.y; y < position_m.y + chunk_size_k; ++y)
    {
        for(auto x = position_m.x; x < position_m.x + chunk_size_k; ++x)
        {
            tile_info_t* tile = world_m.tile({x, y});
            glm::vec3    offset{x, 0, y};

            auto create_vertices = [&](tile_side_t side) {
                auto vertex_indices = side_vertex_indices_k[static_cast<size_t>(side)];

                tile_texture_t tile_texture = tile->texture_wall;
                if(side == tile_side_t::top_k)
                    tile_texture = tile->texture_ceiling;
                else if(side == tile_side_t::bottom_k)
                    tile_texture = tile->texture_floor;

                for(int i = 0; i < vertex_indices.size(); ++i)
                {
                    const auto vertex_index = vertex_indices[i];
                    const auto position     = tile_vertex_positions_k[vertex_index] + offset;

                    static constexpr glm::vec2 uv_size_k{
                        1.0f / tileset_size_k,
                        1.0f / tileset_size_k,
                    };
                    const glm::vec2 tile_uv{(float)(tile_texture % tileset_size_k) / tileset_size_k,
                                            (float)(tile_texture / tileset_size_k) / tileset_size_k};
                    auto            tile_uv_factors = tile_vertex_uvs_k[vertex_uv_indices[i]];

                    glm::vec2 uv{
                        tile_uv.x + tile_uv_factors.x * uv_size_k.x,
                        tile_uv.y + tile_uv_factors.y * uv_size_k.y,
                    };

                    glm::vec4 color{};

                    switch(static_cast<tile_vertex_id_t>(vertex_index))
                    {
                        case tile_vertex_id_t::bottom_northwest_k:
                        case tile_vertex_id_t::top_northwest_k: color = world_m.light({x, y}); break;
                        case tile_vertex_id_t::bottom_northeast_k:
                        case tile_vertex_id_t::top_northeast_k: color = world_m.light({x + 1, y}); break;
                        case tile_vertex_id_t::bottom_southwest_k:
                        case tile_vertex_id_t::top_southwest_k: color = world_m.light({x, y + 1}); break;
                        case tile_vertex_id_t::bottom_southeast_k:
                        case tile_vertex_id_t::top_southeast_k: color = world_m.light({x + 1, y + 1}); break;
                    }

                    vertex_data.emplace_back(position, uv, color);
                }
            };

            if(tile->type == tile_type_t::air_k)
            {
                create_vertices(tile_side_t::bottom_k);
                create_vertices(tile_side_t::top_k);
            }
            else if(tile->type == tile_type_t::wall_k)
            {
                auto tile_south = world_m.tile({x, y + 1});
                auto tile_north = world_m.tile({x, y - 1});
                auto tile_west  = world_m.tile({x - 1, y});
                auto tile_east  = world_m.tile({x + 1, y});

                if(tile_south && !is_tile_type_opaque(tile_south->type))
                    create_vertices(tile_side_t::forward_k);

                if(tile_north && !is_tile_type_opaque(tile_north->type))
                    create_vertices(tile_side_t::back_k);

                if(tile_west && !is_tile_type_opaque(tile_west->type))
                    create_vertices(tile_side_t::left_k);

                if(tile_east && !is_tile_type_opaque(tile_east->type))
                    create_vertices(tile_side_t::right_k);
            }
        }
    }

    vertex_object_m = vertex_object_t::create(vertex_primitive_t::triangle_k, vertex_object_mode_t::static_k, vertex_data);
}

vertex_object_handle_t chunk_t::vertex_object() const
{
    return vertex_object_m;
}

} // namespace mau
