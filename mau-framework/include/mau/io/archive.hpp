#pragma once

#include "mau/io/file.hpp"

#include <fstream>
#include <string>
#include <unordered_map>

namespace mau {

inline static constexpr uint64_t archive_magic_k = 0x726100020455414D;

// An archive contains assets and provides simple interface for loading.
class archive_t : non_movable_t, non_copyable_t
{
public:
#pragma pack(push, 1)
    struct header_t
    {
        uint64_t magic;
        uint64_t file_count;
    };
    struct file_entry_t
    {
        uint8_t  file_type;
        uint64_t data_offset;
        uint64_t data_size;
        uint32_t data_checksum;
    };
#pragma pack(pop)

    // Open an archive at specified path for reading.
    explicit archive_t(std::string_view path);

    // Load file from archive into memory.
    file_handle_t load_file(std::string_view path);

    using file_entry_map_t = std::unordered_map<std::string, file_entry_t>;
    const file_entry_map_t& entry_map() const;

private:
    std::fstream                                  stream_m;
    header_t                                      header_m;
    file_entry_map_t                              entries_m;
    uint64_t                                      data_offset_m;
};

} // namespace mau
