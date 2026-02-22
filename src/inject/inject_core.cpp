#include "inject_core.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"


int inject_core_player_data(sol::environment &env) {
    log_trace("Called function \"%s( env )\"", __FUNCTION__);

    env[engine::player::DATA] = env.create_with(
        engine::player::NAME, "Player Name",
        engine::player::POSITION, env.create_with(
            "x", 0,
            "y", 0,
            "z", 0
        )
    );

    return 0;
}

int inject_core(sol::environment &env) {
    log_trace("Called function \"%s( env )\"", __FUNCTION__);

    inject_core_player_data(env);

    return 0;
}