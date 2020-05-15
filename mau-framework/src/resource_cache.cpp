#include "mau/io/resource_cache.hpp"

#include <fmt/format.h>

#include "mau/rendering/shader.hpp"
#include "mau/rendering/font.hpp"
#include "mau/rendering/texture.hpp"
#include "mau/audio/audio_clip.hpp"

namespace mau {

//======================================
// Resource collection specializations.
// =====================================
template<>
resource_collection_t& resource_cache_t::resource_collection<shader_t>()
{
    return shader_collection_m;
}

template<>
resource_collection_t& resource_cache_t::resource_collection<font_t>()
{
    return font_collection_m;
}

template<>
resource_collection_t& resource_cache_t::resource_collection<texture_t>()
{
    return texture_collection_m;
}

template<>
resource_collection_t& resource_cache_t::resource_collection<audio_clip_t>()
{
    return audio_clip_collection_m;
}

//======================================
// Preloading context.
// =====================================
void resource_preloading_context_t::add(resource_preloading_context_t::resource_descriptor_t descriptor)
{
    descriptors_m.push_back(descriptor);
    ++initial_size_m;
}

std::optional<resource_preloading_context_t::resource_descriptor_t> resource_preloading_context_t::consume()
{
    if(descriptors_m.empty())
        return {};

    auto descriptor = descriptors_m.back();
    descriptors_m.pop_back();
    return descriptor;
}

std::optional<resource_preloading_context_t::resource_descriptor_t> resource_preloading_context_t::current() const
{
    if(descriptors_m.empty())
        return {};

    return descriptors_m.back();
}

float resource_preloading_context_t::progress() const
{
    return 1.0f - (float)descriptors_m.size() / initial_size_m;
}

//======================================
// Resource cache.
// =====================================
resource_cache_t::resource_cache_t(engine_context_t& engine) : engine_m(engine), archive_m("data.mau")
{
}

resource_preloading_context_t resource_cache_t::create_preloading_context()
{
    resource_preloading_context_t context;

    for(const auto& [path, entry] : archive_m.entry_map())
    {
        if(entry.file_type >= static_cast<uint8_t>(resource_type_t::_count_k))
            throw exception_t{fmt::format("preloading failed, unknown resource type: {}", entry.file_type)};

        resource_type_t type = static_cast<resource_type_t>(entry.file_type);
        if(type == resource_type_t::indirect_k)
            continue;

        context.add({path, type});
    }

    return context;
}

// Preload next resource in the context.
resource_preloading_result_t resource_cache_t::preload_next(resource_preloading_context_t& context)
{
    resource_preloading_result_t result{};

    std::optional<resource_preloading_context_t::resource_descriptor_t> descriptor = context.consume();
    if(!descriptor)
    {
        result.finished = true;
        result.progress = 1.0f;
        return result;
    }

    auto path = descriptor->path;
    auto type = descriptor->type;

    switch(type)
    {
        case resource_type_t::texture_k: (void)resource<texture_t>(path);    break;
        case resource_type_t::shader_k:  (void)resource<shader_t>(path);     break;
        case resource_type_t::font_k:    (void)resource<font_t>(path);       break;
        case resource_type_t::sound_k:   (void)resource<audio_clip_t>(path); break;
        default:
            throw exception_t{fmt::format("missing resource loader mapping for resource type: {}", type)};
    }

    result.progress = context.progress();
    return result;
}

file_handle_t resource_cache_t::load_file(std::string_view path)
{
    return archive_m.load_file(path);
}

} // namespace mau
