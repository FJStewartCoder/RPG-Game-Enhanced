cmake_minimum_required(VERSION 4.3)
set (CMAKE_CXX_STANDARD 17)

project(RPenGine)

include_directories(
    "include"
    "include/log"
    "include/sol"
    "include/menus"
    "include/inject"
)

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

if (WIN32)
    add_library(
        LUALIB
        SHARED
        lua54.dll
    )
else()
    add_library(
        LUALIB
        SHARED
        lua54.dll
    )
endif()

set_target_properties(LUALIB PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    ${PROJECT_NAME}
    "C:/Users/finla/Downloads/RPG-Game-Enhanced/lib/lua54.lib"
)