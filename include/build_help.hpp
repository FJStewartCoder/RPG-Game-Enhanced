#ifndef BUILD_HELP_HPP
#define BUILD_HELP_HPP

#include "sol/sol.hpp"

// checks if a lua state has a build function
bool has_func(sol::environment &env, std::string func_name);

#endif // BUILD_HELP_HPP