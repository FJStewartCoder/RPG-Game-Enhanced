#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"
#include "table.hpp"

#include "custom_exception.hpp"

// max and min are 32767 -32768 this is 0b0111 1111 1111 1111 and 0b1000 0000 0000 0000

// the biggest possible short 0xffff >> 1 because all 1s except the first (left to right)
const static inline int max_short = (short)(0xffff >> 1); 
// min short is 0b 1000 0000 0000 0000
const static inline int min_short = (short)(1 << 15);


bool is_valid_short( int num ) {
    return ( num <= max_short ) && ( num >= min_short );
}

coordinates_t parse_coordinate_table(sol::table &coords) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    // initialise x, y, and z
    int x, y, z; 

    // get the values from the table
    if ( IsList( coords ) ) {
        // if list, assume that the first 3 values are x, y, z
        x = coords[1].get_or( 0 );
        y = coords[2].get_or( 0 );
        z = coords[3].get_or( 0 );
    }
    else {
        // if dict-like, assume there are keys x, y, z
        x = coords["x"].get_or( 0 );
        y = coords["y"].get_or( 0 );
        z = coords["z"].get_or( 0 );
    }

    // if any are invalid, throw an exception
    if ( !is_valid_short(x) ) { throw CustomException("x must be between 32767 and -32768"); }
    if ( !is_valid_short(y) ) { throw CustomException("y must be between 32767 and -32768"); }
    if ( !is_valid_short(z) ) { throw CustomException("z must be between 32767 and -32768"); }

    // only valid shorts are not shoved in
    coordinates_t res = create_coords( x, y, z );

    log_debug("Parsed table to coords %s", coords_to_str(&res, true).c_str() );

    return res;
}

int build_player_extension( Campaign &campaign, sol::table extension) {
    log_trace("Called function \"%s( env, table )\"", __FUNCTION__);

    // get the template
    auto player_template = campaign.core_env[engine::player::DATA].get<sol::table>();

    // combine by only adding new properties
    CombineTable::ToSource(
        campaign.lua, player_template, extension,
        CombineTable::ADD_NEW_PROPERTIES
    );

    return 0;
}

int inject_environment_tools( Campaign &campaign ) {
    log_trace("Called function \"%s( Campaign& )\"", __FUNCTION__);

    campaign.build_env.set_function(
        engine::func::api::BUILD_NODE,

        [&campaign](
            std::string node_type,
            std::string location_name,
            sol::table coords,
            sol::table unique_data,
            std::string blocked = ""
        ) {
            // parse coords
            const coordinates_t parsed_coords = parse_coordinate_table(coords);

            return campaign.nodeManager.build_node(
                campaign.lua,
                node_type,
                location_name,
                parsed_coords,
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
        return build_player_extension( campaign, extension );
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

        [&campaign](
            sol::table source_coords,
            sol::table dest_coords,
            std::string dir,
            bool one_way = false,
            bool override_blocking = false
        ) {
            return campaign.nodeManager.make_connection(
                parse_coordinate_table( source_coords ),
                parse_coordinate_table( dest_coords ),
                str_to_direction(dir),
                one_way,
                override_blocking
            );
        }
    );

    inject_environment_tools( campaign );

    return 0;
}
