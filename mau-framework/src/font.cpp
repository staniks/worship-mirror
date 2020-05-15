#include "mau/rendering/font.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/io/file_reader.hpp"
#include "mau/io/resource_cache.hpp"

#include <fmt/format.h>

#include <string>

namespace mau {

inline static constexpr uint64_t font_magic_k                  = 8027223241264546125ULL;
inline static constexpr uint64_t font_descriptor_path_length_k = 256;

#pragma pack(push, 1)
struct font_descriptor_t
{
    uint64_t magic;
    uint8_t  character_count_x;
    uint8_t  character_count_y;
    uint32_t character_size_x;
    uint32_t character_size_y;
    bool     shadow;
    bool     gradient;
};
#pragma pack(pop)

font_t::font_t(texture_handle_t texture, glm::ivec2 character_count, glm::vec2 character_size, bool shadow, bool gradient) :
    character_count(character_count), character_size(character_size), shadow(shadow), gradient(gradient), texture_m(texture)
{
}
font_handle_t font_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    file_reader_t reader{file};

    font_descriptor_t descriptor;
    reader.read(&descriptor);

    if(descriptor.magic != font_magic_k)
        throw exception_t(fmt::format("invalid font magic: {}", file->name()));

    std::string texture_path;
    reader.read_string(texture_path);

    texture_handle_t texture = engine.resource_cache().resource<texture_t>(texture_path);

    return std::make_shared<font_t>(texture,
                                    glm::ivec2{descriptor.character_count_x, descriptor.character_count_y},
                                    glm::vec2{descriptor.character_size_x, descriptor.character_size_y},
                                    descriptor.shadow,
                                    descriptor.gradient);
}
texture_handle_t font_t::texture()
{
    return texture_m;
}

} // namespace mau
