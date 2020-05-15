#include "mau/logging/log_stdout.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>

namespace mau {

log_stdout_t::log_stdout_t(log_t& log) : log_observer_t(log)
{
}
void log_stdout_t::on_message(log_severity_t severity, std::string_view message)
{
#ifdef _WIN32
    static HANDLE win32_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(severity == log_severity_t::normal_k)
        SetConsoleTextAttribute(win32_console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    else if(severity == log_severity_t::warning_k)
        SetConsoleTextAttribute(win32_console_handle, FOREGROUND_RED | FOREGROUND_GREEN);
    else if(severity == log_severity_t::error_k)
        SetConsoleTextAttribute(win32_console_handle, FOREGROUND_RED);
#endif

    // TODO: add ANSI color escape codes support to allow color on *nix terminals
    std::cout << message;

#ifdef _WIN32
    SetConsoleTextAttribute(win32_console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

} // namespace mau
