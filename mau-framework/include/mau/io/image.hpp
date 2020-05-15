#pragma once

#include "mau/io/file.hpp"
#include "mau/io/resource.hpp"
#include "mau/math/vector.hpp"

#include <vector>

namespace mau {

inline static constexpr uint64_t image_magic_k = 0x786574020455414D;

// Forward declarations.
class image_t;
using image_handle_t = std::shared_ptr<image_t>;

// A RGBA image. Consists of raw RGBA byte array.
class image_t : public resource_t
{
public:
#pragma pack(push, 1)
    struct header_t
    {
        uint64_t magic;
        uint32_t width;
        uint32_t height;
    };
#pragma pack(pop)
    explicit image_t(file_handle_t file);

    static image_handle_t create(file_handle_t file);
    static image_handle_t create_resource(engine_context_t& engine, file_handle_t file);

    glm::ivec2                 size() const;
    const std::vector<byte_t>& rgba() const;

private:
    glm::ivec2          size_m;
    std::vector<byte_t> rgba_m;
};

} // namespace mau
