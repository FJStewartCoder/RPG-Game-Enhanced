cmake_minimum_required(VERSION 4.2.3)
set (CMAKE_CXX_STANDARD 17)

project(RPenGine)

include_directories(
    "include"
    "include/log"
    "include/sol"
    "include/menus"
    "include/inject"
)

set(LUA_INCLUDES "lauxlib.h;lua.h;lua.hpp;luaconf.h;lualib.h")

foreach( inc IN LISTS LUA_INCLUDES )
    file( REMOVE include/lua/${inc} )
endforeach()

# copy the correct includes for the platform
if(WIN32)
    set(LUA_INCLUDE_COPY_FROM "lua_includes/windows")
else()
    set(LUA_INCLUDE_COPY_FROM "lua_includes/linux")
endif()

message("Lua includes are being taken from: \"${LUA_INCLUDE_COPY_FROM}\"")

foreach( inc IN LISTS LUA_INCLUDES )
    file( COPY_FILE ${LUA_INCLUDE_COPY_FROM}/${inc} include/lua/${inc})
endforeach()

add_executable(
    ${PROJECT_NAME}

    src/main.cpp
    src/build_help.cpp
    src/build.cpp
    src/campaign.cpp
    src/extension.cpp
    src/input.cpp
    src/nodes.cpp
    src/save.cpp
    src/table.cpp

    src/inject/inject_api.cpp
    src/inject/inject_build.cpp
    src/inject/inject_core.cpp

    src/log/log.c
    src/menus/menus.cpp
)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC find_library(lib)
)

if (WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        lua54.lib
    )
else()
    target_link_libraries(
        ${PROJECT_NAME}
        liblua54.a
    )
endif()