#ifndef INJECT_BUILD_HPP
#define INJECT_BUILD_HPP

#include "sol/sol.hpp"
#include "lua_engine_constants.hpp"


#include "inject_build.hpp"


extern "C" {
    #include "log/log.h"
}

#include "build.hpp"
#include "campaign.hpp"


// injects build tools
int inject_build_tools(Campaign &campaign);


#endif // INJECT_BUILD_HPP