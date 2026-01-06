#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"

// required for the node build scripts
#include "build.hpp"

// include log.h as a C lib
extern "C" {
    #include "log/log.h"
}

// all errors in a namespace
namespace errors {
    
    // node based errors
    namespace node {

        // loading errors
        enum class load {
            OK,
            FILE,
            INTEGRITY,
            BUILD,
            NO_NODES
        };

    };

    // node based errors
    namespace player {

        // loading errors
        enum class load {
            OK,
            FILE,
            INTEGRITY
        };

    };

};


// some special constants for names of variables in lua
const std::string LUA_NODE_NAME = "name";
const std::string LUA_NODE_LAND = "on_land";
const std::string LUA_NODE_LEAVE = "on_leave";

const std::string LUA_NODE_TEMPLATE = "NODE_DATA_TEMPLATE";
const std::string LUA_NODE_AVAILABLE = "AVAILIBLE_NODES";

const std::string LUA_CORE_NODE_FILE = "core/node_data.lua";

// player constants
const std::string LUA_CORE_PLAYER_FILE = "core/player_data.lua";

const std::string LUA_CORE_PLAYER_TEMPLATE = "PLAYER_DATA_TEMPLATE";
const std::string LUA_CORE_PLAYER_DATA = "PLAYER_DATA";

const std::string LUA_CORE_PLAYER_NAME = "name";
const std::string LUA_CORE_PLAYER_POSITION = "position_id";


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

int check_integrity(sol::state &lua) {
    // check if there is an "array" (table) for availible nodes
    sol::optional<sol::table> avail = lua[LUA_NODE_AVAILABLE];
    
    // if the template does not exist that fail the integrity test
    if ( !avail ) {
        log_error("Availible nodes array does not exist.");
        return 1;
    }

    // ensure that the node template contains the expected values (additional values will not be penalised)
    sol::optional<sol::table> node_template = lua[LUA_NODE_TEMPLATE];

    // if it doesn't exist fail
    if ( !node_template ) {
        log_error("Node template does not exist.");
        return 1;
    }

    // check the node template against expected required
    if ( check_default_node_table( node_template.value() ) == 1 ) { return 1; }

    // finally check for a build function
    sol::optional<sol::function> build_func = lua["build"];

    if ( !build_func ) {
        log_error("Build function not found.");
        return 1;
    }

    return 0;
}

// builds all of the nodes
int build(sol::state &lua, std::vector<std::string> &node_types) {
    sol::protected_function build = lua["build"];

    // if does not exist
    if ( !build ) { return 1; }

    // get the result of the function (which should be none)
    auto res = build();

    // if invalid, return
    if ( !res.valid() ) { return 1; }
  
    sol::table avail = lua[LUA_NODE_AVAILABLE];

    // ensure that there are nodes availible
    if ( avail.size() == 0 ) { return 1; }

    // iterate values and add to the vector
    for ( const auto &item : avail ) {
        sol::table node = item.second;

        // if the node is invalid skip
        if ( !node ) { continue; }
        // skip if node has no default values
        if ( check_default_node_table( node ) == 1 ) { continue; }

        // add the node name to the vector
        node_types.push_back( node[LUA_NODE_NAME] );
    }

    return 0;
}

// gets all of the node types and verifies the integrity
errors::node::load get_nodes(sol::state &lua, std::vector<std::string> &node_types) {
    // try to get the lua file
    try {
        lua.safe_script_file(LUA_CORE_NODE_FILE);
        log_info("%s is found and opened with no errors.", LUA_CORE_NODE_FILE.c_str());
    }
    catch (const sol::error &e) {
        std::cout << e.what() << std::endl;
        log_error("%s failed to open.", LUA_CORE_NODE_FILE.c_str());
        return errors::node::load::FILE;
    }

    // check integrity of the core lua files
    if ( check_integrity(lua) != 0 ) {
        log_error("%s integrity check failed.", LUA_CORE_NODE_FILE.c_str());
        return errors::node::load::INTEGRITY;
    }

    log_info("%s integrity check success.", LUA_CORE_NODE_FILE.c_str());

    // build and check result
    if ( build(lua, node_types) != 0 ) {
        log_error("%s build failed.", LUA_CORE_NODE_FILE.c_str());
        return errors::node::load::BUILD;
    }

    log_info("%s build success.", LUA_CORE_NODE_FILE.c_str());

    return errors::node::load::OK;
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

int check_player_integrity(sol::state &lua) {
    sol::optional<sol::table> data_table = lua[LUA_CORE_PLAYER_TEMPLATE];

    if ( !data_table ) {
        log_error("Player data template not found.");
        return 1;
    }

    // this will determine the overall outcome if not already returned 1
    if ( check_default_player_template(data_table.value()) == 1 ) {
        log_error("Player template does not contain integral data.");
        return 1;
    }

    return 0;
}

// creates a new player data variable in the lua that stores a copy of the template populated with real data
errors::player::load get_player_data(sol::state &lua) {
    try {
        lua.safe_script_file("core/player_data.lua");
        log_info("%s is found and opened with no errors.", LUA_CORE_PLAYER_FILE.c_str());
    }
    catch (const sol::error &e) {
        std::cout << e.what() << std::endl;
        log_error("%s failed to open.", LUA_CORE_NODE_FILE.c_str());
        return errors::player::load::FILE;
    }

    if ( check_player_integrity(lua) == 1 ) {
        log_info("%s integrity check failed.", LUA_CORE_PLAYER_FILE.c_str());
        return errors::player::load::INTEGRITY;
    }

    log_info("%s integrity check success.", LUA_CORE_PLAYER_FILE.c_str());

    // create a copy of the template to house the actual player data
    lua.create_named_table(LUA_CORE_PLAYER_DATA);

    // achieve by copying the data with a for loop
    for ( const auto &item : lua[LUA_CORE_PLAYER_TEMPLATE].get<sol::table>() ) {
        lua[LUA_CORE_PLAYER_DATA][item.first] = item.second;
    }

    return errors::player::load::OK;
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

    if ( get_nodes( lua, node_types ) != errors::node::load::OK ) {
        log_fatal("An error has occurred when loading. Aborting...");
        
        // premature end
        fclose(fp);
        return 1;
    }

    if ( get_player_data( lua ) != errors::player::load::OK ) {
        log_fatal("An error has occurred when loading. Aborting...");
        
        // premature end
        fclose(fp);
        return 1;
    }

    // test to check the player data copies
    lua[LUA_CORE_PLAYER_DATA]["name"] = "bob";
    lua[LUA_CORE_PLAYER_TEMPLATE]["name"] = "template bob";

    std::string name = lua[LUA_CORE_PLAYER_TEMPLATE]["name"];

    // this shows that the var only stores the most recent check to the lua (name is not a reference)
    lua[LUA_CORE_PLAYER_TEMPLATE]["name"] = "not template bob";

    std::cout << name << " " << lua[LUA_CORE_PLAYER_DATA]["name"].get<std::string>() << std::endl;

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