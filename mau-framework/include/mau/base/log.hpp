#pragma once

#include "mau/base/timer.hpp"
#include "mau/base/types.hpp"

#include <set>
#include <string_view>

namespace mau {

enum class log_severity_t
{
    normal_k,
    warning_k,
    error_k
};

// Forward declarations.
class log_t;

class log_observer_t
{
public:
    log_observer_t(log_t& log);
    virtual ~log_observer_t();

    virtual void on_message(log_severity_t severity, std::string_view message) = 0;

private:
    log_t& log_m;
};

class log_t : non_copyable_t, non_movable_t
{
public:
    void log(std::string_view message);
    void log(log_severity_t severity, std::string_view message);

    void add_observer(log_observer_t* observer);
    void remove_observer(log_observer_t* observer);

private:
    timer_t                  timer_m;
    std::set<log_observer_t*> observers_m;
};

} // namespace mau
