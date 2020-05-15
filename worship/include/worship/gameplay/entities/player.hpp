#pragma once

#include "worship/gameplay/entity.hpp"

#include "worship/gameplay/weapon.hpp"

namespace mau {

enum class difficulty_t
{
    easy_k, normal_k, hard_k
};

class player_t : public entity_t
{
public:
    player_t(world_t& world, glm::vec3 position, difficulty_t difficulty);

    void fixed_update(float delta_time) override;
    void variable_update(float delta_time) override;

    void fire();
    void walk(glm::vec3 direction, float delta_time);

    glm::vec3 camera_position() const;
    float     move_acceleration() const;
    float     head_bob() const;
    glm::vec2 weapon_bob() const;

    float fov() const;

    float    health() const;
    float    armor() const;
    uint32_t ammo();
    bool     dead() const;

    bool add_weapon(weapon_type_t weapon_type);
    void select_weapon_slot(uint8_t slot);

    bool add_health(float amount);
    bool add_armor(float amount);
    bool add_ammo(ammo_type_t type, int64_t amount);

    int64_t ammo(ammo_type_t type);
    void    consume_ammo(ammo_type_t type);

    weapon_t* current_weapon();

    void take_damage(float damage);
    void die();

private:
    difficulty_t difficulty_m;

    float move_acceleration_m;

    float head_bob_m{0.0f};
    float weapon_bob_m{0.0f};

    float                          health_m;
    float                          armor_m;
    std::map<ammo_type_t, int64_t> ammo_m;

    enum class weapon_switch_t
    {
        none_k,
        lowering_k,
        raising_k
    };

    weapon_switch_t switching_weapons_m{weapon_switch_t::none_k};
    float           switching_weapons_progress_m{0.0f};
    uint8_t         selected_weapon_m{0};
    uint8_t         current_weapon_m{0};

    std::map<weapon_type_t, std::unique_ptr<weapon_t>> weapons_m;

    bool dead_m{false};

    // Sounds
    std::vector<audio_clip_handle_t> grunts_m;
    audio_clip_handle_t              death_sound_m;
};

} // namespace mau
