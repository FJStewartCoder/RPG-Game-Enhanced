#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include <stdio.h>

// int as bool
int check_lua(lua_State *L, int r) {
    if ( r != LUA_OK ) {
        const char *errormsg = lua_tostring(L, -1);
        printf("[LUA] Error - %s\n", errormsg);
        return 0;
    }

    return 1;
}

int main() {
    // create lua interpreter
    lua_State *L = luaL_newstate();

    // open libs so we have access to print
    luaL_openlibs(L);

    int r = luaL_dofile(L, "src/test.lua");
    
    if ( check_lua(L, r) ) {
        lua_getglobal(L, "Test");

        if (lua_isfunction(L, -1)) {
            if ( check_lua(L, lua_pcall(L, 0, 0, 0)) ) {

            }
        }
    }

    lua_close(L);

    return 0;
}