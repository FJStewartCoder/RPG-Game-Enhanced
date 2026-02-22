#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "build.hpp"
#include "table.hpp"

#include "nodes.hpp"


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
