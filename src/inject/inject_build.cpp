#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"


// TODO: implement later
coordinates_t parse_coordinate_table(sol::table &coords) {
    coordinates_t parsed_coords;

    // initialise coordinates
    init_coords(&parsed_coords);

    const bool invalid_length = coords.size() != 3;

    // return incomplete coordinates
    if ( invalid_length ) {
        return parsed_coords;
    }

    // check if using x=, y=, z= system or a list of 3 ints
    // then, parse each value to a short ( if over some boundry set as 0 )
    for ( const auto &point : coords ) {

    }

    return parsed_coords;
}


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
            short x,
            short y,
            short z,
            sol::table unique_data = sol::table(),
            std::string blocked = ""
        ) {
            // parse coords
            // TODO: implement later
            // const coordinates_t parsed_coords = parse_coordinate_table(coords);

            log_debug("Called build function");
            log_debug("Coordinates are (%d %d %d)", x, y, z);
            log_debug("Blocked status is (%s)", blocked.c_str());

            return nodeManager.build_node(node_type, {x, y, z}, unique_data, blocked);
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
