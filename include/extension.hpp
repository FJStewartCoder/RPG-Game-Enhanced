#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <string>
#include "sol/sol.hpp"

int inject_build_tools(sol::state &core_state);

// function to build the extensions
int load_file(sol::state &lua, std::string file_name);

#endif // EXTENSION_HPP