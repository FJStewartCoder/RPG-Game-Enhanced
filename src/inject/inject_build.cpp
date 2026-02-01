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
            sol::table unique_data,
            int previous_node_id,
            std::string relation,
            bool one_way
        ) {
            log_debug("Called build function");
            return nodeManager.build_node(node_type, unique_data, previous_node_id, relation, one_way);
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
