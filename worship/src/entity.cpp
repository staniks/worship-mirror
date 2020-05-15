#include "worship/gameplay/entity.hpp"

#include "worship/gameplay/world/world.hpp"

#include <mau/math/algorithms.hpp>

namespace mau {

entity_t::entity_t(world_t& world, glm::vec3 position) : world_m(world)
{
    set_position(position);
}
entity_t::~entity_t()
{
    world_m.spatial_entity_remove(this);
}
void entity_t::move(glm::vec3 amount, float delta_time)
{
    static constexpr int32_t relevant_sample_size_k = 2;

    glm::ivec3 rounded_size = {bounding_box_m.x + relevant_sample_size_k,
                               bounding_box_m.y + relevant_sample_size_k,
                               bounding_box_m.z + relevant_sample_size_k};

    bool wall_collision{false};

    // TODO: This is terribly inefficient.
    for(auto i = 0; i < 3; ++i)
    {
        glm::vec3 pos = position();
        pos[i] += amount[i] * delta_time;
        set_position(pos);

        glm::ivec3 rounded_position = position();
        bool       collision{false};

        tile_info_t* current_tile = world_m.tile(glm::vec2(rounded_position.x, rounded_position.z));
        if(current_tile)
        {
            for(entity_t* entity: current_tile->entities)
            {
                if(entity == this)
                    continue;
                if(entity->destroyed())
                    continue;

                if(math::aabb_intersect(position(), bounding_box_m, entity->position(), entity->bounding_box_m))
                {
                    if((entity->collision_layer_m & collision_mask_m) != 0)
                    {
                        on_collision(*entity);

                        if(entity->blocking())
                        {
                            collision = true;
                        }
                    }

                    if((collision_layer_m & entity->collision_mask_m) != 0)
                        entity->on_collision(*this);

                    if(collision)
                        goto resolve_collision;
                }
            }
        }

        // We're on a 2D plane, so restrict vertical movement.
        if(position().y < bounding_box_m.y / 2 || position().y > 1.0f - bounding_box_m.y / 2)
        {
            collision      = true;
            wall_collision = true;
            goto resolve_collision;
        }

        for(int32_t z = rounded_position.z - rounded_size.z; z <= rounded_position.z + rounded_size.z; ++z)
        {
            for(int32_t x = rounded_position.x - rounded_size.x; x <= rounded_position.x + rounded_size.x; ++x)
            {
                const glm::ivec3 current_tile_position{x, 0, z};
                tile_info_t*     current_tile = world_m.tile(glm::vec2(current_tile_position.x, current_tile_position.z));

                // Tiles beyond the map are treated as walls for simplicity.
                const tile_type_t current_tile_type = current_tile ? current_tile->type : tile_type_t::wall_k;

                if(current_tile_type == tile_type_t::wall_k)
                {
                    // Position actually marks the center of the bounding box, so we need to translate.
                    static constexpr glm::vec3 tile_offset{0.5f, 0.5f, 0.5f};
                    static constexpr glm::vec3 tile_size{1, 1, 1};

                    if(math::aabb_intersect(
                           position(), bounding_box_m, glm::vec3(current_tile_position) + tile_offset, tile_size))
                    {
                        collision      = true;
                        wall_collision = true;
                        goto resolve_collision;
                    }
                }
            }
        }
    resolve_collision: // hUrR dUrR, gOtO bAd
        if(collision)
        {
            glm::vec3 pos = position();
            pos[i] -= amount[i] * delta_time;
            set_position(pos);

            velocity_m[i] *= -bounciness_m;

            if(wall_collision)
                on_wall_collision();
        }
    }
}
void entity_t::fixed_update(float delta_time)
{
    if(gravity_m)
        apply_force(glm::vec3{0, -gravity_constant_k, 0}, delta_time);

    move(velocity_m, delta_time);

    velocity_m -= velocity_m * friction_m;
}
void entity_t::variable_update(float delta_time)
{
}
void entity_t::render(renderer_t& renderer, shader_handle_t ubershader, float delta_time)
{
}
void entity_t::on_collision(entity_t& entity)
{
}
void entity_t::on_wall_collision()
{
}
void entity_t::destroy()
{
    destroyed_m = true;
}
void entity_t::apply_force(glm::vec3 force, float delta_time)
{
    velocity_m += force * delta_time / mass_m;
}
void entity_t::rotate(float degrees, glm::vec3 axis)
{
    glm::quat temp = glm::angleAxis(glm::degrees(degrees), axis);
    orientation_m  = glm::normalize(temp) * orientation_m;
}
glm::vec3 entity_t::position() const
{
    return position_m;
}
glm::quat entity_t::orientation() const
{
    return orientation_m;
}
glm::vec3 entity_t::bounding_box() const
{
    return bounding_box_m;
}
bool entity_t::blocking() const
{
    return blocking_m;
}
collision_mask_t entity_t::collision_layer() const
{
    return collision_layer_m;
}
collision_mask_t entity_t::collision_mask() const
{
    return collision_mask_m;
}
void entity_t::set_position(glm::vec3 position)
{
    world_m.spatial_entity_remove(this);
    position_m = position;
    world_m.spatial_entity_insert(this);
}
void entity_t::set_orientation(glm::quat orientation)
{
    orientation_m = orientation;
}
void entity_t::set_friction(float friction)
{
    friction_m = friction;
}
void entity_t::set_blocking(bool blocking)
{
    blocking_m = blocking;
}
void entity_t::set_collision_layer(collision_mask_t collision_layer)
{
    collision_layer_m = collision_layer;
}
void entity_t::set_collision_mask(collision_mask_t collision_mask)
{
    collision_mask_m = collision_mask;
}
glm::vec3 entity_t::direction() const
{
    return orientation_m * glm::vec4{0.0f, 0.0f, -1.0f, 0.0f};
}
glm::vec3 entity_t::up() const
{
    return orientation_m * glm::vec4{0.0f, 1.0f, 0.0f, 0.0f};
}
glm::vec3 entity_t::right() const
{
    return orientation_m * glm::vec4{1.0f, 0.0f, 0.0f, 0.0f};
}

bool entity_t::destroyed() const
{
    return destroyed_m;
}

} // namespace mau
