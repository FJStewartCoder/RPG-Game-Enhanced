#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"


// TODO: implement later
coordinates_t parse_coordinate_table(sol::table &coords) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

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
    log_trace("Called function \"%s( env, table )\"", __FUNCTION__);

    for ( const auto &item : extension ) {     
        const std::string var = item.first.as<std::string>();

        log_info("Extend player called with extension: \"%s\"", var.c_str());
        
        const auto player_template = env[engine::player::DATA];
        
        if ( player_template[var] != sol::nil ) {
            log_warn("Script attempted to overwrite property: \"%s\"", var.c_str());
            continue;
        }

        player_template[var] = item.second;
    }

    return 0;
}

int build_node_extension(sol::environment &env, sol::table extension) {
    log_trace("Called function \"%s( env, table )\"", __FUNCTION__);

    for ( const auto &item : extension ) {
        const std::string var = item.first.as<std::string>();

        log_info("Extend node called with extension: \"%s\"", var.c_str());
        
        const auto node_template = env[engine::node::TEMPLATE];
        
        if ( node_template[var] != sol::nil ) {
            log_warn("Script attempted to overwrite property: \"%s\"", var.c_str());
            continue;
        }

        node_template[var] = item.second;
    }

    return 0;
}

int inject_module(Campaign &campaign) {
    campaign.build_env.set_function(
        engine::func::api::REQUIRE_MODULE,

        [&campaign](
            std::string module_name
        ) {
            return campaign.LoadModule( module_name );
        }
    );

    return 0;
}

int inject_environment_tools(sol::environment &build_env, NodeManager &nodeManager) {
    log_trace("Called function \"%s( env, NodeManager )\"", __FUNCTION__);

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

            return nodeManager.build_node(node_type, {x, y, z}, unique_data, blocked);
        }
    );

    return 0;
} 


int inject_build_tools(Campaign &campaign) {
    log_trace("Called function \"%s( env, env, NodeManager )\"", __FUNCTION__);

    // add the extend player function
    campaign.build_env.set_function(engine::func::api::EXTEND_PLAYER, [&campaign](sol::table extension) {
        return build_player_extension(campaign.core_env, extension);
    });

    // add the extend node function
    campaign.build_env.set_function(engine::func::api::EXTEND_NODE, [&campaign](sol::table extension) {
        return build_node_extension(campaign.core_env, extension);
    });

    // add the add node function
    campaign.build_env.set_function(
        engine::func::api::ADD_NODE_TYPE,
        
        [&campaign](sol::table table) {
            return campaign.nodeManager.new_node_type(campaign.core_env, table);
        }
    );

    // add the make connection function
    campaign.build_env.set_function(
        engine::func::api::ARBITRARY_CONNECTION,

        // TODO: improve with parse coordinates once implemented
        [&campaign](
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
            return campaign.nodeManager.make_connection(
                {x1, y1, z1},
                {x2, y2, z2},
                str_to_direction(dir),
                one_way,
                override_blocking
            );
        }
    );

    inject_environment_tools(campaign.build_env, campaign.nodeManager);

    return 0;
}
