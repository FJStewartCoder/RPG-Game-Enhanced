#ifndef COMBINE_TABLE_HPP
#define COMBINE_TABLE_HPP

#include "sol/sol.hpp"

namespace CombineTable {
    typedef enum rules {
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
    };

    sol::table ToNew( sol::state &lua, sol::table &source, sol::table &other, int ruleset ) {

    }

    int ToSource( sol::table &source, sol::table &other, int ruleset ) {

    }
};

#endif COMBINE_TABLE_HPP