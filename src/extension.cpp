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