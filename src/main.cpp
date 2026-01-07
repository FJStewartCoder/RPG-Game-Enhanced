#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"
#include "player.hpp"

// required for the node build scripts
#include "build.hpp"
#include "build_help.hpp"

#include "extension.hpp"

// include log.h as a C lib
extern "C" {
    #include "log/log.h"
}

#include <filesystem>

// all errors in a namespace
enum class errors {
    OK,
    FILE,
    BUILD,
    EXTEND
};


int inject_core_node_data(sol::state &lua) {
    // create a new table with the following data
    lua[LUA_NODE_TEMPLATE] = lua.create_table_with(
        LUA_NODE_NAME, "Node Name",
        LUA_NODE_LAND, []() {},
        LUA_NODE_LEAVE, []() {}
    );

    // create empty list for the availible nodes
    lua[LUA_NODE_AVAILABLE] = lua.create_table();

    // persitently keep the new nodes in lua space
    lua["NODE_QUEUE"] = lua.create_table();

    return 0;
}

int inject_core_player_data(sol::state &lua) {
    lua[LUA_CORE_PLAYER_DATA] = lua.create_table_with(
        LUA_CORE_PLAYER_NAME, "Player Name",
        LUA_CORE_PLAYER_POSITION, 0
    );

    return 0;
}

int inject_core(sol::state &lua) {
    inject_core_node_data(lua);
    inject_core_player_data(lua);

    return 0;
}


int check_default_node_table(sol::table &table) {
    sol::optional<std::string> name = table[LUA_NODE_NAME];
    sol::optional<sol::function> on_land = table[LUA_NODE_LAND];
    sol::optional<sol::function> on_leave = table[LUA_NODE_LEAVE];

    if ( !name ) {
        log_error("Node data does not contain name field.");
        return 1;
    }
    else if ( !on_land ) {
        log_error("Node with name \"%s\" does not have landing function.", name.value().c_str());
        return 1;
    }
    else if ( !on_leave ) {
        log_error("Node with name \"%s\" does not have leaving function.", name.value().c_str());
        return 1;
    }

    return 0;
}

int check_default_player_template(sol::table &table) {
    sol::optional<std::string> name = table[LUA_CORE_PLAYER_NAME];
    sol::optional<int> position = table[LUA_CORE_PLAYER_POSITION];

    if ( !name ) {
        log_error("Player data does not contain name field.");
        return 1;
    }
    else if ( !position ) {
        log_error("Player template does not have position data.");
        return 1;
    }
    
    return 0;
}

// could return none
sol::optional<sol::table> get_node_data(sol::state &lua, std::string name) {
    sol::table node_options = lua[LUA_NODE_AVAILABLE];

    // currently is empty until we find the table
    sol::optional<sol::table> found_table;

    // iterate the list of nodes and check if the node has the name that we are looking for
    for ( const auto &node : node_options ) {
        auto node_table = node.second.as<sol::optional<sol::table>>();

        // skip if the node table does not exist as a table or is not a valid table/
        if ( !node_table ) {
            continue;
        }
        else if ( check_default_node_table( node_table.value() ) != 0 ) {
            continue;
        }

        // if the names match, set found to the current table and break
        if ( node_table.value()[LUA_NODE_NAME] == name ) {
            found_table = node_table;
            break;
        }
    }

    // return no matter what is found
    return found_table;
}

template <typename T>
bool is_in(T item, std::vector<T> array) {
    for ( const auto &i : array) {
        if ( i == item ) {
            return true;
        }
    }

    return false;
}

node_directions get_player_input(node_t *node) {
    std::vector<std::string> options;

    // get each option
    if ( node->left != nullptr ) { options.push_back("left"); }
    if ( node->right != nullptr ) { options.push_back("right"); }
    if ( node->up != nullptr ) { options.push_back("up"); }
    if ( node->down != nullptr ) { options.push_back("down"); }
    if ( node->forward != nullptr ) { options.push_back("forward"); }
    if ( node->back != nullptr ) { options.push_back("back"); }
    if ( node->next != nullptr ) { options.push_back("next"); }
    if ( node->previous != nullptr ) { options.push_back("previous"); }

    std::cout << "Select an option: ";
    for ( const auto &item : options ) {
        std::cout << item << ", ";
    }
    std::cout << "quit: ";

    // add the quit option (after the other option for correct punctuation)
    options.push_back("quit");

    // the user input variable
    std::string input;

    while ( true ) {
        // get the user input
        std::cin >> input;

        if ( !is_in(input, options) ) {
            std::cout << "Please try again!" << std::endl;
            continue;
        }

        break;
    }

    // if block again for correct return
    if ( input == "left" ) { return NODE_LEFT; }
    else if ( input == "right" ) { return NODE_RIGHT; }
    else if ( input == "up" ) { return NODE_UP; }
    else if ( input == "down" ) { return NODE_DOWN; }
    else if ( input == "forward" ) { return NODE_FORWARD; }
    else if ( input == "back" ) { return NODE_BACK; }
    else if ( input == "next" ) { return NODE_NEXT; }
    else if ( input == "previous" ) { return NODE_PREV; }

    return NODE_QUIT;
}

void gameloop(sol::state &lua, node_t *(&start_node)) {
    // reassign the name
    node_t *cur_node = start_node;

    // the running state
    bool running = true;

    // boolean to silence the stuck message
    bool silence_stuck = false;

    uint stuck_count = 0;
    const uint max_stuck_cycles = 100;

    // STEPS:
    // get node data
    // on land ( ensures that the start activates )
    // check if possible to move
    // ask direction ( if we did this first the start would not work )
    // on leave
    // traverse

    while ( running ) {
        // get the player data table
        sol::table player_data = lua[LUA_CORE_PLAYER_DATA];

        // stores the current location of the player
        int script_player_pos = player_data[LUA_CORE_PLAYER_POSITION];

        // check if the script is attempting to manage the player's position
        if ( script_player_pos >= 1 ) {
            try {
                node_t *new_pos = get_node(script_player_pos - 1);

                // set the current node to be the pointer to the new position
                cur_node = new_pos;
            }
            catch (std::exception &e) {
                log_info("Script attempted to manage position but the operation failed.");
            }
        }

        // regardless of attempt, this should be 0
        // set the script location to 0 to ensure that we know C++ is managing position
        player_data[LUA_CORE_PLAYER_POSITION] = 0;

        // get the current node data
        auto cur_node_data = get_node_data(lua, cur_node->node_type);

        log_info("Landed on node with type \"%s\".", cur_node->node_type.c_str());
    
        // if data exists run on land
        if ( cur_node_data ) {
            sol::protected_function on_land = cur_node_data.value()[LUA_NODE_LAND];

            // pass in the unique data, the node data and the player data
            auto res = on_land(cur_node->unique_data, cur_node_data.value(), player_data);

            if ( !res.valid() ) {
                log_error("Landing function failed.");

                // error details
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // ask direction
        node_directions chosen_direction = NODE_NONE;

        if ( can_traverse(cur_node) == NODE_ERROR ) {
            stuck_count++;

            // user input for quitting or not
            bool will_quit = false;

            if ( stuck_count >= max_stuck_cycles ) {
                log_info("Exceeded max stuck limit. Automatically exitting.");
                std::cout << "[ERROR] You have been stuck longer than the stuck limit. Aborting..." << std::endl;
                
                // force quit
                will_quit = true;
                silence_stuck = true;
            }

            log_error("Player is stuck. Count: %d/%d", stuck_count, max_stuck_cycles);

            if ( !silence_stuck ) {
                std::cout << "[ERROR] You may be stuck as you are unable to move. It may be possible that external scripts are handling movement." << std::endl;
            
                std::cout << "Type q to quit or s to silence stuck message: ";
                // get use input
                std::string input;
                std::cin >> input;

                for ( const auto &c : input ) {
                    if ( c == 'q' ) {
                        will_quit = true;
                        break;
                    }
                    else if ( c == 's' ) {
                        silence_stuck = true;
                    }
                }
            } 
            
            // quit
            if ( will_quit ) {
                running = false;
                break;
            }

            // will not run any further until can move or quit
            continue;
        }

        // if we escape, then reset the stuck counter
        stuck_count = 0;

        // get the direction that the player want to move.
        chosen_direction = get_player_input(cur_node);

        // quit if chosen to quit
        if ( chosen_direction == NODE_QUIT ) {
            running = false;
            break;
        }

        // if data exists run on leave
        if ( cur_node_data ) {
            sol::protected_function on_leave = cur_node_data.value()[LUA_NODE_LEAVE];

            // pass in the unique data, the node data and the player data
            auto res = on_leave(cur_node->unique_data, cur_node_data.value(), player_data);

            if ( !res.valid() ) {
                log_error("Leaving function failed.");

                // error message
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // actually traverse
        node_errors res = traverse_node(cur_node, chosen_direction);

        // if there is an error when attempting to log
        if ( res == NODE_ERROR ) {
            log_error("Node traversal failed.");
        }
    }
}

int main() {
    // open the log file
    FILE *fp = fopen("log.txt", "w");

    // will always log
    log_add_fp(fp, 0);
    // log_set_quiet(true);

    // vector of strings (allows for expansion)
    std::vector<std::string> node_types;

    // create lua interpreter
    sol::state lua;

    // open libs so we have access to print
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table);

    auto scripts = std::filesystem::directory_iterator("./scripts");

    bool found_build_file = false;

    for ( const auto &fs_item : scripts ) {
        // currently skipping directories
        if ( fs_item.is_directory() ) {
            continue;
        }

        // ignore all non lua files
        if ( fs_item.path().extension() != ".lua" ) {
            continue;
        }

        if ( fs_item.path().filename() == "BUILD_FILE.lua" ) {
            found_build_file = true;
            continue;
        }

        log_info("File %s will build next.", fs_item.path().c_str());

        // build file if type is lua
        load_file(lua, fs_item.path());
    }

    if ( !found_build_file ) {
        log_fatal("No build file found.");

        fclose(fp);
        return 1;
    }

    // inject core data into the lua state
    inject_core(lua);
    // inject build tools
    inject_build_tools(lua);

    load_file(lua, "scripts/BUILD_FILE.lua");

    if ( has_func(lua, LUA_EXTEND_FUNC) ) {
        sol::protected_function extend_func = lua[LUA_EXTEND_FUNC];

        extend_func();
    }
    else {
        log_fatal("No extend function found.");

        fclose(fp);
        return 1;
    }

    if ( has_func(lua, LUA_BUILD_FUNC) ) {
        sol::protected_function build_func = lua[LUA_BUILD_FUNC];

        build_func();
    }
    else {
        log_fatal("No build function found.");

        fclose(fp);
        return 1;
    }

    // test if the data is found from the injection
    std::cout << lua[LUA_NODE_TEMPLATE]["name"].get<std::string>() << std::endl;
    std::cout << lua[LUA_CORE_PLAYER_DATA]["name"].get<std::string>() << std::endl;

    // test a function
    lua[LUA_NODE_TEMPLATE][LUA_NODE_LAND]();

    log_info("Building files complete.");

    // build the nodes
    build_node_queue(lua, lua[LUA_NODE_TEMPLATE]);

    for ( const auto &table : lua["NODE_QUEUE"].get<sol::table>() ) {
        node_types.push_back(table.second.as<sol::table>()["name"]);
    }

    // show nodes
    for ( const auto &node : node_types ) {
        log_trace("Found node with name \"%s\"", node.c_str());
    }

    // build test
    sol::table start_table = lua.create_table();
    start_table["data1"] = "123";

    sol::table start_table2 = lua.create_table();
    start_table2["data1"] = "234";

    int node1 = build_node(node_types, "Start", start_table);
    int node2 = build_node(node_types, "2", sol::table(), node1, NODE_RIGHT, false);
    int node3 = build_node(node_types, "Start", start_table2, node2, NODE_UP, false);

    // traversal test
    node_t *cur = get_node(node1);

    int res = traverse_node(cur, NODE_RIGHT);
    int res2 = traverse_node(cur, NODE_LEFT);

    std::cout << res << " " << res2 << std::endl;

    // the main game loop
    gameloop(lua, cur);

    // free the nodes
    free_nodes();

    // close the file
    fclose(fp);
    return 0;
}