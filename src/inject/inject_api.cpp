#include "inject_api.hpp"


extern "C" {
    #include "menus/menus.h"
}


// wrapper for menu functions for lua
std::string menu_wrapper(std::string name, std::string description, sol::table options, menu_type_t menu_type) {
    menu_t menu = create_menu((char *)name.c_str(), (char *)description.c_str());

    if ( !menu.is_valid ) { return "INVALID_MENU"; }

    // create a new vector and reserve a size of options length
    std::vector<std::string> options_store;
    options_store.reserve(options.size());

    for ( const auto &item : options ) {
        // push the string into the vector
        options_store.push_back(item.second.as<std::string>());

        // add all of the menu items using references to the vector
        add_menu_item(&menu, (char *)options_store.back().c_str(), false);
    }

    // show the menu
    menu_return_t res = show_menu(&menu, menu_type);

    // return the string result
    return res.str;
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
