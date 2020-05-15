#include "mau/logging/log_file.hpp"

namespace mau {

log_file_t::log_file_t(log_t& log) : log_observer_t(log), stream_m("mau-log.txt", std::ios::out)
{
}
void log_file_t::on_message(log_severity_t severity, std::string_view message)
{
    stream_m << message;
}

} // namespace mau
