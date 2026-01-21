#ifndef INJECT_CORE_HPP
#define INJECT_CORE_HPP


#include "sol/sol.hpp"
#include "lua_engine_constants.hpp"


// injects all core functions into an environment
int inject_core(sol::environment &env);


#endif // INJECT_CORE_HPP