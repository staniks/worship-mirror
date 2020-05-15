#pragma once

#include "mau/base/types.hpp"

namespace mau
{
    // Returns adler32 checksum of given data.
    // TODO: refactor to use mau::span
    uint32_t adler32(byte_t* data, size_t size);
}