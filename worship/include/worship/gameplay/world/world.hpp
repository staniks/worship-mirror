#pragma once

#include "worship/gameplay/entities/player.hpp"
#include "worship/gameplay/event_callback.hpp"
#include "worship/gameplay/world/world_chunk.hpp"

#include <mau/audio/audio_clip.hpp>
#include <mau/base/types.hpp>
#include <mau/math/vector.hpp>
#include <mau/rendering/shader.hpp>
#include <mau/rendering/texture.hpp>

#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mau {

class engine_context_t;

struct dynamic_light_t
{
    glm::vec3 position{};
    glm::vec3 color{};
    float     radius{};
    bool      destroyed{false};
};

class world_t : non_copyable_t, non_movable_t
{
public:
    world_t(engine_context_t& engine, event_callback_t& event_callback, difficulty_t difficulty);

    void fixed_update(float delta_time);
    void variable_update(float delta_time);
    void render(float delta_time, shader_handle_t ubershader, const glm::mat4& projection, const glm::mat4& view);

    engine_context_t& engine();
    event_callback_t& event_callback();

    tile_info_t* tile(glm::ivec2 tile_position);

    void             add_entity(std::unique_ptr<entity_t> entity);
    dynamic_light_t* add_dynamic_light(dynamic_light_t light);

    void spatial_entity_insert(entity_t* entity);
    void spatial_entity_remove(entity_t* entity);

    std::vector<dynamic_light_t*> relevant_lights(glm::vec3 position) const;

    glm::vec4 light(glm::vec2 position);

    // Perform raycast with collision mask, while ignoring specified entity.
    std::optional<glm::vec3> raycast(glm::vec3 position,
                                     glm::vec3 direction,
                                     uint64_t collision_mask,
                                     entity_t* ignore_entity,
                                     entity_t** hit_entity);

    // Raycast just walls, keeping track of visited chunks and entities.
    std::optional<glm::vec3> raycast_walls(glm::vec3                       position,
                                           glm::vec3                       direction,
                                           std::unordered_set<glm::ivec2>* visited_chunks,
                                           std::set<entity_t*>*            visited_entities);

    // Raycast just specified entities.
    std::optional<glm::vec3> raycast_entities(std::set<entity_t*>& entities,
                                              glm::vec3            position,
                                              glm::vec3            direction,
                                              uint64_t             collision_mask,
                                              entity_t**           hit_entity);

    player_t* player();

    void play_sound(audio_clip_handle_t clip);
    void play_sound_at(audio_clip_handle_t clip, glm::vec3 position);

    const std::map<weapon_type_t, weapon_descriptor_t>&         weapon_descriptors() const;
    const std::map<projectile_type_t, projectile_descriptor_t>& projectile_descriptors() const;
    const std::map<particle_type_t, particle_descriptor_t>&     particle_descriptors() const;

private:
    glm::ivec2 chunk_position(glm::ivec2 tile_position);

    engine_context_t& engine_m;
    event_callback_t& event_callback_m;

    world_info_t                            world_info_m;
    std::unordered_map<glm::ivec2, chunk_t> chunks_m;

    texture_handle_t tileset_texture_diffuse_m;
    texture_handle_t tileset_texture_emission_m;
    texture_handle_t wireframe_texture_m;

    std::vector<std::unique_ptr<entity_t>> entities_m;
    player_t*                              player_m{nullptr};

    std::vector<std::unique_ptr<dynamic_light_t>> dynamic_lights_m;

    vertex_object_handle_t wireframe_cube_m;

    std::map<object_type_t, pickup_descriptor_t>         pickup_descriptors_m;
    std::map<weapon_type_t, weapon_descriptor_t>         weapon_descriptors_m;
    std::map<projectile_type_t, projectile_descriptor_t> projectile_descriptors_m;
    std::map<particle_type_t, particle_descriptor_t>     particle_descriptors_m;
    std::map<enemy_type_t, enemy_descriptor_t>           enemy_descriptors_m;
};

} // namespace mau
