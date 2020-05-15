#include "mau/math/checksum.hpp"

namespace mau {

uint32_t adler32(byte_t* data, size_t size)
{
    static constexpr uint32_t mod_adler_k = 65521U;

    uint32_t a = 1, b = 0;

    for(uint64_t i = 0; i < size; ++i)
    {
        a = (a + data[i]) % mod_adler_k;
        b = (b + a) % mod_adler_k;
    }

    return (b << 16U) | a;
}

} // namespace mau
