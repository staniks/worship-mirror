#include "worship/gameplay/world/world_info.hpp"

#include "worship/gameplay/entity.hpp"

#include <mau/io/file_reader.hpp>

#include <queue>
#include <set>

static constexpr int32_t entity_spatial_range_k = 1;

namespace mau {

world_info_handle_t world_info_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    return std::make_shared<world_info_t>(engine, file);
}
world_info_t::world_info_t(engine_context_t& engine, file_handle_t file)
{
    file_reader_t reader{file};

    header_t header;
    reader.read(&header);

    if(header.magic != world_header_magic_k)
        throw exception_t{"invalid world header magic"};

    size_m.x = header.width;
    size_m.y = header.height;

    for(uint32_t y = 0; y < header.height; ++y)
    {
        for(uint32_t x = 0; x < header.width; ++x)
        {
            tile_info_serialized_t serialized_info;
            reader.read(&serialized_info);

            tile_info_t tile_info;
            tile_info.type            = (decltype(tile_info.type))serialized_info.type;
            tile_info.texture_ceiling = serialized_info.texture_ceiling;
            tile_info.texture_floor   = serialized_info.texture_floor;
            tile_info.texture_wall    = serialized_info.texture_wall;
            tile_info.lighting        = glm::vec3{0, 0, 0};
            tile_info_m.push_back(tile_info);
        }
    }

    for(uint32_t i = 0; i < header.object_count; ++i)
    {
        object_info_serialized_t serialized_info;
        reader.read(&serialized_info);

        object_info_t object_info;
        object_info.type     = (decltype(object_info.type))serialized_info.type;
        object_info.position = glm::vec2{serialized_info.x, serialized_info.z};
        object_info_m.push_back(object_info);
    }
}

const std::vector<object_info_t>& world_info_t::object_info()
{
    return object_info_m;
}

tile_info_t* world_info_t::tile(glm::ivec2 tile_position)
{
    if(tile_position.x < 0 || tile_position.x >= size_m.x || tile_position.y < 0 || tile_position.y >= size_m.y)
        return nullptr;

    return &tile_info_m[tile_position.y * size_m.x + tile_position.x];
}

glm::vec3 world_info_t::tile_light(glm::ivec2 tile_position)
{
    auto current_tile = tile(tile_position);
    return current_tile ? current_tile->lighting : glm::vec3{0, 0, 0};
}

void world_info_t::add_entity(entity_t* entity)
{
    glm::ivec2 position{entity->position().x, entity->position().z};

    for(int32_t x = position.x - entity_spatial_range_k; x <= position.x + entity_spatial_range_k; ++x)
    {
        for(int32_t y = position.y - entity_spatial_range_k; y <= position.y + entity_spatial_range_k; ++y)
        {
            tile_info_t* current_tile = tile({x, y});
            if(!current_tile)
                continue;

            current_tile->entities.insert(entity);
        }
    }
}

void world_info_t::remove_entity(entity_t* entity)
{
    glm::ivec2 position{entity->position().x, entity->position().z};

    for(int32_t x = position.x - entity_spatial_range_k; x <= position.x + entity_spatial_range_k; ++x)
    {
        for(int32_t y = position.y - entity_spatial_range_k; y <= position.y + entity_spatial_range_k; ++y)
        {
            tile_info_t* current_tile = tile({x, y});
            if(!current_tile)
                continue;

            current_tile->entities.erase(entity);
        }
    }
}

glm::ivec2 world_info_t::size() const
{
    return size_m;
}

void world_info_t::perform_radiosity(glm::ivec2 tile_position, glm::vec3 light_source)
{
    std::queue<std::pair<glm::ivec2, glm::vec3>> frontier;
    std::set<std::tuple<int32_t, int32_t>>       visited;

    frontier.push({tile_position, light_source});

    while(!frontier.empty())
    {
        std::pair<glm::ivec2, glm::vec3> current = frontier.front();
        frontier.pop();

        glm::ivec2 current_position    = current.first;
        glm::vec3  current_light_level = current.second;

        if(visited.count({current_position.x, current_position.y}))
            continue;
        visited.emplace(current_position.x, current_position.y);

        tile_info_t* current_tile = tile(current_position);
        if(!current_tile)
            continue;

        if(is_tile_type_opaque(current_tile->type))
            continue;

        if(current_light_level == glm::vec3{0, 0, 0})
            continue;

        current_tile->lighting += current_light_level;

        static constexpr float light_level_step_k = 0.25f;
        auto next_lighting = current_light_level - glm::vec3(light_level_step_k, light_level_step_k, light_level_step_k);

        for(int i = 0; i < 3; ++i)
        {
            if(next_lighting[i] < 0.0f)
                next_lighting[i] = 0.0f;
        }

        frontier.push({current_position - glm::ivec2(-1, 0), next_lighting});
        frontier.push({current_position - glm::ivec2(1, 0), next_lighting});
        frontier.push({current_position - glm::ivec2(0, -1), next_lighting});
        frontier.push({current_position - glm::ivec2(0, 1), next_lighting});
    }
}

tile_light_level_t::tile_light_level_t(int8_t r, int8_t g, int8_t b) : r(r), g(g), b(b)
{
    if(r < 0)
        r = 0;
    if(g < 0)
        g = 0;
    if(b < 0)
        b = 0;
}

tile_light_level_t tile_light_level_t::operator+(const tile_light_level_t& rhs) const
{
    int16_t result_r = r + rhs.r;
    int16_t result_g = g + rhs.g;
    int16_t result_b = b + rhs.b;

    if(result_r < 0)
        result_r = 0;
    if(result_g < 0)
        result_g = 0;
    if(result_b < 0)
        result_b = 0;

    if(result_r > std::numeric_limits<int8_t>::max())
        result_r = std::numeric_limits<int8_t>::max();
    if(result_g > std::numeric_limits<int8_t>::max())
        result_g = std::numeric_limits<int8_t>::max();
    if(result_b > std::numeric_limits<int8_t>::max())
        result_b = std::numeric_limits<int8_t>::max();

    return tile_light_level_t{static_cast<int8_t>(result_r), static_cast<int8_t>(result_g), static_cast<int8_t>(result_b)};
}

bool tile_light_level_t::operator==(const tile_light_level_t& rhs) const
{
    return r == rhs.r && g == rhs.g && b == rhs.b;
}

tile_light_level_t tile_light_level_t::create_decreased() const
{
    static constexpr int8_t light_level_step_k = 32;

    tile_light_level_t data{*this};

    data.r -= light_level_step_k;
    data.g -= light_level_step_k;
    data.b -= light_level_step_k;

    if(data.r < 0)
        data.r = 0;
    if(data.g < 0)
        data.g = 0;
    if(data.b < 0)
        data.b = 0;

    return data;
}

glm::vec4 tile_light_level_t::to_color() const
{
    static constexpr glm::vec4 ambient_color_k{0.01f, 0.01f, 0.01f, 1.0f};
    static constexpr float     light_level_max_float_k = 127.0f;

    glm::vec4 normalized_color{r / light_level_max_float_k, g / light_level_max_float_k, b / light_level_max_float_k, 1.0f};

    return glm::vec4{ambient_color_k.x + normalized_color.x * (1.0f - ambient_color_k.x),
                     ambient_color_k.y + normalized_color.y * (1.0f - ambient_color_k.y),
                     ambient_color_k.z + normalized_color.z * (1.0f - ambient_color_k.z),
                     1.0f};
}

} // namespace mau
