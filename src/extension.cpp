#include "extension.hpp"
#include "build_help.hpp"

// for "new_node_type" injection
#include "build.hpp"

#include "nodes.hpp"

#include "lua_engine_constants.hpp"

int build_player_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {     
        std::cout << "Extend player called with extension " << item.first.as<std::string>() << std::endl; 
        core[engine::player::DATA][item.first] = item.second;
    }

    return 0;
}

int build_node_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {
        std::cout << "Extend node called with extension " << item.first.as<std::string>() << std::endl;
        core[engine::node::TEMPLATE][item.first] = item.second;
    }

    return 0;
}

int inject_build_tools(sol::state &core_state) {
    // add the extend player function
    core_state.set_function(engine::func::api::EXTEND_PLAYER, [&core_state](sol::table extension) {
        return build_player_extension(core_state, extension);
    });

    // add the extend node function
    core_state.set_function(engine::func::api::EXTEND_NODE, [&core_state](sol::table extension) {
        return build_node_extension(core_state, extension);
    });

    // add the add node function
    core_state.set_function(engine::func::api::ADD_NODE_TYPE, [&core_state](sol::table table) {
        return new_node_type(core_state, table);
    });

    return 0;
}

// function to ensure that there are no matching globals
int test_new_state(sol::state &base, sol::environment extension) {
    // iterate each global in the extension state. If there is a matching one in base, we throw an error
    for ( const auto &item : extension.pairs() ) {
        const std::string key = item.first.as<std::string>();

        // DEBUG PRINT
        // std::cout << key << " " << (int)item.second.get_type() << std::endl;

        // for base to have key, it is not nil
        const bool base_has_key = base[key] != sol::nil;

        // DEBUG PRINT
        // std::cout << key << " " << base_has_key << std::endl;

        // error is base global does not have key (user created) and is in the base
        if ( base_has_key ) {
            std::cout << "Error; some other file has key: " << item.first.as<std::string>() << std::endl;
            return 1;
        }
    }

    return 0;
}

// function to build the extensions
int load_file(sol::state &lua, std::string file_name) {
    // create a new special state to verify the file before combining with the main program
    sol::environment load_env(lua, sol::create);

    try {
        lua.safe_script_file(file_name, load_env);
        std::cout << file_name << " has been opened" << std::endl;
    }
    catch ( const sol::error &e ) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    if ( test_new_state(lua, load_env) != 0 ) {
        std::cout << "Invalid extension" << std::endl; 
        return 1;
    }

    // if passes the first tests load into the real environment
    lua.safe_script_file(file_name);

    return 0;
}