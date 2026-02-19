#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"


// TODO: implement later
// ABLE TO USE IsList from table.hpp
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

// TODO: change both below functions to use the new table merge system
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

int inject_environment_tools( Campaign &campaign ) {
    log_trace("Called function \"%s( Campaign& )\"", __FUNCTION__);

    campaign.build_env.set_function(
        engine::func::api::BUILD_NODE,

        [&campaign](
            std::string node_type,
            std::string location_name,
            short x,
            short y,
            short z,
            sol::table unique_data = sol::table(),
            std::string blocked = ""
        ) {
            // parse coords
            // TODO: implement later
            // const coordinates_t parsed_coords = parse_coordinate_table(coords);

            return campaign.nodeManager.build_node(
                campaign.lua,
                node_type,
                location_name,
                create_coords( x, y, z ),
                unique_data,
                blocked
            );
        }
    );

    return 0;
} 

int inject_build_tools(Campaign &campaign) {
    log_trace("Called function \"%s( Campaign& )\"", __FUNCTION__);

    // add the extend player function
    campaign.build_env.set_function(engine::func::api::EXTEND_PLAYER, [&campaign](sol::table extension) {
        return build_player_extension(campaign.core_env, extension);
    });

    // add the add node function
    campaign.build_env.set_function(
        engine::func::api::ADD_NODE_TYPE,
        
        [&campaign](
            std::string type_name,
            sol::function on_land,
            sol::function on_leave,
            sol::table unique_data_template
        ) {
            return campaign.nodeManager.new_node_type(
                type_name,
                on_land,
                on_leave,
                unique_data_template
            );
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
                create_coords( x1, y1, z1 ),
                create_coords( x2, y2, z2 ),
                str_to_direction(dir),
                one_way,
                override_blocking
            );
        }
    );

    inject_environment_tools( campaign );

    return 0;
}
