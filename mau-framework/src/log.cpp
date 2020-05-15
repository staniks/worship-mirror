#include "mau/base/log.hpp"

#include <fmt/format.h>

namespace mau {

void log_t::log(std::string_view message)
{
    log(log_severity_t::normal_k, message);
}

void log_t::log(log_severity_t severity, std::string_view message)
{
    uint64_t hours        = timer_m.hours() % 99;
    uint64_t minutes      = timer_m.minutes() % 60;
    uint64_t seconds      = timer_m.seconds() % 60;
    uint64_t milliseconds = timer_m.milliseconds() % 1000;

    std::string formatted_message =
        fmt::format("[{:02d}:{:02d}:{:02d}:{:03d}] {:s}\n", hours, minutes, seconds, milliseconds, message);

    for(log_observer_t* observer: observers_m)
        observer->on_message(severity, formatted_message);
}

void log_t::add_observer(log_observer_t* observer)
{
    observers_m.insert(observer);
}

void log_t::remove_observer(log_observer_t* observer)
{
    observers_m.erase(observer);
}

log_observer_t::log_observer_t(log_t& log) : log_m(log)
{
    log_m.add_observer(this);
}

log_observer_t::~log_observer_t()
{
    log_m.remove_observer(this);
}

} // namespace mau
