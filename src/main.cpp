#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"

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
            OK
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

// creates a new player data variable in the lua that stores a copy of the template populated with real data
errors::player::load get_player_data(sol::state &lua) {
    // TODO: implement
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

void gameloop(sol::state &lua, node_t *(&start_node)) {
    // reassign the name
    node_t *cur_node = start_node;

    // the running state
    bool running = true;

    // STEPS:
    // get node data
    // on land ( ensures that the start activates )
    // ask direction ( if we did this first the start would not work )
    // on leave
    // traverse

    while ( running ) {
        auto cur_node_data = get_node_data(lua, cur_node->node_type);

        log_info("Landed on node with type \"%s\".", cur_node->node_type.c_str());
    
        // if data exists run on land
        if ( cur_node_data ) {
            sol::protected_function on_land = cur_node_data.value()[LUA_NODE_LAND];

            auto res = on_land();

            if ( !res.valid() ) {
                log_error("Landing function failed.");
            }
        }

        // ask direction
        node_directions chosen_direction = NODE_NONE;

        // equivalent to quit
        if ( chosen_direction == NODE_NONE ) {
            running = false;
            break;
        }

        // if data exists run on leave
        if ( cur_node_data ) {
            sol::protected_function on_leave = cur_node_data.value()[LUA_NODE_LEAVE];
            auto res = on_leave();

            if ( !res.valid() ) {
                log_error("Leaving function failed.");
            }
        }

        // actually traverse
        traverse_node(cur_node, chosen_direction);
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

    for ( const auto &node : node_types ) {
        log_trace("Found node with name \"%s\"", node.c_str());
    }

    node_t node1 = build_node(node_types, "Start");
    node_t node2 = build_node(node_types, "2", &node1, NODE_RIGHT, false);

    node_t *cur = &node1;

    int res = traverse_node(cur, NODE_RIGHT);
    int res2 = traverse_node(cur, NODE_LEFT);

    std::cout << res << " " << res2 << std::endl;

    // the main game loop
    gameloop(lua, cur);

    // close the file
    fclose(fp);
    return 0;
}