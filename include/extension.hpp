#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <string>
#include "sol/sol.hpp"

int inject_build_tools(sol::environment &build_env, sol::environment &core);

// function to build the extensions
int load_file(sol::state &lua, sol::environment &load_env, std::string file_name);

#endif // EXTENSION_HPP