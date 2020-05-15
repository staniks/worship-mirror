#pragma once

#include "mau/base/types.hpp"

namespace mau {

// RAII container for value with cleanup function.
template<typename T, typename Cleaner>
class value_container_t : non_copyable_t, non_movable_t
{
public:
    explicit value_container_t(T value, Cleaner cleaner);
    virtual ~value_container_t();

    void reset(T value);

    T& get();

protected:
    T       value_m;
    Cleaner cleaner_m;
};

template<typename T, typename Cleaner>
inline value_container_t<T, Cleaner>::value_container_t(T value, Cleaner cleaner) : value_m(value), cleaner_m(cleaner)
{
}

template<typename T, typename Cleaner>
inline value_container_t<T, Cleaner>::~value_container_t()
{
    cleaner_m(value_m);
}

template<typename T, typename Cleaner>
inline void value_container_t<T, Cleaner>::reset(T value)
{
    cleaner_m(value_m);
    value_m = value;
}

template<typename T, typename Cleaner>
inline T& value_container_t<T, Cleaner>::get()
{
    return value_m;
}

} // namespace mau
