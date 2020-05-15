#include "mau/io/archive.hpp"

#include "mau/math/checksum.hpp"

#include <fmt/format.h>

namespace mau {

archive_t::archive_t(std::string_view path) : stream_m(std::string(path), std::ios_base::in | std::ios_base::binary)
{
    if(!stream_m)
    {
        throw exception_t{fmt::format("unable to open archive: {}", path)};
    }

    if(!stream_m.read(reinterpret_cast<char*>(&header_m), sizeof(header_m)))
        throw exception_t{fmt::format("unable to read archive magic: {}", path)};

    if(header_m.magic != archive_magic_k)
        throw exception_t{fmt::format("invalid archive magic: {}", path)};

    std::string_view archive_corrupted_error_k{"archive is corrupted: {}"};

    for(uint64_t i = 0; i < header_m.file_count; ++i)
    {
        uint16_t filename_length{};
        if(!stream_m.read(reinterpret_cast<char*>(&filename_length), sizeof(filename_length)))
            throw exception_t{fmt::format(archive_corrupted_error_k, path)};

        std::string filename;
        filename.resize(filename_length);
        if(!stream_m.read(reinterpret_cast<char*>(filename.data()), filename.size()))
            throw exception_t{fmt::format(archive_corrupted_error_k, path)};

        file_entry_t raw_file_entry{};
        if(!stream_m.read(reinterpret_cast<char*>(&raw_file_entry), sizeof(raw_file_entry)))
            throw exception_t{fmt::format(archive_corrupted_error_k, path)};

        entries_m.emplace(filename, file_entry_t{raw_file_entry});
    }

    data_offset_m = stream_m.tellg();
}
file_handle_t archive_t::load_file(std::string_view path)
{
    auto it = entries_m.find(std::string(path));
    if(it == entries_m.end())
        throw exception_t{fmt::format("asset not found: {}", path)};

    auto& entry = it->second;

    stream_m.seekg(entry.data_offset + data_offset_m, std::ios::beg);

    file_handle_t file = std::make_shared<file_t>(path, new byte_t[entry.data_size], entry.data_size);
    if(!stream_m.read(reinterpret_cast<char*>(file->data()), file->size()))
    {
        throw exception_t{fmt::format("error while reading asset: {}", path)};
    }

    uint32_t checksum = adler32(file->data(), file->size());
    if(checksum != entry.data_checksum)
        throw exception_t{fmt::format("bad checksum, file seems corrupted: {}", path)};

    return file;
}

const archive_t::file_entry_map_t& archive_t::entry_map() const
{
    return entries_m;
}

} // namespace mau
