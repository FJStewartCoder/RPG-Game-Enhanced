#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"


int build_player_extension(sol::environment &env, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {     
        log_info("Extend player called with extension: \"%s\"", item.first.as<std::string>().c_str());
        env[engine::player::DATA][item.first] = item.second;
    }

    return 0;
}

int build_node_extension(sol::environment &env, sol::table extension) {
    // TODO: add validation to prevent overrighting default properties or properties that already exist
    for ( const auto &item : extension ) {
        log_info("Extend node called with extension: \"%s\"", item.first.as<std::string>().c_str());
        env[engine::node::TEMPLATE][item.first] = item.second;
    }

    return 0;
}

int inject_environment_tools(sol::environment &build_env, NodeManager &nodeManager) {
    build_env.set_function(
        engine::func::api::BUILD_NODE,

        [&nodeManager](
            std::string node_type,
            sol::table coords,
            sol::table unique_data = sol::table(),
            std::string blocked = ""
        ) {
            // TODO: implement proper coordinates system
            // parse coords

            log_debug("Called build function");
            return nodeManager.build_node(node_type, {0, 0, 0}, unique_data, blocked);
        }
    );

    return 0;
} 


int inject_build_tools(sol::environment &build_env, sol::environment &core, NodeManager &nodeManager) {
    // add the extend player function
    build_env.set_function(engine::func::api::EXTEND_PLAYER, [&core](sol::table extension) {
        return build_player_extension(core, extension);
    });

    // add the extend node function
    build_env.set_function(engine::func::api::EXTEND_NODE, [&core](sol::table extension) {
        return build_node_extension(core, extension);
    });

    // add the add node function
    build_env.set_function(engine::func::api::ADD_NODE_TYPE, [&core, &nodeManager](sol::table table) {
        return nodeManager.new_node_type(core, table);
    });

    inject_environment_tools(build_env, nodeManager);

    return 0;
}
