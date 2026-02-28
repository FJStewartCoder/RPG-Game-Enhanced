#include "table.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "to_binary.hpp"

// COMPARE SCRIPTS ----------------------------------------------------------------------------

// check t1 compared to t2 but not the other way around
bool CompareSingleTable( sol::table &t1, sol::table &t2 ) {
    log_trace("Called function \"%s( table, table )\"", __FUNCTION__);

    for ( const auto &item : t1 ) {
        // get the key
        const auto key = item.first;

        // get both values
        const auto val = item.second;
        const sol::object otherVal = t2[key];

        // if the t2 is missing a key, they are not the same
        if ( !otherVal.valid() ) { 
            log_debug(
                "t2 does not have key %s",
                ObjectToString( otherVal ).c_str()
            );

            return false;
        }

        // get the type
        const sol::type valType = val.get_type();
        const sol::type otherValType = val.get_type();

        // if the types are different the data is not the same
        if ( valType != otherValType ) {
            log_debug("Types do not match; the tables are not the same");
            return false;
        }

        // if one is table and both are same type then both are table
        if ( valType == sol::type::table ) {
            // get both new values are references
            sol::table valRef = val;
            sol::table otherRef = otherVal;

            // if the subtables are different then return false
            if ( !CompareTable(valRef, otherRef) ) { return false; }
        }
        // if not tables but same type just compare
        else if ( val != otherVal ) {
            log_debug("Values are not the same");
            return false;
        }
    }

    // if we don't return false at any other point, they must be the same
    return true;
}

bool CompareTable( sol::table &t1, sol::table &t2 ) {
    log_trace("Called function \"%s( table, table )\"", __FUNCTION__);

    // get the result of both ways around
    return CompareSingleTable( t1, t2 ) && CompareSingleTable( t2, t1 );
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

        log_debug(
            "Got %s=%s with type %s",
            ObjectToString(key).c_str(),
            ObjectToString(val).c_str(),
            sol::type_name(NULL, valType).c_str()
        );

        // if table, copy the table
        if ( valType == sol::type::table ) {
            // temporary table to be used as a reference
            sol::table subTable = val.as<sol::table>();

            result[key] = CopyTable( lua, subTable );
        }

        // else copy the value
        else {
            result[key] = val;
        }
    }
    
    log_trace("Finished copying table");

    return result;
}

// OUTPUT SCRIPTS -----------------------------------------------------------------------------

// converts a sol object to a string
std::string ObjectToString( const sol::object &obj ) {

#ifndef REMOVE_FREQUENT_LOGS 
    log_trace("Called function \"%s( sol::object )\"", __FUNCTION__);
#endif  // REMOVE_FREQUENT_LOGS

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
            res = "\"" + obj.as<std::string>() + "\"";
            break;
        
        case sol::type::table:
            // show the table length
            res = "table[" + std::to_string( obj.as<sol::table>().size() ) + "]";
            break;

        default:
            res = "<" + sol::type_name(NULL, type) + ">";
            break;
    }

#ifndef REMOVE_FREQUENT_LOGS
    log_debug(
        "Returning object with type \"%s\" as string: %s",
        sol::type_name(NULL, type).c_str(),
        res.c_str()
    );
#endif  // REMOVE_FREQUENT_LOGS

    return res;
}

void ShowTableReal( sol::table &table, int depth ) {

#ifndef REMOVE_FREQUENT_LOGS
    log_trace("Called function \"%s( table, %d )\"",
        __FUNCTION__,
        depth
    );
#endif  // REMOVE_FREQUENT_LOGS

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

// HELPER SCRIPTS -----------------------------------------------------------------------------

TableType GetTableType( const sol::table &table ) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    // invalid table is not a table
    if ( !table.valid() ) { return TableType::NONE; }

    // the expected first value of the pair to be a list
    size_t expected = 1;

    // iterate each key to look for the pattern 1, 2, 3, 4, 5, ...
    for ( const auto &item : table ) {
        const auto key = item.first;

        // if the key is not an index (int), then it is not a list
        if ( !key.is<int>() ) { return TableType::DICTIONARY; }

        // if the indexes do not match up, it is not a list
        // this is because it is possible to create a dictionary-like with ints as keys
        if ( item.first.as<int>() != expected ) { return TableType::DICTIONARY; }

        // increment the expected index
        expected++;
    }

    // if we get to the end, it is either a list (because its not a dict)
    // or there were never any items
    // check if expected is still 1 (would have been incremented otherwise)

    const bool isEmpty = expected == 1;

    if ( isEmpty ) { return TableType::UNDETERMINED; }

    // if not empty, must be a list
    return TableType::LIST;
}

bool IsList( const sol::table &table ) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    TableType type = GetTableType( table );

    return type == TableType::LIST || type == TableType::UNDETERMINED; 
}

// COMBINATION SCRIPTS ------------------------------------------------------------------------

typedef struct {
    const bool overwrite_existing_properties;
    const bool add_new_properties;
    const bool preserve_types;
    const bool deep_combine;
} table_rules_t;

table_rules_t parse_rules( const int ruleset ) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        ToBinary( ruleset ).c_str()
    );

    // set all of the values as constant booleans
    const bool overwrite_existing_properties = (ruleset & CombineTable::OVERWRITE_EXISTING) != 0;
    const bool add_new_properties = (ruleset & CombineTable::ADD_NEW_PROPERTIES) != 0;
    const bool preserve_types = (ruleset & CombineTable::PRESERVE_TYPES) != 0;
    const bool deep_combine = (ruleset & CombineTable::DEEP) != 0;

    // output the booleans
    log_debug(
        "Rules: OVERWRITE_EXISTING=%d, ADD_NEW_PROPERTIES=%d, PRESERVE_TYPES=%d, DEEP=%d",
        overwrite_existing_properties,
        add_new_properties,
        preserve_types,
        deep_combine
    );

    // return a struct using the pre-calculated values
    return {
        overwrite_existing_properties,
        add_new_properties,
        preserve_types,
        deep_combine
    };
}

bool TypeMatches( const sol::object &obj1, const sol::object &obj2 ) {
    log_trace(
        "Called function \"%s( %s, %s )\"",
        __FUNCTION__,
        ObjectToString( obj1 ).c_str(),
        ObjectToString( obj2 ).c_str()
    );

    const sol::type obj1Type = obj1.get_type();
    const sol::type obj2Type = obj2.get_type();

    log_debug(
        "Object 1 has type %s and Object 2 has type %s",
        sol::type_name(NULL, obj1Type).c_str(),
        sol::type_name(NULL, obj2Type).c_str()
    );

    // if the types are different then false
    if ( obj1Type != obj2Type ) {
        log_debug("Types do not match");
        return false;
    }

    // types must match if we are here

    // types are not tables so no additional processing
    if ( obj1Type != sol::type::table ) {
        log_debug("Types are not tables and match");
        return true;
    }

    sol::table obj1Table = obj1;
    sol::table obj2Table = obj2;

    TableType obj1TableType = GetTableType(obj1Table);
    TableType obj2TableType = GetTableType(obj2Table);

    // type matches so return true
    if ( obj1TableType == obj2TableType ) { 
        log_debug("Table types match");
        return true;
    }

    // final scenarios are:
    // DICT, LIST
    // LIST, DICT
    // UNKNOWN, OTHER

    // if one is undetermined, that means that they match ( because other is not also undetermined )
    // otherwise it is a permutation of dict and list
    bool res = obj1TableType == TableType::UNDETERMINED || obj2TableType == TableType::UNDETERMINED;

    if ( res ) {
        log_debug("Table types are similar");
        return true;
    }
    else {
        log_debug("Table types do not match");
        return false;
    }
}

int CombineTable::ToSource( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {
    log_trace("Called function \"%s( table, table, %s )\"",
        __FUNCTION__,
        ToBinary( ruleset ).c_str()
    );

    // get the rules
    const table_rules_t rules = parse_rules( ruleset );

    // if overwrite existing properties, iterate the source list and check if other has them
    if ( rules.overwrite_existing_properties ) {
        log_trace("Started overwriting existing properties");

        for ( const auto &item : source ) {
            // get the key and value
            const auto key = item.first;
            const auto val = item.second;

            log_debug(
                "Source has %s=%s",
                ObjectToString( key ).c_str(),
                ObjectToString( val ).c_str()
            );

            const sol::object otherVal = other[key];
            const bool otherHasKey = otherVal.valid();

            // if the other table does not have the same key, continue to next item 
            // else continue processing
            if ( !otherHasKey ) {
                log_debug(
                    "Other table does not have key %s. Not adding this property.",
                    ObjectToString( key ).c_str()
                );
                continue;
            }

            log_debug(
                "Other has %s=%s",
                ObjectToString( key ).c_str(),
                ObjectToString( otherVal ).c_str()
            );

            // get the types for the values
            const sol::type valType = val.get_type();
            const sol::type otherValType = otherVal.get_type();

            // check if the types are equivalent
            const bool typeMatches = TypeMatches( val, otherVal );

            // if we want to preserve types and they do not match, continue
            // this is invalid
            if ( rules.preserve_types && !typeMatches ) {
                log_trace("Types do not match and PRESERVE_TYPES=true; Not processing this value.");
                continue;
            }

            // from all points here, the values are either the same type or we don't care the type
            // so we can just copy over regardless

            // specific table handling
            if ( otherValType != sol::type::table ) {
                log_debug(
                    "Copying value %s to source table at key: %s",
                    ObjectToString(otherVal).c_str(),
                    ObjectToString(key).c_str()
                );

                // copy over the value
                source[key] = otherVal;
                continue;
            }

            // other value is a table so convert to a table to be used as a reference
            log_trace("Other value is a table");
            sol::table otherAsTable = otherVal;

            // table handling from here
            // THERE ARE SEVERAL SCENARIOS:
            // both dict
            // both list
            // both empty
            // one dict, one empty
            // one list, one empty
            // one dict, one list
            // only other is table

            // we can only deep combine if both are dictionaries and the user wants to
            // otherwise we must copy
            
            // *scriptly dict
            const bool jColeAndDeepCombine = ( 
                GetTableType( val ) == TableType::DICTIONARY &&
                GetTableType( otherVal ) == TableType::DICTIONARY &&
                rules.deep_combine
            );

            if ( jColeAndDeepCombine ) {
                // convert val to a table to be used as a reference
                // it must now be a table
                sol::table valAsTable = val;

                log_trace("Deep combining tables");

                // if we want to deep combine and both are table, combine using same rules
                CombineTable::ToSource( lua, valAsTable, otherAsTable, ruleset );

                // continue to next value
                continue;
            }
            
            // SINCE BOTH ARE NOT DICTIONARIES
            // deep copying is not possible so copy the table (other is a table)

            // so just make a copy and assign
            log_trace("Either value is not a table or not deep combining");

            log_debug(
                "Copying other table to source at key: %s",
                ObjectToString(key).c_str()
            );

            source[key] = CopyTable( lua, otherAsTable );
        }

        log_trace("Finished overwriting existing properties");
    }

    // if adding new properties, iterate the other list and check if source doesn't have them
    if ( rules.add_new_properties ) {
        log_trace("Started adding new properties");

        for ( const auto &item : other ) {
            const auto key = item.first;
            const auto val = item.second;

            log_debug(
                "Other has %s=%s",
                ObjectToString( key ).c_str(),
                ObjectToString( val ).c_str()
            );

            const bool sourceHasKey = source[key].valid();

            // if source has the key, then we are not proceding since we are not adding new data
            if ( sourceHasKey ) {
                log_trace("Source already has this key");
                continue;
            }
            
            // get the types for the values
            const sol::type valType = val.get_type();

            if ( valType == sol::type::table ) {
                log_debug(
                    "Copying table to source with key %s",
                    ObjectToString(key).c_str()
                );

                // create a temporary table to use as a reference
                sol::table t = val;
                source[key] = CopyTable( lua, t );
            }
            else {
                log_debug(
                    "Copying value %s to source table at key: %s",
                    ObjectToString(val).c_str(),
                    ObjectToString(key).c_str()
                );

                source[key] = val;
            }
        }

        log_trace("Finished adding new properties");
    }

    log_trace("Finished combining tables");

    return 0;
}

sol::table CombineTable::ToNew( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {
    log_trace("Called function \"%s( state&, table&, table&, %s )\"",
        __FUNCTION__,
        ToBinary( ruleset ).c_str()
    );

    // create a new table
    sol::table result = CopyTable( lua, source );

    // since we have created a new source which is a copy, we can just use the other function
    int res = CombineTable::ToSource( lua, result, other, ruleset );

    return result;
}