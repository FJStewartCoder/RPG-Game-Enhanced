#include "menus.hpp"

// needed for sorting
#include <algorithm>

// for fast lookups in alias
#include <unordered_set>
#include <cmath>

#include "input.hpp"


// GLOBAL CONSTANTS -------------------------------------------------------------------

 
const std::string EMPTY_STRING_ERROR = "Please enter an input";
const std::string NOT_INTEGER_ERROR = "Please enter a number";
const std::string OUT_OF_RANGE = "Please enter a number within the valid range";
const std::string ERROR_GENERAL = "Please try again";


// UTILITIES --------------------------------------------------------------------------

std::string ToLower(std::string str) {
    std::string res;
    const char aDiff = 'A' - 'a';

    for ( const auto &c : str ) {
        if ( c >= 'A' && c <= 'Z' ) {
            res += ( c - aDiff );
        }
        else {
            res += c;
        }
    }

    return res;
}

std::string ToUpper(std::string str) {
    std::string res;
    const char aDiff = 'A' - 'a';

    for ( const auto &c : str ) {
        if ( c >= 'a' && c <= 'z' ) {
            res += ( c + aDiff );
        }
        else {
            res += c;
        }
    }

    return res;
}

// MENU ITEMS -------------------------------------------------------------------------

MenuItem::MenuItem(
    std::string name,
    std::string description,
    bool isDefault 
) {
    // initialise all of the variables
    this->name = name;
    this->description = description;
    this->isDefault = isDefault;
}

std::ostream &operator<<(std::ostream &os, const MenuItem &item) {
    // if default, write as such (DE)fault
    if ( item.isDefault ) {
        os << "(" + ToUpper( item.alias ) + ")" + ToLower( item.name.substr(item.alias.size()) );
    }
    // else, write as such (de)fault
    else {
        os << "(" + ToLower( item.alias ) + ")" + ToLower( item.name.substr(item.alias.size()) );
    }
    
    return os;
}


// MENUS ------------------------------------------------------------------------------


Menu::Menu(
    std::string name,
    std::string description,
    std::string message
) {
    // initialise the menu
    this->name = name;
    this->description = description;
    this->message = message;
}

void Menu::OutputHeading() {
    using namespace std;

    // write the name if it exists else write a placeholder
    const bool hasName = !name.empty();
    if ( hasName ) {
        cout << "-- " << name << " --" << endl;
    }
    else {
        cout << "-- UNKNOWN MENU NAME --" << endl;
    }

    // optionally output the description
    const bool hasDesc = !description.empty();
    if ( hasDesc ) {
        cout << description << endl;
    }
}

void Menu::OutputMessage() {
    using namespace std;

    const bool hasMessage = !message.empty();
    if ( hasMessage ) {
        cout << message << ": ";
    }
    // show a default option
    else {
        cout << "Select an option: ";
    }
}

bool Menu::HasDefault() {
    return defaultIndex != -1;
}

MenuItem *Menu::GetDefault() {
    if ( !HasDefault() ) {
        return nullptr;
    }

    // return the address of where the item is in the vector
    return &items[defaultIndex];
}

void Menu::AddItem(MenuItem newItem) {
    // if the current item is default, perform default checks
    if ( newItem.isDefault ) {
        // if the menu already has a default, set isDefault to false
        // because we can't have a new default
        if ( HasDefault() ) {
            newItem.isDefault = false;
        }

        // if the menu doesn't already have a default, set the new default index
        // set it to the size of the items list because the size is last idx + 1 or next index
        else {
            defaultIndex = items.size();
        }
    }
    
    // add the new item to the list
    items.push_back(newItem);
}

std::string Menu::ShowStandard() {
    using namespace std;

    OutputHeading();

    const int numMenuItems = items.size();

    // print each option with formato:
    // n - name
    //     description

    for ( int i = 0; i < numMenuItems; i++ ) {
        const MenuItem &item = items[i];
        const int itemNum = i + 1;

        // output the item's name
        cout << itemNum << " - " << item.name << endl;

        const bool hasItemDesc = !item.description.empty();
        if ( hasItemDesc ) {
            // log10 of 10 = 1 because 10^1 = 10
            // log10 of 100 = 2 because 10^2 = 100
            // this tells us how many digits a number has - 1
            // use this to create a number of spaces
            int numSpacesForNumber = log10( itemNum ) + 1;

            // used to pad the description
            // 3 spaces for " - "
            // n spaces for the length of the number
            std::string spacing( 3 + numSpacesForNumber, ' ' );

            cout << spacing << item.description << endl;
        } 
    }

    // create a single blank line for nicer formatting
    cout << endl;

    OutputMessage();

    // the chosen value by the user
    int intChoice;

    while ( true ) {
        string choice = ReadStdin();

        if ( choice.empty() ) {
            cout << EMPTY_STRING_ERROR << endl;
            continue;
        }

        // scan the buffer into int
        const bool failed = sscanf(choice.c_str(), "%d", &intChoice) == 0;

        if ( failed ) {
            cout << NOT_INTEGER_ERROR << endl;
            continue;
        }
        
        const bool inRange = intChoice >= 1 && intChoice <= numMenuItems;

        if ( !inRange ) {
            cout << OUT_OF_RANGE << endl;
            continue;
        }

        break;
    }

    // return the string that represents the item selected
    return items.at(intChoice - 1).name;
}

void Menu::SetAliases() {
    // vector of pointers to items
    // needs to be pointers to not impact the actual order and is faster than big memory copies
    std::vector<MenuItem *> aliasVec;

    for ( auto &item : items ) {
        aliasVec.push_back(&item);
    }

    // sort the items in ascending order of name length
    std::stable_sort(aliasVec.begin(), aliasVec.end(), [](MenuItem *a, MenuItem *b) {
        return a->name.length() < b->name.length();
    });

    std::unordered_set<std::string> aliasSet;

    // create all of the aliases and set them in the order of aliasVec
    for ( auto &item : aliasVec ) {
        // create a string buffer
        std::string currentAlias;

        // iterate each character
        for ( char &c : item->name ) {
            // add that character to the end of the string
            currentAlias += c;

            const bool aliasExists = aliasSet.find(currentAlias) != aliasSet.end();

            // if the alias doesn't already exist, set that alias and stop this search
            if ( !aliasExists ) {
                // add the alias if not existing
                aliasSet.insert(currentAlias);

                item->alias = currentAlias;
                break;
            }
        }
    }
}

std::string Menu::ShowAlt() {
    // reduces unnecessary std::
    using namespace std;

    // create and set all of the aliases
    SetAliases();

    // show the menu's message
    OutputMessage();

    const int numItems = items.size();

    if ( numItems < 1 ) {
        cout << endl;
        return "ERROR";
    }

    // print out all of the menu items
    for ( int i = 0; i < numItems - 1; i++ ) {
        cout << items[i] << ", ";
    }

    // print the last item with different formatting
    cout << items.back() << ": ";

    std::string chosenString;
    bool selectionMade = false;

    // get the valid user input
    while ( !selectionMade ) {
        string choice = ReadStdin();

        if ( choice.empty() ) {
            // get the default item
            MenuItem *defaultItem = GetDefault();

            // if there is a default item, set the chosen string and break out of loop
            if ( defaultItem != nullptr ) {
                chosenString = defaultItem->name;
                break;
            }
        }

        for ( const auto& item : items ) {
            if ( ToLower( choice ) == ToLower( item.alias ) ) {
                chosenString = item.name;
                selectionMade = true;
                break;
            }
        }

        if ( !selectionMade ) {
            cout << ERROR_GENERAL << endl;
        }
    }

    // return the string that represents the item selected
    return chosenString;
}