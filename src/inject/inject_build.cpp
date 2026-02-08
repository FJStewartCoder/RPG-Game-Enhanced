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
    for ( const auto &item : extension ) {     
        const std::string var = item.first.as<std::string>();

        log_info("Extend player called with extension: \"%s\"", var.c_str());
        
        const auto player_template = env[engine::player::DATA];
        
        if ( player_template[var] != sol::nil ) {
            log_warn("Script attempted to overwrite this property");
            continue;
        }

        player_template[var] = item.second;
    }

    return 0;
}

int build_node_extension(sol::environment &env, sol::table extension) {
    for ( const auto &item : extension ) {
        const std::string var = item.first.as<std::string>();

        log_info("Extend node called with extension: \"%s\"", var.c_str());
        
        const auto node_template = env[engine::node::TEMPLATE];
        
        if ( node_template[var] != sol::nil ) {
            log_warn("Script attempted to overwrite this property");
            continue;
        }

        node_template[var] = item.second;
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

    // add the make connection function
    build_env.set_function(
        engine::func::api::ARBITRARY_CONNECTION,

        // TODO: improve with parse coordinates once implemented
        [&core, &nodeManager](
            short x1,
            short y1,
            short z1,
            short x2,
            short y2,
            short z2,
            std::string dir,
            bool one_way = false,
            bool override_blocking = false
        ) {
            return nodeManager.make_connection(
                {x1, y1, z1},
                {x2, y2, z2},
                str_to_direction(dir),
                one_way,
                override_blocking
            );
        }
    );

    inject_environment_tools(build_env, nodeManager);

    return 0;
}
