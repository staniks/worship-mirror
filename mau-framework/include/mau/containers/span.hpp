#pragma once

#include "mau/base/types.hpp"

namespace mau {

// Simple span implementation. At the time of writing this, I had no c++20 compiler available, so I hacked this. Good enough
// for my use-case, but hardly a replacement for std::span.
template<typename T>
class span_t
{
public:
    span_t();
    span_t(const T* data, size_t size);

    template<typename Container>
    span_t(Container& container);

    const T* data() const;
    size_t   size() const;

private:
    const T*     data_m;
    const size_t size_m;
};

template<typename T>
inline span_t<T>::span_t() : data_m(nullptr), size_m(0)
{
}

template<typename T>
inline span_t<T>::span_t(const T* data, size_t size) : data_m(data), size_m(size)
{
}

template<typename T>
inline const T* span_t<T>::data() const
{
    return data_m;
}

template<typename T>
inline size_t span_t<T>::size() const
{
    return size_m;
}

template<typename T>
template<typename Container>
inline span_t<T>::span_t(Container& container) : data_m(container.data()), size_m(container.size())
{
}

} // namespace mau
