#include "mau/io/image.hpp"

#include "mau/io/file_reader.hpp"

#include <fmt/format.h>

namespace mau {

static constexpr uint64_t image_bytes_per_pixel_k = 4;

image_t::image_t(file_handle_t file)
{
    file_reader_t reader(file);
    header_t      header;

    reader.read(&header);

    if (header.magic != image_magic_k)
        throw exception_t{ fmt::format("invalid image magic: {}", file->name()) };

    size_m.x = header.width;
    size_m.y = header.height;

    // Will throw std::bad_alloc if unreasonable.
    rgba_m.resize(header.width * header.height * image_bytes_per_pixel_k);

    reader.read(rgba_m.data(), rgba_m.size());
}
image_handle_t image_t::create(file_handle_t file)
{
    return std::make_shared<image_t>(file);
}
image_handle_t image_t::create_resource(engine_context_t& engine, file_handle_t file)
{
    return create(file);
}
glm::ivec2 image_t::size() const
{
    return size_m;
}
const std::vector<byte_t>& image_t::rgba() const
{
    return rgba_m;
}
} // namespace mau
