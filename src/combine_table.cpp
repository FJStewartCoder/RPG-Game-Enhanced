#include "combine_table.hpp"

typedef struct {
    const bool overwrite_existing_properties;
    const bool add_new_properties;
    const bool preserve_types;
} table_rules_t;

table_rules_t parse_rules( const int ruleset ) {
    return {
        (ruleset & CombineTable::OVERWRITE_EXISTING) > 0,
        (ruleset & CombineTable::ADD_NEW_PROPERTIES) > 0,
        (ruleset & CombineTable::PRESERVE_TYPES) > 0
    };
}

sol::table CombineTable::ToNew( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {
    // create a new table
    sol::table result = lua.create_table();

    const table_rules_t rules = parse_rules( ruleset );

    return result;
}

int CombineTable::ToSource( sol::table &source, sol::table &other, int ruleset ) {
    const table_rules_t rules = parse_rules( ruleset );

    return 0;
}