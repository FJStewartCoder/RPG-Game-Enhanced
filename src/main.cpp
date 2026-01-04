#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// just ensures that all required variables exist
int check_core_integrity(sol::state &lua) {
    return 0;
}

int main() {
    // create lua interpreter
    sol::state lua;

    // open libs so we have access to print
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table);

    // try to get the lua file
    try {
        lua.safe_script_file("core/node_data.lua");
        std::cout << "Ok\n";
    }
    catch (const sol::error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    sol::table a = lua["AVAILIBLE_NODES"];

    sol::table b = a[1];

    std::cout << std::string(b["name"]) << std::endl;

    return 0;
}