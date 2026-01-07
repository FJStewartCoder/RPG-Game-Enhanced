#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <string>
#include "sol/sol.hpp"

// names of the build and extension functions that we expect
const std::string LUA_BUILD_FUNC = "build";
const std::string LUA_EXTEND_FUNC = "extend";

int inject_build_tools(sol::state &core_state);

// function to build the extensions
int load_file(sol::state &lua, std::string file_name);

#endif // EXTENSION_HPP