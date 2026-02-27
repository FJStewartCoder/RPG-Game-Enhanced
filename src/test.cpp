#include "test.hpp"

#include "inject_api.hpp"
#include "inject_build.hpp"
#include "inject_core.hpp"

#include "sol/sol.hpp"

#include "build_help.hpp"
#include "build.hpp"
#include "campaign.hpp"
#include "custom_exception.hpp"
#include "nodes.hpp"
#include "save.hpp"
#include "table.hpp"
#include "to_binary.hpp"

extern "C" {
    #include "log/log.h"
}


bool Test::Table() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    sol::state lua;

    sol::table t1 = lua.create_table_with(
        "a", 3,
        "b", 7
    );

    sol::table t2 = lua.create_table_with(
        "a", 3,
        "b", 4
    );

    bool res = CompareTable( t1, t2 );
    std::cout << res << std::endl;

    lua["a"] = lua.create_table();
    sol::table a = lua["a"];

    a["b"] = 5;
    a["c"] = lua.create_table();
    a["c"]["d"] = "hello";

    sol::table b = CopyTable(lua, a);

    b["c"]["d"] = 45;
    b["b"] = 8;

    a["e"] = "THIS DOES NOT EXIST IN B";

    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);

    sol::table c = CombineTable::ToNew( lua, b, a, CombineTable::OVERWRITE_EXISTING | CombineTable::ADD_NEW_PROPERTIES | CombineTable::DEEP | CombineTable::PRESERVE_TYPES );

    std::cout << "FINAL TABLES [a, b, c]:" << std::endl;
    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);
    std::cout << std::endl;
    ShowTable(c);

    CombineTable::ToSource( lua, a, b, CombineTable::OVERWRITE_EXISTING );

    std::cout << "After combining to source" << std::endl;

    ShowTable(a);
    std::cout << std::endl;
    ShowTable(b);

    return true;
}

bool Test::All() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    bool res = true;
    
    // funcs is a list of pointers to functions that return bools
    bool (*funcs[])() = {
        Test::Table
    };

    // iterate functions and call them
    for ( const auto &func : funcs ) {
        res = res && func();
    }

    return res;
}