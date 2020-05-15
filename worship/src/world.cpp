#include "worship/gameplay/world/world.hpp"

#include "worship/gameplay/entities/pickup.hpp"
#include "worship/gameplay/entities/enemy.hpp"

#include <mau/base/engine_context.hpp>
#include <mau/io/resource_cache.hpp>
#include <mau/math/algorithms.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <functional>
#include <set>

namespace mau {

std::vector<vertex_t> create_wireframe_cube()
{
    static std::array<vertex_t, 8> cube_vertices = {
        vertex_t{glm::vec3{-0.5f, 0.5f, -0.5f}}, // 0
        vertex_t{glm::vec3{-0.5f, 0.5f, 0.5f}},  // 1
        vertex_t{glm::vec3{0.5f, 0.5f, 0.5f}},   // 2
        vertex_t{glm::vec3{0.5f, 0.5f, -0.5f}},  // 3

        vertex_t{glm::vec3{-0.5f, -0.5f, -0.5f}}, // 4
        vertex_t{glm::vec3{-0.5f, -0.5f, 0.5f}},  // 5
        vertex_t{glm::vec3{0.5f, -0.5f, 0.5f}},   // 6
        vertex_t{glm::vec3{0.5f, -0.5f, -0.5f}},  // 7
    };

    int lines[] = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

    std::vector<vertex_t> mesh_vertices;
    for(auto index: lines)
        mesh_vertices.push_back(cube_vertices[index]);
    return mesh_vertices;
}

world_t::world_t(engine_context_t& engine, event_callback_t& event_callback, difficulty_t difficulty) :
    engine_m(engine),
    event_callback_m(event_callback),
    world_info_m(engine, engine.resource_cache().load_file("level1.lvl")),
    pickup_descriptors_m(create_pickup_descriptors(engine)),
    weapon_descriptors_m(create_weapon_descriptors(engine)),
    projectile_descriptors_m(create_projectile_descriptors(engine)),
    particle_descriptors_m(create_particle_descriptors(engine)),
    enemy_descriptors_m(create_enemy_descriptors(engine))
{
    tileset_texture_diffuse_m  = engine.resource_cache().resource<texture_t>("tileset-diffuse.tex");
    tileset_texture_emission_m = engine.resource_cache().resource<texture_t>("tileset-emission.tex");
    wireframe_texture_m        = engine.resource_cache().resource<texture_t>("white.tex");

    // Create the wireframe cube vertex object.
    auto wireframe_cube_vertices = create_wireframe_cube();
    wireframe_cube_m =
        vertex_object_t::create(vertex_primitive_t::line_k, vertex_object_mode_t::static_k, wireframe_cube_vertices);

    // Spawn entities.
    for(auto& object_info: world_info_m.object_info())
    {
        switch(object_info.type)
        {
            case object_type_t::player_spawn_k: {
                if(player_m)
                    throw exception_t{"duplicate player spawn encountered - level likely corrupted"};

                glm::vec3 position{object_info.position.x, 0.5f, object_info.position.y};
                auto      player = std::make_unique<player_t>(*this, position, difficulty);

                player_m = player.get();

                add_entity(std::move(player));
            }
            break;
            case object_type_t::light_source_white_k: {
                world_info_m.perform_radiosity(object_info.position, glm::vec3{1.0f, 1.0f, 1.0f});
            }
            break;
            case object_type_t::light_source_red_k: {
                world_info_m.perform_radiosity(object_info.position, glm::vec3{1.0f, 0.0f, 0.0f});
            }
            break;
            case object_type_t::light_source_green_k: {
                world_info_m.perform_radiosity(object_info.position, glm::vec3{0.0f, 1.0f, 0.0f});
            }
            break;
            case object_type_t::light_source_blue_k: {
                world_info_m.perform_radiosity(object_info.position, glm::vec3{0.0f, 0.0f, 1.0f});
            }
            break;
            case object_type_t::light_source_orange_k: {
                world_info_m.perform_radiosity(object_info.position, glm::vec3{1.0f, 0.75f, 0.5f});
            }
            break;
            case object_type_t::enemy_light_k:
            {
                glm::vec3 position{ object_info.position.x, 0, object_info.position.y };
                add_entity(std::make_unique<enemy_t>(*this, position, enemy_descriptors_m[enemy_type_t::light_k]));
            }
            break;
            case object_type_t::enemy_medium_k:
            {
                glm::vec3 position{ object_info.position.x, 0, object_info.position.y };
                add_entity(std::make_unique<enemy_t>(*this, position, enemy_descriptors_m[enemy_type_t::medium_k]));
            }
            break;
            case object_type_t::enemy_heavy_k:
            {
                glm::vec3 position{ object_info.position.x, 0, object_info.position.y };
                add_entity(std::make_unique<enemy_t>(*this, position, enemy_descriptors_m[enemy_type_t::heavy_k]));
            }
            break;
            default: {
                glm::vec3 position{object_info.position.x, 0, object_info.position.y};

                std::unique_ptr<entity_t> entity{nullptr};

                // Check if it's a pickup.
                auto pickup_it = pickup_descriptors_m.find(object_info.type);
                if(pickup_it != pickup_descriptors_m.end())
                {
                    entity = std::make_unique<pickup_t>(*this, position, pickup_descriptors_m[object_info.type]);
                }

                if(entity)
                {
                    add_entity(std::move(entity));
                }
                else
                {
                    const auto error = fmt::format("unknown object type: {}", object_info.type);
                    engine_m.log().log(error);
                }
            }
        }
    }

    // Generate level chunks.
    for(int32_t z = 0; z < world_info_m.size().y; z += chunk_size_k)
    {
        for(int32_t x = 0; x < world_info_m.size().x; x += chunk_size_k)
        {
            chunks_m.insert({chunk_position({x, z}), chunk_t{*this, {x, z}}});
        }
    }
}
void world_t::fixed_update(float delta_time)
{
    for(auto& entity: entities_m)
    {
        entity->fixed_update(delta_time);
    }

    // Clean up entities marked for garbage collection.
    entities_m.erase(std::remove_if(entities_m.begin(),
                                    entities_m.end(),
                                    [](const std::unique_ptr<entity_t>& entity) { return entity->destroyed(); }),
                     entities_m.end());
}
void world_t::variable_update(float delta_time)
{
    for(auto& entity: entities_m)
    {
        entity->variable_update(delta_time);
    }

    // Fade existing lights.
    static constexpr float light_fade_speed_k = 4.0f;
    for(auto& light: dynamic_lights_m)
    {
        if(light->destroyed)
        {
            light->color -= glm::vec3{light_fade_speed_k, light_fade_speed_k, light_fade_speed_k} * delta_time;

            // glm::max here is needed here due to how glm::clamp works internally.
            light->color = glm::clamp(light->color, {0, 0, 0}, glm::max({0, 0, 0}, light->color));
        }
    }

    // Clean up lights which have faded out.
    dynamic_lights_m.erase(std::remove_if(dynamic_lights_m.begin(),
                                          dynamic_lights_m.end(),
                                          [](const std::unique_ptr<dynamic_light_t>& light) {
                                              return light->destroyed && glm::length(light->color) == 0;
                                          }),
                           dynamic_lights_m.end());
}
void world_t::render(float delta_time, shader_handle_t ubershader, const glm::mat4& projection, const glm::mat4& view)
{
    auto& renderer = engine_m.renderer();

    //=========================================================================
    // Occlusion query.
    //=========================================================================
    // Cast rays and keep track of chunks they touch. Then render these chunks only.

    std::set<entity_t*>            visible_entities;
    std::unordered_set<glm::ivec2> visible_chunks;

    const float fov_step = player_m->fov() / (engine_m.viewport_size().x / 2.0f);

    for(float i = 0; i < engine_m.viewport_size().x / 2.0f; i += 1.0f)
    {
        float angle = fov_step * i;

        auto direction_right = glm::rotate(player_m->direction(), angle, {0, 1, 0});
        auto direction_left  = glm::rotate(player_m->direction(), -angle, {0, 1, 0});

        raycast_walls(player_m->camera_position(), direction_right, &visible_chunks, &visible_entities);
        raycast_walls(player_m->camera_position(), direction_left, &visible_chunks, &visible_entities);
    }

    //=========================================================================
    // Dynamic lighting.
    //=========================================================================

    if(engine_m.config().dynamic_lighting)
    {
        ubershader->set_uniform_bool(shader_uniform_t::enable_lighting_k, true);

        // Fetch the most relevant lights.
        auto lights = relevant_lights(player_m->position());

        // Prepare light data for the shader.
        std::vector<glm::vec3> light_positions;
        std::vector<glm::vec3> light_colors;
        std::vector<float>     light_radii;

        for(auto& light: lights)
        {
            light_positions.push_back(light->position);
            light_colors.push_back(light->color);
            light_radii.push_back(light->radius);
        }

        // TODO: There must be a better way to do this, we're grinding really hard against the uniform limit here.
        ubershader->set_uniform_vec3_array(shader_uniform_t::light_positions_k, light_positions);
        ubershader->set_uniform_vec3_array(shader_uniform_t::light_colors_k, light_colors);
        ubershader->set_uniform_float_array(shader_uniform_t::light_radii_k, light_radii);
        ubershader->set_uniform_int(shader_uniform_t::light_count_k, (int32_t)lights.size());
    }

    //=========================================================================
    // Chunk rendering.
    //=========================================================================

    ubershader->set_uniform_mat4(shader_uniform_t::model_matrix_k, glm::identity<glm::mat4>());
    ubershader->set_uniform_mat4(shader_uniform_t::view_matrix_k, view);
    ubershader->set_uniform_mat4(shader_uniform_t::projection_matrix_k, projection);

    renderer.bind_texture(tileset_texture_diffuse_m, 0);
    renderer.bind_texture(tileset_texture_emission_m, 1);

    for(auto chunk_coordinates: visible_chunks)
    {
        auto it = chunks_m.find(chunk_coordinates);
        if(it == chunks_m.end())
            continue;

        const auto& chunk = it->second;
        engine_m.renderer().render_vertex_object(chunk.vertex_object());
    }

    //=========================================================================
    // Entity rendering.
    //=========================================================================

    ubershader->set_uniform_mat4(shader_uniform_t::view_matrix_k, view);
    ubershader->set_uniform_mat4(shader_uniform_t::projection_matrix_k, projection);

    for(auto& entity: visible_entities)
    {
        // Render wireframe box.
        if(false)
        {
            glm::mat4 model = glm::translate(entity->position()) * glm::scale(entity->bounding_box());

            renderer.bind_texture(wireframe_texture_m, 0);
            renderer.bind_texture(wireframe_texture_m, 1);
            ubershader->set_uniform_mat4(shader_uniform_t::model_matrix_k, model);

            if(entity != player_m)
                renderer.render_vertex_object(wireframe_cube_m);
        }

        entity->render(renderer, ubershader, delta_time);
    }

    ubershader->set_uniform_bool(shader_uniform_t::enable_lighting_k, false);
}
engine_context_t & world_t::engine()
{
    return engine_m;
}
event_callback_t& world_t::event_callback()
{
    return event_callback_m;
}
tile_info_t* world_t::tile(glm::ivec2 tile_position)
{
    return world_info_m.tile(tile_position);
}

void world_t::add_entity(std::unique_ptr<entity_t> entity)
{
    entities_m.push_back(std::move(entity));
}

dynamic_light_t* world_t::add_dynamic_light(dynamic_light_t light)
{
    dynamic_lights_m.push_back(std::make_unique<dynamic_light_t>(light));
    return dynamic_lights_m.back().get();
}

void world_t::spatial_entity_insert(entity_t* entity)
{
    world_info_m.add_entity(entity);
}

void world_t::spatial_entity_remove(entity_t* entity)
{
    world_info_m.remove_entity(entity);
}

std::vector<dynamic_light_t*> world_t::relevant_lights(glm::vec3 position) const
{
    std::vector<dynamic_light_t*> relevant_lights;

    // We sort lights by the relevance score, which is distance to the player multiplied by radius.
    // TODO: maybe add color length here as well?
    auto light_sorter = [&](dynamic_light_t* a, dynamic_light_t* b) {
        float distance_a = glm::length(player_m->position() - a->position);
        float distance_b = glm::length(player_m->position() - b->position);
        return distance_a * a->radius < distance_b * b->radius;
    };

    std::set<dynamic_light_t*, std::function<bool(dynamic_light_t*, dynamic_light_t*)>> sorted_lights(light_sorter);

    for(auto& light: dynamic_lights_m)
    {
        sorted_lights.insert(light.get());
    }

    static constexpr size_t max_light_count_k = 32;
    for(auto& light: sorted_lights)
    {
        if(relevant_lights.size() < max_light_count_k)
            relevant_lights.push_back(light);
    }

    return relevant_lights;
}

glm::vec4 world_t::light(glm::vec2 position)
{
    glm::vec3 light_current = world_info_m.tile_light(position);

    int64_t x = (int64_t)position.x;
    int64_t y = (int64_t)position.y;

    auto light_north     = world_info_m.tile_light({x, y - 1});
    auto light_northwest = world_info_m.tile_light({x - 1, y - 1});
    auto light_west      = world_info_m.tile_light({x - 1, y});
    auto light_southwest = world_info_m.tile_light({x - 1, y + 1});
    auto light_south     = world_info_m.tile_light({x, y + 1});
    auto light_southeast = world_info_m.tile_light({x + 1, y + 1});
    auto light_east      = world_info_m.tile_light({x + 1, y});
    auto light_northeast = world_info_m.tile_light({x + 1, y - 1});

    // TODO: Bake this information into tile object instead of calculating every frame.
    auto northwest = (light_north + light_west + light_northwest + light_current) / 4.0f;
    auto northeast = (light_north + light_east + light_northeast + light_current) / 4.0f;
    auto southwest = (light_west + light_southwest + light_south + light_current) / 4.0f;
    auto southeast = (light_south + light_southeast + light_east + light_current) / 4.0f;

    glm::vec2 interpolation_coefficient = position - glm::vec2(glm::ivec2(position));

    auto north_interpolated = glm::mix(northwest, northeast, interpolation_coefficient.x);
    auto south_interpolated = glm::mix(southwest, southeast, interpolation_coefficient.x);
    auto final_interpolated = glm::mix(north_interpolated, south_interpolated, interpolation_coefficient.y);

    // Apply ambient color.
    static constexpr glm::vec4 ambient_color_k{0.25f, 0.25f, 0.25f, 1.0f};
    return ambient_color_k + glm::vec4{final_interpolated, 1.0f} * (glm::vec4(1, 1, 1, 1) - ambient_color_k);
}

std::optional<glm::vec3> world_t::raycast(glm::vec3 position, glm::vec3 direction, uint64_t collision_mask, entity_t* ignore_entity, entity_t** hit_entity)
{
    // Raycast both wall and entities. Return whichever point the ray hits closer.
    std::set<entity_t*> entities;

    if(hit_entity)
        *hit_entity = nullptr;

    entity_t* entity = nullptr;

    auto hit_wall     = raycast_walls(position, direction, nullptr, &entities);

    // Remove ignored entity from set.
    if(ignore_entity)
        entities.erase(ignore_entity);

    auto hit_entities = raycast_entities(entities, position, direction, collision_mask, &entity);

    if(!hit_wall)
        return std::optional<glm::vec3>{};

    if(!hit_entities)
        return hit_wall;
    else
    {
        if(glm::length(*hit_wall - position) < glm::length(*hit_entities - position))
            return hit_wall;
        else
        {
            if(hit_entity)
                *hit_entity = entity;
            return hit_entities;
        }
    }
}

std::optional<glm::vec3> world_t::raycast_walls(glm::vec3                       position,
                                                glm::vec3                       direction,
                                                std::unordered_set<glm::ivec2>* visited_chunks,
                                                std::set<entity_t*>*            visited_entities)
{
    direction.y = 0;
    direction   = glm::normalize(direction);

    glm::ivec2 tile_position{position.x, position.z};

    const glm::vec2 delta_distance{std::abs(1 / direction.x), std::abs(1 / direction.z)};

    bool wall_hit{false};

    tile_info_t* starting_tile = tile(tile_position);

    if(visited_chunks)
        visited_chunks->insert(chunk_position(tile_position));

    if(visited_entities)
        visited_entities->insert(starting_tile->entities.begin(), starting_tile->entities.end());

    while(!wall_hit)
    {
        glm::vec2  side_distance{};
        glm::ivec2 step_direction{};

        if(direction.x < 0)
        {
            side_distance.x  = (position.x - tile_position.x) * delta_distance.x;
            step_direction.x = -1;
        }
        else
        {
            side_distance.x  = (tile_position.x + 1.0f - position.x) * delta_distance.x;
            step_direction.x = 1;
        }

        if(direction.z < 0)
        {
            side_distance.y  = (position.z - tile_position.y) * delta_distance.y;
            step_direction.y = -1;
        }
        else
        {
            side_distance.y  = (tile_position.y + 1.0f - position.z) * delta_distance.y;
            step_direction.y = 1;
        }

        if(side_distance.x < side_distance.y)
        {
            position += direction * side_distance.x;
            tile_position.x += step_direction.x;
        }
        else
        {
            position += direction * side_distance.y;
            tile_position.y += step_direction.y;
        }

        tile_info_t* current_tile = tile(tile_position);
        if(!current_tile)
        {
            wall_hit = true;
            break;
        }

        if(is_tile_type_opaque(current_tile->type))
            wall_hit = true;

        if(visited_chunks != nullptr)
            visited_chunks->insert(chunk_position(tile_position));

        if(visited_entities)
            visited_entities->insert(current_tile->entities.begin(), current_tile->entities.end());
    }

    return position;
}

std::optional<glm::vec3>
world_t::raycast_entities(std::set<entity_t*>& entities, glm::vec3 position, glm::vec3 direction, uint64_t collision_mask, entity_t** hit_entity)
{
    std::optional<glm::vec3> nearest_point;

    direction.y = 0;
    direction   = glm::normalize(direction);

    for(auto& entity: entities)
    {
        if((entity->collision_layer() & collision_mask) == 0)
            continue;

        // Make height huge, since we're casting in 2D.
        auto bounding_box = entity->bounding_box();
        bounding_box.y    = 1024;

        glm::vec3 point;

        if(math::aabb_ray_intersect(entity->position(), bounding_box, position, direction, point))
        {
            if(!nearest_point || glm::length(position - point) < glm::length(*nearest_point - position))
            {
                nearest_point = point;

                if(hit_entity)
                    *hit_entity = entity;
            }
        }
    }
    return nearest_point;
}

player_t* world_t::player()
{
    return player_m;
}

void world_t::play_sound(audio_clip_handle_t clip)
{
    if (clip)
        engine_m.audio().play_clip(clip);
}

void world_t::play_sound_at(audio_clip_handle_t clip, glm::vec3 position)
{
    if(clip)
        engine_m.audio().play_clip(clip, position);
}

const std::map<weapon_type_t, weapon_descriptor_t>& world_t::weapon_descriptors() const
{
    return weapon_descriptors_m;
}

const std::map<projectile_type_t, projectile_descriptor_t>& world_t::projectile_descriptors() const
{
    return projectile_descriptors_m;
}

const std::map<particle_type_t, particle_descriptor_t>& world_t::particle_descriptors() const
{
    return particle_descriptors_m;
}

glm::ivec2 world_t::chunk_position(glm::ivec2 tile_position)
{
    glm::ivec2 chunk_position;

    for(auto i = 0; i < 2; ++i)
    {
        if(tile_position[i] >= 0)
            chunk_position[i] = tile_position[i] / chunk_size_k;
        else
            chunk_position[i] = ((tile_position[i] + 1) / chunk_size_k) - 1;
    }

    return chunk_position;
}

} // namespace mau
