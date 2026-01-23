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
        // name and alias 
        std::string name;
        std::string alias;

        bool isDefault;
    
        MenuItem(std::string name = "", bool isDefault = false);

        std::string GetAliasedName();
};

class Menu {
    private:
        std::vector<MenuItem> items;

        std::string name;
        std::string description;

        // the index in the items at which the default item is
        // -1 is no default
        int defaultIndex = -1;

        // function that sets the alias for each current item
        void SetAliases();

    public:
        Menu(std::string name = "", std::string description = "");

        void AddItem(std::string name = "", bool isDefault = false);

        bool HasDefault();

        MenuItem *GetDefault();

        // show functions
        std::string ShowStandard();
        std::string ShowAlt();
};


#endif // MENUS_HPP