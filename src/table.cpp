#include "table.hpp"

extern "C" {
    #include "log/log.h"
}


// COPY SCRIPTS -------------------------------------------------------------------------------

sol::table CopyTable( sol::state &lua, sol::table &table ) {
    log_trace("Called function \"%s( state, table )\"", __FUNCTION__);

    sol::table result = lua.create_table();

    for ( const auto &item : table ) {
        // get some data for better readability
        const auto key = item.first;
        const auto val = item.second;
        const sol::type valType = val.get_type(); 

        // if table, copy the table
        if ( valType == sol::type::table ) {
            sol::table subTable = val.as<sol::table>();
            result[key] = CopyTable( lua, subTable );
        }

        // else copy the value
        else {
            result[key] = val;
        }
    }

    return result;
}

// OUTPUT SCRIPTS -----------------------------------------------------------------------------

// converts a sol object to a string
std::string ObjectToString( const sol::object &obj ) {
    log_trace("Called function \"%s( sol::object )\"", __FUNCTION__);

    const sol::type type = obj.get_type();
    std::string res = "";

    switch ( type ) {
        case sol::type::boolean:
            if ( obj.as<bool>() ) {
                res = "true";
            }
            else {
                res = "false";
            }

            break;
        
        case sol::type::function:
            res = "<FUNCTION>";
            break;
        
        case sol::type::number:
            // there are two types of number; int and float. So, parse each one differently
            // must check int first since ints will be valid floats so will always output float

            if ( obj.is<int>() ) {
                res = std::to_string( obj.as<int>() );
            }
            else {
                res = std::to_string( obj.as<float>() );
            }

            break;

        case sol::type::string:
            res = obj.as<std::string>();
            break;
        
        case sol::type::table:
            // show the table length
            res = "table[" + std::to_string( obj.as<sol::table>().size() ) + "]";
            break;

        default:
            res = "<" + sol::type_name(NULL, type) + ">";
            break;
    }

    return res;
}

void ShowTableReal( sol::table &table, int depth ) {
    log_trace("Called function \"%s( table, %d )\"",
        __FUNCTION__,
        depth
    );

    for ( const auto &item : table ) {
        const auto key = item.first;
        const auto val = item.second;

        const std::string spacing(depth * 2, ' ');

        if ( val.get_type() == sol::type::table ) {
            sol::table t = val.as<sol::table>();

            // show the key and opening bracket
            std::cout << spacing << ObjectToString(key) << " = {" << std::endl;

            // show the table indented by one
            ShowTableReal( t, depth + 1 );

            // show the ending bracket
            std::cout << spacing << "}" << std::endl;
        }
        else {
            std::cout << spacing << ObjectToString(key) << " = " << ObjectToString(val) << std::endl;
        }
    }
}

void ShowTable( sol::table &table ) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    ShowTableReal( table, 0 );
}

// COMBINATION SCRIPTS ------------------------------------------------------------------------

typedef struct {
    const bool overwrite_existing_properties;
    const bool add_new_properties;
    const bool preserve_types;
    const bool deep_combine;
} table_rules_t;

table_rules_t parse_rules( const int ruleset ) {
    log_trace("Called function \"%s( %b )\"",
        __FUNCTION__,
        ruleset
    );

    return {
        (ruleset & CombineTable::OVERWRITE_EXISTING) > 0,
        (ruleset & CombineTable::ADD_NEW_PROPERTIES) > 0,
        (ruleset & CombineTable::PRESERVE_TYPES) > 0,
        (ruleset & CombineTable::DEEP) > 0
    };
}

int CombineTable::ToSource( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {
    log_trace("Called function \"%s( table, table, %b )\"",
        __FUNCTION__,
        ruleset
    );

    // get the rules
    const table_rules_t rules = parse_rules( ruleset );

    // if overwrite existing properties, iterate the source list and check if other has them
    if ( rules.overwrite_existing_properties ) {
        ShowTable( source );

        for ( const auto &item : source ) {
            const auto key = item.first;
            const auto val = item.second;

            const auto otherVal = other[key];
            const bool otherHasKey = otherVal.valid();

            // if the other table does not have the same key, continue to next item 
            // else continue processing
            if ( !otherHasKey ) {
                continue;
            }

            // get the types for the values
            const sol::type valType = val.get_type();
            const sol::type otherValType = otherVal.get_type();

            // if we are not preserving types, 
            // copy the value from other to source at key
            // ( if table, need to make a copy )
            if ( !rules.preserve_types ) {
                if ( otherValType == sol::type::table ) {
                    // need to make a type copy to pass as a reference
                    sol::table t = otherVal;

                    // copy the table to the source
                    source[key] = CopyTable( lua, t );
                }
                else {
                    source[key] = otherVal;
                }

                // complete this loop so repeat for next value
                continue;
            }

            // perform type checking
            // if the types are not the same, don't copy
            if ( valType != otherValType ) {
                continue;
            }

            // values here have the same type
            // if the type is not a table, copy the value simply return to start of loop
            if ( valType != sol::type::table ) {
                source[key] = otherVal;
                continue;
            }

            sol::table valAsTable = val;
            sol::table otherAsTable = otherVal;

            // if we don't want to deep combine, simply make a copy
            if ( !rules.deep_combine ) {
                source[key] = CopyTable( lua, otherAsTable );
                continue;
            }

            // if we want to deep combine and both are table, combine using same rules
            CombineTable::ToSource( lua, valAsTable, otherAsTable, ruleset );
        }
    }

    // if adding new properties, iterate the other list and check if source doesn't have them
    if ( rules.add_new_properties ) {
        ShowTable( other );

        for ( const auto &item : other ) {
            const auto key = item.first;
            const auto val = item.second;

            const bool sourceHasKey = other[key].valid();

            // if source has the key, then we are not proceding since we are not adding new data
            if ( sourceHasKey ) {
                continue;
            }
            
            // get the types for the values
            const sol::type valType = val.get_type();

            if ( valType == sol::type::table ) {
                sol::table t = val;
                source[key] = CopyTable( lua, t );
            }
            else {
                source[key] = val;
            }
        }
    }

    return 0;
}

sol::table CombineTable::ToNew( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {
    log_trace("Called function \"%s( state&, table&, table&, %b )\"",
        __FUNCTION__,
        ruleset
    );

    // create a new table
    sol::table result = CopyTable( lua, source );

    // since we have created a new source which is a copy, we can just use the other function
    int res = CombineTable::ToSource( lua, result, other, ruleset );

    return result;
}