#include "inject_api.hpp"

#include "menus/menus.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "table.hpp"
#include "nodes.hpp"


MenuItem table_to_item( const sol::table &table ) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    // try to get both the name and description
    // if neither are available
    std::string name;
    std::string description;

    // if table is a list, assume { name, description }
    if ( IsList( table ) ) {
        log_trace("Table type is list");

        name = table[1].get_or<std::string>("");
        description = table[2].get_or<std::string>("");
    }
    // if dictionary-like, assume { name="name", description="description" }
    else {
        log_trace("Table type is dictionary-like");

        name = table["name"].get_or<std::string>("");
        description = table["description"].get_or<std::string>("");
    }

    // show a debug message of the data in the new MenuItem
    log_debug("New MenuItem created with: name=\"%s\", description=\"%s\"", name.c_str(), description.c_str());

    return MenuItem( name, description );
}

// wrapper for menu functions for lua
std::string menu_wrapper(std::string name, std::string message, sol::table options, menu_type_t menu_type) {
    log_trace(
        "Called function \"%s( %s, %s, table, %d )\"",
        __FUNCTION__,
        name.c_str(),
        message.c_str(),
        menu_type
    );

    Menu menu(name, "", message);

    for ( const auto &item : options ) {
        // get the type of the item that has attempted to be passed to the function
        const auto type = item.second.get_type();

        // if the type is string, assume that this is just the name
        if ( type == sol::type::string ) {
            log_trace("Script has passed a string as this menu item");

            menu.AddItem( 
                MenuItem( item.second.as<std::string>() )
            );
        }

        // if the type is table, try to extract data from the table
        else if ( type == sol::type::table ) {
            log_trace("Script passed a table as this menu item");

            menu.AddItem( 
                MenuItem( 
                    table_to_item( item.second.as<sol::table>() )
                )
            );
        }

        // if someone passes in some random value - WARNING!
        else {
            log_warn("Passed variable to table item that is not a valid type");
        }
    }
    
    MenuItem *menu_res = NULL;

    // create a menu and save the result
    if ( menu_type == STANDARD ) {
        menu_res = menu.ShowList();
    }
    else {
        menu_res = menu.ShowAlias();
    }    

    // debug print the return value
    log_debug("Script menu returned: name=\"%s\", idx=%d",
        menu_res->name.c_str(),
        menu_res->idx
    );

    return menu_res->name;
}


int inject_virtual_events( sol::environment &scripts_env, VirtualEvents &event ) {
    log_trace("Called function \"%s( env, VirtualEvent& )\"",
        __FUNCTION__
    );

    // function to cause the virtual event QUIT
    scripts_env.set_function(
        engine::func::scripts_api::virtual_events::QUIT,

        [&event]() {
            event = VirtualEvents::QUIT;
        }
    );

    return 0;
}


int inject_table_functions( Campaign &campaign ) {
    using namespace engine::func::scripts_api::table;

    // set copy_table to the copy function
    campaign.scripts_env.set_function(
        COPY,
        [&campaign](
            sol::table table
        ) {
            return CopyTable( campaign.lua, table );
        }
    );

    // set show_table to the ShowTable function
    campaign.scripts_env.set_function(
        SHOW,
        [](
            sol::table table
        ) {
            ShowTable( table );
        }
    );

    // set show_table to the ShowTable function
    campaign.scripts_env.set_function(
        MERGE,
        [&campaign](
            sol::table source,
            sol::table other,
            std::string strategy 
        ) {
            // a = add new, o = overwrite, d = deep, t = preserve types
            // aka TOAD or aodt I guess...

            // the int interpretation of the string
            int strategy_code = 0;

            // iterate the characters in the strategy and |= the corrent strategy argument based on the character
            for ( const auto &c : strategy ) {
                switch ( c ) {
                    case 't':
                        strategy_code |= CombineTable::PRESERVE_TYPES;
                        break;

                    case 'o':
                        strategy_code |= CombineTable::OVERWRITE_EXISTING;
                        break;

                    case 'a':
                        strategy_code |= CombineTable::ADD_NEW_PROPERTIES;
                        break;

                    case 'd':
                        strategy_code |= CombineTable::DEEP;
                        break;
                }
            }

            return CombineTable::ToNew( campaign.lua, source, other, strategy_code );
        }
    );

    return 0;
}

int inject_node_functions( Campaign &campaign ) {
    using namespace engine::func::scripts_api::node;

    campaign.scripts_env.set_function(
        GET_NODE,

        [&campaign](
            sol::table coords 
        ) {
            // the res
            sol::table res = campaign.lua.create_table_with(
                "type", "NONE",  // the type of the node
                "unique_name", "",  // the unique name data
                "exists", false,  // if the node requested exists
                "value", campaign.lua.create_table()  // the unique data table
            );

            // get the node
            node_t *found_node = 
                campaign.nodeManager.get_node(
                    parse_coordinate_table( coords )
                );
                
            // check if the node exists
            const bool node_exists = found_node != NULL;
            
            // if the node exist, set that it exists and pass in the unique data
            if ( node_exists ) {
                res["type"] = found_node->node_type;
                res["unique_name"] = found_node->unique_name;
                res["exists"] = true;
                res["value"] = found_node->unique_data;
            }
            
            // return the table
            return res;
        }
    );

    return 0;
}

int inject_api( Campaign &campaign ) {
    log_trace("Called function \"%s( env, VirtualEvent& )\"",
        __FUNCTION__
    );

    campaign.scripts_env.set_function(
        engine::func::scripts_api::BASIC_MENU,
    
        [](
            std::string name,
            std::string message,
            sol::table options
        ) {
            return menu_wrapper(name, message, options, STANDARD);
        }
    );

    campaign.scripts_env.set_function(
        engine::func::scripts_api::ALTERNATE_MENU,
    
        [](
            std::string name,
            std::string message,
            sol::table options
        ) {
            return menu_wrapper(name, message, options, TEXT);
        }
    );

    inject_virtual_events( campaign.scripts_env, campaign.event );
    inject_table_functions( campaign );
    inject_node_functions( campaign );

    return 0;
}
