#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <string>
#include "sol/sol.hpp"

// load file function that loads straight into a lua state
int load_file(sol::state &lua, std::string file_name);

// function to build the extensions
int load_file(sol::state &lua, sol::environment &load_env, std::string file_name);

#endif // EXTENSION_HPP