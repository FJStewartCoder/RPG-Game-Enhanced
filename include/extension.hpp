#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <string>
#include "sol/sol.hpp"

// names of the build and extension functions that we expect
const std::string LUA_BUILD_FUNC = "build";
const std::string LUA_EXTEND_FUNC = "extend";

// function to build the extensions
int build_file(sol::state &core_state, std::string file_name);

#endif // EXTENSION_HPP