#pragma once

#include "worship/gameplay/collision_layers.hpp"
#include "worship/gameplay/event_callback.hpp"

#include "mau/rendering/renderer.hpp"

#include <mau/math/quaternion.hpp>
#include <mau/math/vector.hpp>
#include <mau/rendering/texture.hpp>

namespace mau {

class world_t;

// TODO: This is just arbitrary, make it something standard?
inline static constexpr float gravity_constant_k = 32.0f;

class entity_t : non_copyable_t, non_movable_t
{
public:
    entity_t(world_t& world, glm::vec3 position);
    virtual ~entity_t();

    virtual void fixed_update(float delta_time);
    virtual void variable_update(float delta_time);
    virtual void render(renderer_t& renderer, shader_handle_t ubershader, float delta_time);

    virtual void on_collision(entity_t& entity);
    virtual void on_wall_collision();

    virtual void destroy();

    // Applies acceleration based on mass. Use only from within fixed update!
    void apply_force(glm::vec3 force, float delta_time);

    // Change orientation around axis.
    void rotate(float degrees, glm::vec3 axis);

    glm::vec3        position() const;
    glm::quat        orientation() const;
    glm::vec3        bounding_box() const;
    bool             blocking() const;
    collision_mask_t collision_layer() const;
    collision_mask_t collision_mask() const;

    glm::vec3 direction() const;
    glm::vec3 up() const;
    glm::vec3 right() const;

    bool destroyed() const;

    void set_position(glm::vec3 position);
    void set_orientation(glm::quat orientation);
    void set_friction(float friction);
    void set_blocking(bool blocking);
    void set_collision_layer(collision_mask_t collision_layer);
    void set_collision_mask(collision_mask_t collision_mask);

private:
    // This should never be modified without accoringly taking care of spatial structures.
    glm::vec3 position_m{0, 0, 0};

protected:
    void move(glm::vec3 amount, float delta_time);

    world_t& world_m;

    glm::quat orientation_m{1, 0, 0, 0};
    glm::vec3 velocity_m{0, 0, 0};
    glm::vec3 bounding_box_m{1, 1, 1}; // Bounding box size used for collision detection.

    float mass_m{1.0f};
    float friction_m{0.0f};
    float bounciness_m{0.0f};
    bool  gravity_m{false};

    bool destroyed_m{false};

    // Whether or not prevent movement when collided with.
    bool blocking_m{false};

    // Mask which indicates which layer the object is on.
    collision_mask_t collision_layer_m{0};

    // Mask which indicates which objects will block movement for this object, or for which on_collision() will be called. In
    // short, this mask is a list of layer we want to check collision against.
    collision_mask_t collision_mask_m{0};
};

} // namespace mau
