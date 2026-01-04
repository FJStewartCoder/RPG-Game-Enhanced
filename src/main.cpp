#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"

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

};

int check_default_node_table(sol::table &table) {
    sol::optional<std::string> name = table["name"];
    sol::optional<sol::function> on_land = table["on_land"];
    sol::optional<sol::function> on_leave = table["on_leave"];

    if ( !name ) { return 1; }
    else if ( !on_land ) { return 1; }
    else if ( !on_leave ) { return 1; }

    return 0;
}

int check_integrity(sol::state &lua) {
    // check if there is an "array" (table) for availible nodes
    sol::optional<sol::table> avail = lua["AVAILIBLE_NODES"];
    
    // if the template does not exist that fail the integrity test
    if ( !avail ) {
        return 1;
    }

    // ensure that the node template contains the expected values (additional values will not be penalised)
    sol::optional<sol::table> node_template = lua["NODE_DATA_TEMPLATE"];

    // if it doesn't exist fail
    if ( !node_template ) {
        return 1;
    }

    // check the node template against expected required
    if ( check_default_node_table( node_template.value() ) == 1 ) { return 1; }

    // finally check for a build function
    sol::optional<sol::function> build_func = lua["build"];

    if ( !build_func ) {
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
  
    sol::table avail = lua["AVAILIBLE_NODES"];

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
        node_types.push_back( node["name"] );
    }

    return 0;
}

// gets all of the node types and verifies the integrity
errors::node::load get_nodes(sol::state &lua, std::vector<std::string> &node_types) {
    // try to get the lua file
    try {
        lua.safe_script_file("core/node_data.lua");
        std::cout << "Ok\n";
    }
    catch (const sol::error &e) {
        std::cout << e.what() << std::endl;
        return errors::node::load::FILE;
    }

    // check integrity of the core lua files
    if ( check_integrity(lua) != 0 ) {
        return errors::node::load::INTEGRITY;
    }

    // build and check result
    if ( build(lua, node_types) != 0 ) {
        return errors::node::load::BUILD;
    }

    return errors::node::load::OK;
}

int main() {
    // vector of strings (allows for expansion)
    std::vector<std::string> node_types;

    // create lua interpreter
    sol::state lua;

    // open libs so we have access to print
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table);

    if ( get_nodes( lua, node_types ) != errors::node::load::OK ) {
        std::cout << "An error has occurred when loading. Aborting..." << std::endl;
        return 1;
    }

    for ( const auto &node : node_types ) {
        std::cout << node << std::endl;
    }

    node_t node1 = build_node("1");
    node_t node2 = build_node("1", &node1, NODE_RIGHT, false);

    node_t *cur = &node1;

    int res = traverse_node(&cur, NODE_RIGHT);
    int res2 = traverse_node(&cur, NODE_RIGHT);

    std::cout << res << " " << res2 << std::endl;

    return 0;
}