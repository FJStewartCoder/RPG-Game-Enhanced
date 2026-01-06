#include "build_help.hpp"

// checks if a lua state has a build function
bool has_func(sol::state &lua, std::string func_name) {
    sol::optional<sol::function> func = lua[func_name];
    
    if ( !func ) {
        return false;
    }

    return true;
}