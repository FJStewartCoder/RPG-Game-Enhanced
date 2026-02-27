#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}

// gets the version as a string
const std::string engine::version_string() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    std::string res = "";
    std::string ver_str = std::to_string(engine::VERSION);

    const auto str_len = ver_str.length();

    for ( int i = 0; i < str_len - 1; i++ ) {
        const char c = ver_str[i];
        res += c;
        res += '.';
    }

    res += ver_str[str_len - 1];

    return res;
}

// check is ver is compatible with the current version
const bool engine::is_version_compatible(int ver) {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    if ( ver > engine::VERSION ) { return false; }

    return true;
}