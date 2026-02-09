#ifndef MENUS_HPP
#define MENUS_HPP

#include <iostream>
#include <vector>


typedef enum {
    STANDARD,
    TEXT
} menu_type_t;



class MenuItem {
    public:
        // the name of the menu
        std::string name;
        // the item description
        std::string description;

        // a shortened version of the name to make it easier to select menu items
        std::string alias;
        
        // determines whether or not the menu item is the default
        bool isDefault;
    
        MenuItem(
            std::string name,
            std::string description = "",
            bool isDefault = false
        );

        friend std::ostream &operator<<(std::ostream &os, const MenuItem &item);
};


// MENU DESIGNS

// BASIC
// -- MENU NAME --
// MENU DESCRIPTION
// 1 - ITEM
//     ITEM DESCRIPTION
// 2 - ITEM
//     ITEM DESCRIPTION
// 3 - ITEM
//
// MENU MESSAGE >>>

// ALTERNATIVE
// MENU MESSAGE: (I)TEM 1, (IT)EM 2, (ITE)M 3:

// ALTERNATIVE VERTICAL MODE
// -- MENU NAME --
// MENU DESCRIPTION
// (I)TEM 1 - ITEM DESCRIPTION
// (IT)EM 2 - ITEM DESCTIPTION
// (ITE)M 3 - ITEM DESCRIPTION
//
// MENU MESSAGE >>>


class Menu {
    private:
        // all of the menu's items
        std::vector<MenuItem> items;

        // the name of the menu
        std::string name;

        // an optional menu description
        std::string description;

        // the message that will be used for the input field
        std::string message;

        // the index in the items at which the default item is
        // -1 is no default
        int defaultIndex = -1;

        // function that sets the alias for each current item
        void SetAliases();

        // helper functions that are used in several menus
        void OutputHeading();
        void OutputMessage();

    public:
        Menu(
            std::string name = "",
            std::string description = "",
            std::string message = ""
        );

        void AddItem(MenuItem newItem);

        bool HasDefault();

        MenuItem *GetDefault();

        // show functions
        std::string ShowStandard();
        std::string ShowAlt();
};


#endif // MENUS_HPP