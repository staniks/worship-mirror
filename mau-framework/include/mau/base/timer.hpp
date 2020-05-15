#pragma once

#include "mau/base/types.hpp"

#include <chrono>
#include <cstdint>

namespace mau {

class timer_t
{
public:
    explicit timer_t();

    void reset();

    inline uint64_t hours() const;
    inline uint64_t minutes() const;
    inline uint64_t seconds() const;
    inline uint64_t milliseconds() const;
    inline uint64_t microseconds() const;

    static constexpr double microseconds_to_seconds_k = 1.0 / 1000000.0;

private:
    template<typename T>
    uint64_t time() const;

    std::chrono::high_resolution_clock                          clock_m;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_m;
};

template<typename T>
inline uint64_t timer_t::time() const
{
    const auto delta = clock_m.now() - start_time_m;
    return std::chrono::duration_cast<T>(delta).count();
}

uint64_t timer_t::hours() const
{
    return time<std::chrono::hours>();
}

uint64_t timer_t::minutes() const
{
    return time<std::chrono::minutes>();
}

uint64_t timer_t::seconds() const
{
    return time<std::chrono::seconds>();
}

uint64_t timer_t::milliseconds() const
{
    return time<std::chrono::milliseconds>();
}

uint64_t timer_t::microseconds() const
{
    return time<std::chrono::microseconds>();
}

} // namespace mau
