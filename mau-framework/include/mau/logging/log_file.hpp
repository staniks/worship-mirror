#pragma once

#include "mau/base/log.hpp"

#include <fstream>

namespace mau {

// Perform logging into file.
class log_file_t : public log_observer_t
{
public:
    log_file_t(log_t& log);

    void on_message(log_severity_t severity, std::string_view message) override;

private:
    std::fstream stream_m;
};

} // namespace mau
