#ifndef INJECT_BUILD_HPP
#define INJECT_BUILD_HPP

#include "sol/sol.hpp"
#include "lua_engine_constants.hpp"


// injects the build tools
int inject_build_tools(sol::environment &build_env, sol::environment &core);


#endif // INJECT_BUILD_HPP