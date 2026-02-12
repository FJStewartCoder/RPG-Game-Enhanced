#include "extension.hpp"
#include "build_help.hpp"

// for "new_node_type" injection
#include "build.hpp"

#include "nodes.hpp"

#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}

// function to ensure that there are no matching globals
int test_new_state(sol::environment &load_env, sol::environment extension) {
    log_trace("Called function \"%s( env, table )\"", __FUNCTION__);

    // iterate each global in the extension state. If there is a matching one in base, we throw an error
    for ( const auto &item : extension.pairs() ) {
        const std::string key = item.first.as<std::string>();

        // DEBUG PRINT
        // std::cout << key << " " << (int)item.second.get_type() << std::endl;

        // for base to have key, it is not nil
        const bool base_has_key = load_env[key] != sol::nil;

        // DEBUG PRINT
        // std::cout << key << " " << base_has_key << std::endl;

        // error is base global does not have key (user created) and is in the base
        if ( base_has_key ) {
            log_error("Another file has key \"%s\"", item.first.as<std::string>().c_str());
            return 1;
        }
    }

    return 0;
}

// function to build the extensions
int load_file(sol::state &lua, std::string file_name) {
    log_trace("Called function \"%s( state, %s )\"",
        __FUNCTION__,
        file_name.c_str()
    );

    // create a new special state to verify the file before combining with the main program
    sol::environment test_env(lua, sol::create);

    try {
        lua.safe_script_file(file_name);
        log_info("%s has been opened", file_name.c_str());
    }
    catch ( const sol::error &e ) {
        log_error("File %s open with error\n%s", file_name.c_str(), e.what());
        return 1;
    }

    return 0;
}

// function to build the extensions
int load_file(sol::state &lua, sol::environment &load_env, std::string file_name) {
    log_trace("Called function \"%s( state, env, %s )\"",
        __FUNCTION__,
        file_name.c_str()
    );

    // create a new special state to verify the file before combining with the main program
    sol::environment test_env(lua, sol::create);

    try {
        lua.safe_script_file(file_name, test_env);
        log_info("%s has been opened", file_name.c_str());
    }
    catch ( const sol::error &e ) {
        log_error("File %s open with error\n%s", file_name.c_str(), e.what());
        return 1;
    }

    if ( test_new_state(load_env, test_env) != 0 ) {
        log_error("Invalid extension with file name: %s", file_name.c_str()); 
        return 1;
    }

    // if passes the first tests load into the real environment
    lua.safe_script_file(file_name, load_env);

    return 0;
}