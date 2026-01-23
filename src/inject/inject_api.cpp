#include "inject_api.hpp"

#include "menus/menus.hpp"


// wrapper for menu functions for lua
std::string menu_wrapper(std::string name, std::string description, sol::table options, menu_type_t menu_type) {
    Menu menu(name, description);

    for ( const auto &item : options ) {
        menu.AddItem( item.second.as<std::string>() );
    }

    if ( menu_type == STANDARD ) {
        return menu.ShowStandard();
    }
    else {
        return menu.ShowAlt();
    }    
}


int inject_api(sol::environment scripts_env) {
    scripts_env.set_function(
        engine::func::scripts_api::BASIC_MENU,
    
        [](std::string name, std::string description, sol::table options) {
            return menu_wrapper(name, description, options, STANDARD);
        }
    );

    scripts_env.set_function(
        engine::func::scripts_api::ALTERNATE_MENU,
    
        [](std::string name, std::string description, sol::table options) {
            return menu_wrapper(name, description, options, TEXT);
        }
    );

    return 0;
}
