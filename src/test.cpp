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


bool Test::TableCategorisation() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    /*
    TEST EMPTY - SHOULD SAY EMPTY
    TEST DICT - SHOULD SAY DICT
    TEST ORDERED LIST ( 1, 2, 3, 4, 5... ) - SHOULD SAY LIST
    TEST UNORDERED LIST ( 5, 3, 1, 4, 2... ) - SHOULD ALSO SAY LIST
    */

    sol::state lua;
    TableType t;
    std::string res;
    
    // ---------------------------------------------------------------------

    sol::table a = lua.create_table();
    t = GetTableType( a );

    res = ( t == TableType::UNDETERMINED ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // ---------------------------------------------------------------------

    sol::table b = lua.create_table_with(
        "a", "A",
        "b", "B"
    );
    t = GetTableType( b );

    res = ( t == TableType::DICTIONARY ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // ---------------------------------------------------------------------

    sol::table c = lua.create_table_with(
        1, "A",
        2, "B"
    );
    t = GetTableType( c );

    // ShowTable(c);

    res = ( t == TableType::LIST ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // ---------------------------------------------------------------------

    sol::table d = lua.create_table_with(
        5, "E",
        2, "B",
        3, "C",
        1, "A",
        4, "D"
    );
    t = GetTableType( d );

    //  ShowTable(d);

    res = ( t == TableType::LIST ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // ---------------------------------------------------------------------

    sol::table e = lua.create_table_with(
        5, "E",
        2, "B",
        3, "C",
        "a", "A",  // one rogue value
        1, "A",
        4, "D"
    );
    t = GetTableType( e );

    // ShowTable(e);

    res = ( t == TableType::DICTIONARY ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    return true;
}

bool Test::CombineTable() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    /*
    ALL BELOW ASSUME OVERWRITE, PRESERVE TYPES AND DEEP

    (TEST) BOTH EMPTY -> Copy one to the other
    (TEST) BOTH LIST -> Copy other list to source
    (TEST) BOTH DICT -> Deep combine if deep else copy other to source

    (TEST) ONE EMPTY, ONE LIST -> Copy list to source
    (TEST) ONE EMPTY, ONE DICT -> Copy dict to source

    (TEST) ONE DICT, ONE LIST -> Do nothing because type is different 
    (TEST) ONE LIST, ONE DICT -> Do nothing because type is different 

    ONE DICT, ONE EMPTY -> Deep combine otherwise copy other to source
    ONE LIST, ONE EMPTY -> Copy empty list over original
    */

    sol::state lua;

    sol::table source;
    sol::table other;
    sol::table result;
    sol::table expected;

    std::string res;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table()
    );
    other = lua.create_table_with(
        "a", lua.create_table()
    );
    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = lua.create_table_with(
        "a", lua.create_table()
    );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table()
    );
    source["a"][1] = "hello";
    source["a"][2] = "abc";

    other = lua.create_table_with(
        "a", lua.create_table()
    );
    other["a"][1] = "not hello";
    other["a"][2] = "not abc";

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = lua.create_table_with(
        "a", lua.create_table_with(
            1, "not hello",
            2, "not abc"
        )
    );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 4,
            "c", "SEA"
        )
    );

    other = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 7,
            "c", 123978
        )
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 7,
            "c", "SEA"
        )
    );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table()
    );

    other = lua.create_table_with(
        "a", lua.create_table_with(
            1, "bob",
            2, "not bob"
        )
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, other );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table()
    );

    other = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 7,
            "c", 123978
        )
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, other );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    source = lua.create_table_with(
        "a", lua.create_table_with(
            1, 7,
            2, 123978
        )
    );

    other = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 7,
            "c", 123978
        )
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, source );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------

    result = CombineTable::ToNew(
        lua, other, source,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, other );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------
    
    source = lua.create_table_with(
        "a", lua.create_table_with(
            "b", 4,
            "c", "SEA"
        )
    );

    other = lua.create_table_with(
        "a", lua.create_table()
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, source );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    // -------------------------------------------------------------------------------------------
    
    source = lua.create_table_with(
        "a", lua.create_table_with(
            1, "bob",
            2, "someone else"
        )
    );

    other = lua.create_table_with(
        "a", lua.create_table()
    );

    result = CombineTable::ToNew(
        lua, source, other,
        CombineTable::OVERWRITE_EXISTING | CombineTable::DEEP | CombineTable::PRESERVE_TYPES
    );

    expected = CopyTable( lua, other );

    res = ( CompareTable( result, expected ) ) ? "Passed" : "Failed";
    std::cout << "Test " << res << std::endl;

    return true;
}

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

bool Test::Save() {
    sol::state lua;

    sol::table a = lua.create_table_with(
        "a", "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "b", 123,
        "c", 123.456,
        "d", true,
        "e", false,
        "f", lua.create_table_with(
            1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
            2, 123,
            3, 123.456,
            4, true,
            5, false
        )
    );

    FILE *fp = fopen( "test_save_file.txt", "wb" );
    WriteV2::Write( fp, a, false );
    fclose(fp);

    fp = fopen( "test_save_file.txt", "rb" );
    // read the type of the variable
    fgetc(fp);
    ReadV2::TableReturn t = ReadV2::Table( fp, lua );
    fclose(fp);

    ShowTable( t.value );
    std::cout << std::endl;
    ShowTable( a );

    std::cout << CompareTable( t.value, a ) << std::endl;

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
        Test::TableCategorisation,
        Test::CombineTable,
        Test::Table,
        Test::Save
    };

    size_t i = 1;

    // iterate functions and call them
    for ( const auto &func : funcs ) {
        printf("-- Test Set %d --\n", i);
        res = res && func();
        i++;
        printf("\n");
    }

#ifdef DEV 
    // re-enable logging if dev mode
    log_set_quiet(false);
#endif

    return res;
}