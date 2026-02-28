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

#include "settings.h"


bool Test::Table() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    sol::state lua;

    TableType type = TableType::NONE;
    sol::table aa = lua.create_table_with();

    type = GetTableType( aa );
    
    std::string res = ( type == TableType::UNDETERMINED ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    aa[1] = "hello";
    aa[2] = "thing";

    type = GetTableType( aa );
    
    res = ( type == TableType::LIST ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    aa["asdf"] = "asdf";

    type = GetTableType( aa );
    
    res = ( type == TableType::DICTIONARY ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // ---------------------------------------------------------------------------------

    sol::table t1 = lua.create_table_with(
        "a", 3,
        "b", 7
    );

    sol::table t2 = lua.create_table_with(
        "a", 3,
        "b", 7,
        "c", 5
    );

    bool compRes = CompareTable( t1, t2 );
    std::cout << compRes << std::endl;

    sol::table a = lua.create_table_with(
        "b", 5,
        "c", lua.create_table_with(
            "d", "hello"
        )
    );

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

    sol::table d = lua.create_table_with(
        "a", lua.create_table()
    );

    sol::table e = CopyTable( lua, d );
    
    e["a"] = lua.create_table_with(
        1, "hello",
        2, "heloo2",
        3, "hello4"
    );

    sol::table f = CombineTable::ToNew( lua, d, e, CombineTable::OVERWRITE_EXISTING | CombineTable::PRESERVE_TYPES );
    ShowTable( f );

    d["a"]["b"] = "B";
    f = CombineTable::ToNew( lua, d, e, CombineTable::OVERWRITE_EXISTING | CombineTable::PRESERVE_TYPES );
    ShowTable( f );

    sol::table g = lua.create_table_with(
        "a", lua.create_table()
    );

    sol::table h = CopyTable(lua, g);
    h["a"]["b"] = 10;
    h["a"]["c"] = "dvsjhk";

    sol::table i = CombineTable::ToNew(
        lua, g, h, CombineTable::OVERWRITE_EXISTING | CombineTable::PRESERVE_TYPES
    );

    sol::table expected = CopyTable(lua, h);
    std::string testRes = CompareTable( h, i ) ? "Passed" : "Failed"; 
    std::cout << "Test " << testRes << std::endl;

    return true;
}

bool Test::All() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

#ifdef DISABLE_TESTING_LOGS
    log_set_quiet(true);
    std::cout << "[LOGGING DISABLED]" << std::endl;
#endif

    bool res = true;
    
    // funcs is a list of pointers to functions that return bools
    bool (*funcs[])() = {
        Test::Table
    };

    // iterate functions and call them
    for ( const auto &func : funcs ) {
        res = res && func();
    }

#ifdef DEV 
    // re-enable logging if dev mode
    log_set_quiet(false);
#endif

    return res;
}