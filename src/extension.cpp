#include "extension.hpp"
#include "build_help.hpp"

// for "new_node_type" injection
#include "build.hpp"

#include "nodes.hpp"
#include "player.hpp"

int build_player_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {
        core[LUA_NODE_TEMPLATE][item.first] = item.second;
    }

    return 0;
}

int build_node_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {
        core[LUA_CORE_PLAYER_DATA][item.first] = item.second;
    }

    return 0;
}

int inject_build_tools(sol::state &core_state, sol::state &extension_state) {
    // add the extend player function
    extension_state.set_function("extend_player", [&core_state](sol::table extension) {
        build_player_extension(core_state, extension);
    });

    // add the extend node function
    extension_state.set_function("extend_node", [&core_state](sol::table extension) {
        build_node_extension(core_state, extension);
    });

    // add the add node function
    extension_state.set_function("add_node", [&core_state](sol::table table) {
        new_node_type(core_state, table);
    });

    return 0;
}

// function to build the extensions
int build_file(sol::state &core_state, std::string file_name) {
    sol::state extension_state;

    extension_state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table);

    try {
        extension_state.safe_script_file(file_name);
        std::cout << file_name << " has been opened" << std::endl;
    }
    catch ( const sol::error &e ) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    // inject the relevant functions
    inject_build_tools(core_state, extension_state);

    // if a build func exists try to run it
    if ( has_func(extension_state, LUA_BUILD_FUNC) ) {
        std::cout << "Build function found" << std::endl;

        sol::protected_function build_func = extension_state[LUA_BUILD_FUNC];

        auto res = build_func();

        if ( !res.valid() ) {
            std::cout << "Build function in file " << file_name << " has run with errors." << std::endl;

            sol::error error = res;
            std::cout << error.what() << std::endl;
        }
    }

    // if a extend func exists try to run it
    if ( has_func(extension_state, LUA_EXTEND_FUNC) ) {
        std::cout << "Extend function found" << std::endl;

        sol::protected_function extend_func = extension_state[LUA_EXTEND_FUNC];

        auto res = extend_func();

        if ( !res.valid() ) {
            std::cout << "Extend function in file " << file_name << " has run with errors." << std::endl;

            sol::error error = res;
            std::cout << error.what() << std::endl;
        }
    }

    return 0;
}