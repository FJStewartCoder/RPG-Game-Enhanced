#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"

// required for the node build scripts
#include "build.hpp"
#include "build_help.hpp"

#include "extension.hpp"

// include log.h as a C lib
extern "C" {
    #include "log/log.h"
}

#include "menus/menus.hpp"

// for all of the constants
#include "lua_engine_constants.hpp"

#include <filesystem>
// required for a BFS
#include <queue>

// inject headers
#include "inject/inject_api.hpp"
#include "inject/inject_core.hpp"
#include "inject/inject_build.hpp"

#include "save.hpp"

#include "input.hpp"

#include "campaign.hpp"

#include "table.hpp"



// all errors in a namespace
enum class errors {
    OK,
    FILE,
    BUILD,
    EXTEND
};

node_directions get_player_input(node_t *node) {
    log_trace("Called function \"%s( node )\"", __FUNCTION__);

    Menu menu("", "", "Select a direction");

    // create a list of all of the directions you can move
    node_directions directions[8] = {
        NODE_LEFT,
        NODE_RIGHT,
        NODE_UP,
        NODE_DOWN,
        NODE_FORWARD,
        NODE_BACK,
        NODE_NEXT,
        NODE_PREV
    };

    // iterate each direction in directions
    for ( int i = 0; i < 8; i++ ) {
        // get the direction
        const node_directions dir = directions[i];

        // get the node in a specific direction
        node_t *node_in_dir = get_node_in_direction( *node, dir );

        // if the new node is null, don't add it to the menu
        if ( node_in_dir == NULL ) {
            continue;
        }

        // if the unique_name is blank, show the user the type of the node
        if ( node_in_dir->unique_name == "" ) {
            menu.AddItem(
                MenuItem(
                    dir_to_string( dir ),
                    "Type: " + node_in_dir->node_type + ", Coords: " + coords_to_str(&node_in_dir->coords)
                )
            );
        }

        // if the node has a unique name, show the user the name
        else {
            menu.AddItem(
                MenuItem(
                    dir_to_string( dir ),
                    "Name: " + node_in_dir->unique_name + ", Coords: " + coords_to_str(&node_in_dir->coords)
                )
            );
        }
    }

    menu.AddItem(
        MenuItem("Quit")
    );
    
    auto menu_res = menu.ShowAliasList();
    const std::string input = menu_res->name;

    log_debug("Menu returned: %s.", input.c_str());

    // if block again for correct return
    if ( input == "Left" ) { return NODE_LEFT; }
    else if ( input == "Right" ) { return NODE_RIGHT; }
    else if ( input == "Up" ) { return NODE_UP; }
    else if ( input == "Down" ) { return NODE_DOWN; }
    else if ( input == "Forward" ) { return NODE_FORWARD; }
    else if ( input == "Back" ) { return NODE_BACK; }
    else if ( input == "Next" ) { return NODE_NEXT; }
    else if ( input == "Previous" ) { return NODE_PREV; }

    return NODE_QUIT;
}

// makes some action if required by virtual event
// else returns the same virtual event
VirtualEvents handle_virtual_event(Campaign &campaign) {
    log_trace("Called function \"%s( Campaign& )\"", __FUNCTION__);

    // create a constant copy
    const VirtualEvents ev = campaign.event;

    // use this copy to make an action
    switch (ev) {
        case VirtualEvents::NONE:
            break;
        case VirtualEvents::QUIT:
            log_debug("Virtual Event: Quit called");
            break;
    }

    // set the campaign's event to none
    campaign.event = VirtualEvents::NONE;

    // return the event that we handled if more processing is required
    return ev;
}

// returns true if the position has been moved by the script else false
bool handle_script_movement(Campaign &campaign, node_t *(&cur_node), sol::table &player_data) {
    log_trace("Called function \"%s( Campaign&, node, table )\"", __FUNCTION__);

    // stores the current location of the player
    sol::table script_player_pos = player_data[engine::player::POSITION];

    // interpretation of the lua position
    short script_x = player_data[engine::player::POSITION]["x"];
    short script_y = player_data[engine::player::POSITION]["y"];
    short script_z = player_data[engine::player::POSITION]["z"];

    // check if the position is synced
    bool position_sync = (cur_node->coords.x == script_x) && (cur_node->coords.y == script_y) && (cur_node->coords.z == script_z);

    log_debug( "C++: (%d %d %d), SCRIPT: (%d %d %d), SYNC: %d", cur_node->coords.x, cur_node->coords.y, cur_node->coords.z, script_x, script_y, script_z, position_sync );

    // check if the script is attempting to manage the player's position
    // by checking if any position data is different
    if ( position_sync ) {
        // position matches so C++ manages movement ( false )
        return false;
    }

    node_t *new_pos = campaign.nodeManager.get_node( { script_x, script_y, script_z } );

    if ( new_pos == NULL ) {
        log_warn("Script attempted to manage position but the operation failed.");
        return false;
    }

    // set the current node to be the pointer to the new position
    cur_node = new_pos;

    // message to inform me the script moved the player
    log_trace("Script moved player to node ID: %lld.", cur_node->coords.hash);

    return true;
}

void sync_player_position(node_t *cur_node, sol::table &player_data) {
    log_trace("Called function \"%s( node, table )\"", __FUNCTION__);

    // stores the current location of the player
    sol::table script_player_pos = player_data[engine::player::POSITION];

    // sync the position
    script_player_pos["x"] = cur_node->coords.x;
    script_player_pos["y"] = cur_node->coords.y;
    script_player_pos["z"] = cur_node->coords.z;
}

int gameloop(Campaign &campaign, node_t *start_node) {
    log_trace("Called function \"%s( Campaign&, node )\"", __FUNCTION__);

    sol::environment &core_env = campaign.core_env;

    // reassign the name
    node_t *cur_node = start_node;

    // the running state
    bool running = true;

    // get a reference to all of the node types
    TYPE_MAP &node_types = campaign.nodeManager.get_all_node_types();

    // STEPS:
    // get node data
    // on land ( ensures that the start activates )
    // check if script has managed movement
    // if true:
    //     try to move player
    // else:
    //     check if possible to move
    //     if can't move:
    //         quit gameloop
    //
    //     ask direction ( if we did this first the start would not work )
    // sync lua and c++ position
    // on leave
    // repeat script movement check but don't re-ask for movement
    // traverse

    while ( running ) {
        // get the current node data
        node_type_t cur_node_data = node_types[cur_node->node_type];

        log_debug("Current node has type: \"%s\", with ID: %lld", cur_node->node_type.c_str(), cur_node->coords.hash);

        // get the player data table
        sol::table player_data = core_env[engine::player::DATA];

        // get the landing function
        auto on_land = cur_node_data.on_land;

        // if data exists run on land
        if ( on_land ) {
            auto func = on_land.value();

            // pass in the unique data, the node data and the player data
            auto res = func(cur_node->unique_data, player_data);

            if ( !res.valid() ) {
                log_warn("Landing function failed.");

                // error details
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // virtual event handler ------------------------------------

        auto ev = handle_virtual_event(campaign);

        if ( ev == VirtualEvents::QUIT ) {
            return 0;
        }

        // ----------------------------------------------------------

        bool ask_user_for_direction = !handle_script_movement(campaign, cur_node, player_data);
        
        if ( ask_user_for_direction ) {
            if ( can_traverse(cur_node) == NODE_ERROR ) {
                // TODO: potentially create a similar unstuck system like before instead of just quitting
                log_fatal("Player is stuck");
                break;
            }

            // get the direction that the player want to move.
            node_directions chosen_direction = get_player_input(cur_node);

            // quit if chosen to quit
            if ( chosen_direction == NODE_QUIT ) {
                return 0;
            }

            // actually traverse
            node_errors res = traverse_node(cur_node, chosen_direction);

            // if there is an error when attempting to log
            if ( res == NODE_ERROR ) {
                log_error("Node traversal failed.");
            }
        }

        sync_player_position(cur_node, player_data);

        // get the landing function
        auto on_leave = cur_node_data.on_leave;

        // if on_leave exists, run on_leave
        if ( on_leave ) {
            auto func = on_leave.value();

            // pass in the unique data, the node data and the player data
            auto res = func(cur_node->unique_data, player_data);

            if ( !res.valid() ) {
                log_warn("Leaving function failed.");

                // error details
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // virtual event handler ------------------------------------

        ev = handle_virtual_event(campaign);

        if ( ev == VirtualEvents::QUIT ) {
            return 0;
        }

        // ----------------------------------------------------------

        handle_script_movement(campaign, cur_node, player_data);
        sync_player_position(cur_node, player_data);
    }

    return 0;
}

std::string get_savefile_name() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    std::cout << "Enter your savefile name" << std::endl;
    
    std::string filename;

    while ( true ) {
        std::string user_input = ReadStdin();

        filename = user_input + ".txt";
        const bool file_exists = std::filesystem::exists( engine::directories::SAVEFILES + "/" + filename );

        if ( file_exists ) {
            std::cout << "This file already exists" << std::endl;
            continue;
        }

        break;
    }

    return filename;
}

void new_campaign() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    Menu menu("Campaign Selection");

    Campaign campaign;
    auto campaigns = campaign.GetCampaigns();

    const bool empty = campaigns.size() == 0;

    // if there are no campaigns return to main menu
    if ( empty ) {
        log_error("No campaigns availible.");
        return;
    }

    for ( const auto &item : campaigns ) {
        menu.AddItem(item.first);
    }

    // add the back item
    menu.AddItem( 
        MenuItem( "Back" )
    );

    auto menu_res = menu.ShowList();
    const std::string campaign_choice = menu_res->name;

    if ( campaign_choice == "Back" ) {
        log_trace("User chose to back out of this menu");
        return;
    }

    std::string filename = get_savefile_name();

    campaign.SetSavefile( filename );
    
    if ( campaign.LoadCampaign(campaign_choice) ) {
        log_error("Loading campaign failed");
        return;
    }

    node_t *cur = campaign.nodeManager.get_node({0, 0, 0});
    if ( cur == NULL ) {
        return;
    }

    gameloop(campaign, cur);
    campaign.SaveToFile();
}


void load_campaign() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    std::filesystem::__cxx11::directory_iterator savefiles;

    try {
        savefiles = std::filesystem::directory_iterator(engine::directories::SAVEFILES);
    }
    catch ( std::filesystem::filesystem_error &e ) {
        log_error("There is no \"%s\" directory", engine::directories::SAVEFILES.c_str());
        return;
    }

    Menu savefile_menu("Load From Savefile");

    // iterate each file in savefiles
    // if the file is file, add it to the menu
    for ( const auto &file : savefiles ) {
        if ( file.is_directory() ) {
            continue;
        }

        FILE *fp = fopen( file.path().c_str(), "rb" );
        if ( fp == NULL ) {
            fclose(fp);
            continue;
        }

        file_metadata meta = read_file_metadata(fp);

        if ( meta.error != 0 ) {
            fclose(fp);
            continue;
        }

        savefile_menu.AddItem(
            MenuItem(
                file.path().filename(),
                "Campaign: " + meta.campaign_name
            )
        );

        fclose(fp);
    }

    // add the back item
    savefile_menu.AddItem(
        MenuItem( "Back" )
    );

    auto menu_res = savefile_menu.ShowList();
    const std::string campaign_choice = menu_res->name;

    if ( campaign_choice == "Back" ) {
        log_trace("User chose to back out of this menu");
        return;
    }

    Campaign campaign;
    campaign.SetSavefile( campaign_choice );

    if ( campaign.LoadFromFile() ) {
        log_error("Loading from file failed");
        return;
    }

    node_t *cur = campaign.nodeManager.get_node({0, 0, 0});
    if ( cur == NULL ) {
        return;
    }

    gameloop(campaign, cur);
    campaign.SaveToFile();
}

#ifdef DEV
void test_campaign() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    Menu menu("Test Campaign Selection");

    Campaign campaign;
    auto campaigns = campaign.GetCampaigns();

    const bool empty = campaigns.size() == 0;

    // if there are no campaigns return to main menu
    if ( empty ) {
        log_error("No campaigns availible.");
        return;
    }

    for ( const auto &item : campaigns ) {
        menu.AddItem(
            MenuItem(item.first)
        );
    }

    // add a quit option
    menu.AddItem(
        MenuItem( "Back" )
    );

    auto menu_res = menu.ShowList();
    const std::string campaign_choice = menu_res->name;

    if ( campaign_choice == "Back" ) {
        log_trace("User chose to back out of this menu.");
        return;
    }
    
    if ( campaign.LoadCampaign(campaign_choice) ) {
        log_error("Loading campaign failed");
        return;
    }

    node_t *cur = campaign.nodeManager.get_node({0, 0, 0});
    if ( cur == NULL ) {
        return;
    }

    gameloop(campaign, cur);
}
#endif

int main_menu() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    Menu menu("Main Menu", "Welcome to the game!");

    menu.AddItem(
        MenuItem(
            "New Campaign",
            "Create a new campaign and save to a save file"
        )
    );
    menu.AddItem(
        MenuItem(
            "Load Campaign",
            "Load an existing campaign from a save file"
        )
    );

#ifdef DEV
    menu.AddItem(
        MenuItem(
            "Test Campaign",
            "Test a campaign without a save file"
        )
    );
#endif

    menu.AddItem(
        MenuItem(
            "Quit"
        )
    );

    while ( true ) {

        auto res = menu.ShowList();
        const std::string choice = res->name;

        // log the option the user chose
        log_trace("User selected %s.", choice.c_str());

        // NEW CAMPAIGN
        if ( choice == "New Campaign" ) {
            new_campaign();
        }
        else if ( choice == "Load Campaign" ) {
            load_campaign();
        }

#ifdef DEV
        else if ( choice == "Test Campaign" ) {
            test_campaign();
        }
#endif

        else if ( choice == "Quit" ){
            break;
        }
        else {
            log_fatal("Whoever wrote the code to allow this is bad.");
        }

    }

    return 0;
}

bool test() {
    sol::state lua;

    lua["a"] = lua.create_table();
    sol::table a = lua["a"];

    a["b"] = 5;
    a["c"] = lua.create_table();
    a["c"]["d"] = "hello";

    sol::table b = CopyTable(lua, a);

    b["c"]["d"] = 45;
    b["b"] = 8;

    a["e"] = "THIS DOES NOT EXIST IN B";

    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);

    sol::table c = CombineTable::ToNew( lua, b, a, CombineTable::OVERWRITE_EXISTING | CombineTable::ADD_NEW_PROPERTIES | CombineTable::DEEP | CombineTable::PRESERVE_TYPES );

    std::cout << "FINAL TABLES [a, b, c]:" << std::endl;
    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);
    std::cout << std::endl;
    ShowTable(c);

    CombineTable::ToSource( lua, a, b, CombineTable::OVERWRITE_EXISTING );

    std::cout << "After combining to source" << std::endl;

    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);

    return 0;
}

int main() {
    // open the log file
    FILE *fp = fopen("log.txt", "w");

    // log_set_level(1);

    // will always log
    // log_add_fp(fp, 0);

#ifndef DEV
    log_set_quiet(true);
#endif // DEV

    main_menu();

    fclose(fp);
    return 0;
}