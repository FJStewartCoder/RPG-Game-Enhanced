#ifndef SAVE_HPP
#define SAVE_HPP

#include <sol/sol.hpp>
#include <iostream>

#include "lua_engine_constants.hpp"


namespace Write {
    int Var(FILE *fp, std::string var);

    int TypelessString(FILE *fp, std::string str);
    
    int String(FILE *fp, std::string str);

    int Int(FILE *fp, int number);

    int Float(FILE *fp, float number);

    int Boolean(FILE *fp, bool boolean);

    int Nil(FILE *fp);

    int Table(FILE *fp, sol::table table);
}

namespace Read {
    template <typename T>
    struct ReturnVal {
        int error;
        T value;
    };

    // table return specifically for reading tables
    // values need heap memory allocated due to wierd references required by sol
    struct TableReturn {
        // allocated memory for all values read from the table
        std::vector<std::string> vars;
        std::vector<std::string> strs;
        std::vector<int> ints;
        std::vector<float> floats;
        std::vector<bool> bools;
        std::vector<char> chars;
        std::vector<struct TableReturn> tables;

        // the value is the final table
        sol::table value;
        // any error
        int error;
    }; 
    
    int Var(FILE *fp, std::string &dest);

    int Type(FILE *fp, char &dest);

    int TypelessString(FILE *fp, std::string &dest);

    int String(FILE *fp, std::string &dest);

    int Int(FILE *fp, int &dest);

    int Float(FILE *fp, float &dest);

    int Boolean(FILE *fp, bool &dest);

    int Nil(FILE *fp);

    struct TableReturn Table(FILE *fp, sol::state &lua);
}

namespace WriteV2 {
    void String( FILE *fp, sol::object &obj );
    void Int( FILE *fp, sol::object &obj );
    void Float( FILE *fp, sol::object &obj );
    void Boolean( FILE *fp, sol::object &obj );
    void Table( FILE *fp, sol::object &obj );

    int Write( FILE *fp, sol::object &obj, bool isVar = false );
}

namespace ReadV2 {
    struct Item {
        // the type and some other data
        char type = engine::save::NIL;
        bool isVar = false;
        bool valid = true;

        // the actual value as a struct 
        struct {
            // since this has a wierd initialiser, it can not be in the union
            // jobs amiright
            std::string strVal;

            union {
                int intVal;
                float floatVal;
                bool boolVal;
                char charVal;
            };
        } value;
    };

    struct TableReturn {
        // allocated memory for all values read from the table
        std::vector<struct Item> items;
        std::vector<struct TableReturn> tables;

        // the value is the final table
        sol::table value;
        // any error
        int error;
    }; 

    struct ReadV2::Item Read( FILE *fp );

    struct ReadV2::TableReturn Table(FILE *fp, sol::state &lua);
}

#endif  // SAVE_HPP