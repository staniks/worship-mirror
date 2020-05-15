#pragma once

#include "mau/base/engine_context.hpp"
#include "mau/base/types.hpp"
#include "mau/io/archive.hpp"
#include "mau/io/file.hpp"

#include <frozen/map.h>
#include <fmt/format.h>

#include <string>
#include <vector>
#include <optional>

#ifdef MOCK_LOADING_TIMES
#include <chrono>
#include <thread>
#endif

namespace mau {

using resource_handle_t = std::shared_ptr<resource_t>;
using resource_collection_t = std::map<std::string, resource_handle_t, std::less<>>;

enum class resource_type_t
{
    indirect_k,
    texture_k,
    shader_k,
    font_k,
    sound_k,
    _count_k
};

class resource_preloading_context_t
{
public:
    struct resource_descriptor_t
    {
        std::string_view path;
        resource_type_t  type;
    };

    void add(resource_descriptor_t);
    std::optional<resource_descriptor_t> consume();
    std::optional<resource_descriptor_t> current() const;

    float progress() const;

private:
    std::vector<resource_descriptor_t> descriptors_m;
    size_t                             initial_size_m{0};
};

struct resource_preloading_result_t
{
    // True if no more resources follow.
    bool finished {false};

    // Preloading progress, [0.0f, 1.0f].
    float progress {0.0f};
};

// Resource cache abstract away some of the complexity of handling resources, performs resource caching and retrieval.
class resource_cache_t : non_copyable_t, non_movable_t
{
public:
    resource_cache_t(engine_context_t& engine);

    // Create a list of assets to be preloaded.
    resource_preloading_context_t create_preloading_context();

    // Preload next resource in the context.
    resource_preloading_result_t preload_next(resource_preloading_context_t& context);

    template<class T>
    auto resource(std::string_view path)
    {
        auto& collection = resource_collection<T>();
        auto it = collection.find(path);
        if(it == collection.end())
        {
            engine_m.log().log(fmt::format("loading resource: {}...", path));
            auto handle = T::create_resource(engine_m, archive_m.load_file(path));
            collection[std::string{path}] = handle;

#ifdef MOCK_LOADING_TIMES
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif

            return handle;
        }

        return std::static_pointer_cast<T>(it->second);
    }

    file_handle_t load_file(std::string_view path);

private:
    template<class T>
    resource_collection_t& resource_collection();

    engine_context_t& engine_m;
    archive_t         archive_m;

    resource_collection_t shader_collection_m;
    resource_collection_t font_collection_m;
    resource_collection_t texture_collection_m;
    resource_collection_t audio_clip_collection_m;
};

} // namespace mau
