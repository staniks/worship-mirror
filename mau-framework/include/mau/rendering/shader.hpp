#pragma once

#include "mau/containers/span.hpp"
#include "mau/io/resource.hpp"
#include "mau/math/matrix.hpp"

#include <map>
#include <string>

namespace mau {

enum class shader_uniform_t
{
    // Generic
    texture0_k,
    texture1_k,
    texture2_k,
    texture3_k,

    // Specific
    model_matrix_k,
    view_matrix_k,
    projection_matrix_k,

    enable_lighting_k,
    enable_emission_k,
    enable_fog_k,
    enable_sprite_billboarding_k,
    enable_particle_mode_k,

    fog_density_k,
    particle_size_k,
    pass_k,
    resolution_k,

    light_count_k,
    light_positions_k,
    light_colors_k,
    light_radii_k
};

inline static constexpr std::pair<shader_uniform_t, const char*> shader_uniform_mapping_k[] = {
    {shader_uniform_t::texture0_k, "uniformTexture0"},
    {shader_uniform_t::texture1_k, "uniformTexture1"},
    {shader_uniform_t::texture2_k, "uniformTexture2"},
    {shader_uniform_t::texture3_k, "uniformTexture3"},
    {shader_uniform_t::model_matrix_k, "uniformModel"},
    {shader_uniform_t::view_matrix_k, "uniformView"},
    {shader_uniform_t::projection_matrix_k, "uniformProjection"},
    {shader_uniform_t::enable_sprite_billboarding_k, "uniformEnableSpriteBillboarding"},
    {shader_uniform_t::pass_k, "uniformPass"},
    {shader_uniform_t::resolution_k, "uniformResolution"},
    {shader_uniform_t::enable_lighting_k, "uniformEnableLighting"},
    {shader_uniform_t::enable_emission_k, "uniformEnableEmission"},
    {shader_uniform_t::enable_fog_k, "uniformEnableFog"},
    {shader_uniform_t::fog_density_k, "uniformFogDensity"},
    {shader_uniform_t::enable_particle_mode_k, "uniformEnableParticleMode"},
    {shader_uniform_t::particle_size_k, "uniformParticleSize"},
    {shader_uniform_t::light_count_k, "uniformLightCount"},
    {shader_uniform_t::light_positions_k, "uniformLightPositions"},
    {shader_uniform_t::light_colors_k, "uniformLightColors"},
    {shader_uniform_t::light_radii_k, "uniformLightRadii"},
};

enum class texture_unit_t
{
    texture0_k = 0,
    texture1_k,
    texture2_k,
    texture3_k
};

// Forward declaration.
class shader_t;

using shader_handle_t     = std::shared_ptr<shader_t>;
using gl_uniform_handle_t = int32_t;

class shader_t : public resource_t
{
public:
    static shader_handle_t create(file_handle_t vertex_file, file_handle_t fragment_file);

    shader_t(file_handle_t vertex_file, file_handle_t fragment_file);
    ~shader_t();

    void set_uniform_mat4(shader_uniform_t uniform, const glm::mat4& value);
    void set_uniform_vec2(shader_uniform_t uniform, glm::vec2 value);
    void set_uniform_int(shader_uniform_t uniform, int32_t value);
    void set_uniform_bool(shader_uniform_t uniform, bool value);
    void set_uniform_float(shader_uniform_t uniform, float value);
    void set_uniform_vec3_array(shader_uniform_t uniform, span_t<glm::vec3> values);
    void set_uniform_float_array(shader_uniform_t uniform, span_t<float> values);

    void bind_texture_units();

    gl_handle_t gl_handle();

    static shader_handle_t create_resource(engine_context_t& engine, file_handle_t file);

private:
    gl_uniform_handle_t gl_uniform(shader_uniform_t uniform);

    gl_handle_t                                     gl_handle_m{0};
    std::map<shader_uniform_t, gl_uniform_handle_t> gl_uniform_mapping_m;
};

} // namespace mau
