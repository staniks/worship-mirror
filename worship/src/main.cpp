#include "worship/states/state_gameplay.hpp"
#include "worship/states/state_main_menu.hpp"
#include "worship/states/state_title_screen.hpp"
#include "worship/states/state_preloading.hpp"

#include <mau/logging/log_file.hpp>
#include <mau/logging/log_stdout.hpp>

int main(int argc, char** argv)
{
    mau::log_t        log;
    mau::log_stdout_t log_stdout{log};
    mau::log_file_t   log_file{log};

    for(;;)
    {
        try
        {
            mau::engine_context_t engine{"Worship", log};
            engine.run<mau::state_preloading_t>();
            if(!engine.restart_requested())
                break;
        }
        catch(mau::exception_t& e)
        {
            log.log(mau::log_severity_t::error_k, e.what());
            break;
        }
    }

    return 0;
}
