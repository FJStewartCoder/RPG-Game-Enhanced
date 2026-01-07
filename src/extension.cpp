#include "extension.hpp"
#include "build_help.hpp"

// for "new_node_type" injection
#include "build.hpp"

#include "nodes.hpp"
#include "player.hpp"

int build_player_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {     
        std::cout << "Extend player called with extension " << item.first.as<std::string>() << std::endl; 
        core[LUA_CORE_PLAYER_DATA][item.first] = item.second;
    }

    return 0;
}

int build_node_extension(sol::state &core, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {
        std::cout << "Extend node called with extension " << item.first.as<std::string>() << std::endl;
        core[LUA_NODE_TEMPLATE][item.first] = item.second;
    }

    return 0;
}

int inject_build_tools(sol::state &core_state) {
    // add the extend player function
    core_state.set_function("extend_player", [&core_state](sol::table extension) {
        build_player_extension(core_state, extension);
    });

    // add the extend node function
    core_state.set_function("extend_node", [&core_state](sol::table extension) {
        build_node_extension(core_state, extension);
    });

    // add the add node function
    core_state.set_function("add_node", [&core_state](sol::table table) {
        new_node_type(core_state, table);
    });

    return 0;
}

// function to build the extensions
int load_file(sol::state &lua, std::string file_name) {
    try {
        lua.safe_script_file(file_name);
        std::cout << file_name << " has been opened" << std::endl;
    }
    catch ( const sol::error &e ) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}