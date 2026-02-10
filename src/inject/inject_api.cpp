#include "inject_api.hpp"

#include "menus/menus.hpp"

extern "C" {
    #include "log/log.h"
}


MenuItem table_to_item( const sol::table &table ) {
    for ( const auto &item : table ) {
        log_debug("This item has type: %d", item.second.get_type());
    }

    // try to get both the name and description
    // if neither are available
    const std::string name = table[1].get_or<std::string>("");
    const std::string description = table[2].get_or<std::string>("");

    return MenuItem( name, description );
}

// wrapper for menu functions for lua
std::string menu_wrapper(std::string name, std::string message, sol::table options, menu_type_t menu_type) {
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

    if ( menu_type == STANDARD ) {
        return menu.ShowStandard();
    }
    else {
        return menu.ShowAlt();
    }    
}


int inject_api(sol::environment scripts_env, VirtualEvents &event) {
    scripts_env.set_function(
        engine::func::scripts_api::BASIC_MENU,
    
        [](
            std::string name,
            std::string message,
            sol::table options
        ) {
            return menu_wrapper(name, message, options, STANDARD);
        }
    );

    scripts_env.set_function(
        engine::func::scripts_api::ALTERNATE_MENU,
    
        [](
            std::string name,
            std::string message,
            sol::table options
        ) {
            return menu_wrapper(name, message, options, TEXT);
        }
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
