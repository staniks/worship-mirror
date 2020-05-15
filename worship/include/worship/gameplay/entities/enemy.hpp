#pragma once

#include "worship/gameplay/sprite_entity.hpp"

#include "worship/gameplay/entity_descriptor.hpp"

namespace mau {

class enemy_t : public sprite_entity_t
{
    enum class enemy_state_t
    {
        unaware_k,
        acquiring_player_k,
        moving_k,
        shooting_k,
        dying_k
    };

public:
    enemy_t(world_t& world, glm::vec3 position, const enemy_descriptor_t& descriptor);

    void fixed_update(float delta_time) override;
    void variable_update(float delta_time) override;

    texture_handle_t sprite_texture_diffuse() override;
    texture_handle_t sprite_texture_emission() override;

    void on_collision(entity_t& entity) override;

    float health() const;

    void take_damage(float damage);

private:
    void change_animation(enemy_descriptor_t::animation_t animation);
    void die();

    const enemy_descriptor_t& descriptor_m;

    float health_m;
    bool  dead_m{false};

    uint32_t current_direction_index_m{0};
    float    current_frame_m{0.0f};

    enemy_descriptor_t::animation_t animation_m{enemy_descriptor_t::animation_t::walking_k};

    enemy_state_t state_m{enemy_state_t::unaware_k};
    float         state_timer_m{0.0f};

    static constexpr float unaware_interval_k          = 0.25f;
    static constexpr float acquiring_player_interval_k = 0.25f;
    static constexpr float move_duration_k             = 1.0f;

    float fire_duration_m;
    float fire_range_m;

    glm::vec3 desired_direction_m{};
};

} // namespace mau
