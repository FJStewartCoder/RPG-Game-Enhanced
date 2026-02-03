#ifndef INJECT_API_HPP
#define INJECT_API_HPP


#include "sol/sol.hpp"
#include "lua_engine_constants.hpp"


int inject_api(sol::environment scripts_env, VirtualEvents &event);


#endif // INJECT_API_HPP