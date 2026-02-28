#ifndef TABLE_HPP
#define TABLE_HPP

#include "sol/sol.hpp"

bool CompareTable( sol::table &t1, sol::table &t2 );

sol::table CopyTable( sol::state &lua, sol::table &table );

std::string ObjectToString( const sol::object &obj );

void ShowTable( sol::table &table );

enum class TableType {
    LIST,  // list being defined as all consecutive numeric keys starting at 1
    DICTIONARY,  // not a list
    UNDETERMINED,  // the table is empty
    NONE  // it's not even table
};

// gets the type of the table
TableType GetTableType( const sol::table &table );

// just a GetTableType wrapper
// if undetermined, then this will show true
bool IsList( const sol::table &table );

namespace CombineTable {
    typedef enum {
        // will overwrite existing properties in the table
        // if this is on, a property other has will overwrite the property with the same name in source
        // if this is off, a property other has that source also has will not be overwritten
        OVERWRITE_EXISTING = 1,
        
        // if the other table has a property the original does not have, add it to the source table
        // if this is on, a property that other has that source does not have will be added to the result
        // if this is off, a property that other has that source does not have will not be added
        ADD_NEW_PROPERTIES = 1 << 1,

        // will use type checking when overwriting properties
        // if this is on, a property from other being written into source will be type checked to ensure that it is the same type as source. If the type matches, they will be combined
        // if this is off, a property from other being written into source will be combined regardless of type
        PRESERVE_TYPES = 1 << 2,

        // will deep combine tables
        // if this is on, will attempt to combine tables within the main table using the same rules
        // if this is off, will create a copy of the table based on the rules
        DEEP = 1 << 3,
    } rules;

    sol::table ToNew( sol::state &lua, sol::table &source, sol::table &other, int ruleset );

    int ToSource( sol::state &lua, sol::table &source, sol::table &other, int ruleset );
};

#endif  // TABLE_HPP