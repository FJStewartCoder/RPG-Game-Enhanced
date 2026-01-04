#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// all errors in a namespace
namespace errors {
    
    // node based errors
    namespace node {

        // loading errors
        enum class load {
            OK,
            FILE,
            INTEGRITY
        };

    };

};

int check_integrity(sol::state &lua) {
    // check if there is an "array" (table) for availible nodes
    sol::optional<sol::table> avail = lua["AVAILIBLE_NODES"];
    
    // if the template does not exist that fail the integrity test
    if ( !avail ) {
        return 1;
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

    if ( check_integrity(lua) == 1 ) {
        return errors::node::load::INTEGRITY;
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

    return 0;
}