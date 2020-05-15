#pragma once

#include "mau/base/log.hpp"

namespace mau {

// Perform logging to standard output.
class log_stdout_t : public log_observer_t
{
public:
    log_stdout_t(log_t& log);

    void on_message(log_severity_t severity, std::string_view message) override;
};

} // namespace mau
