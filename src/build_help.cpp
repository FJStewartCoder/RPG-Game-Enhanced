#include "build_help.hpp"

extern "C" {
    #include "log/log.h"
}

// checks if a lua state has a build function
bool has_func(sol::environment &env, std::string func_name) {
    log_trace("Called function \"%s( env, %s )\"",
        __FUNCTION__,
        func_name.c_str()
    );

    sol::optional<sol::function> func = env[func_name];
    
    if ( !func ) { return false; }
    return true;
}