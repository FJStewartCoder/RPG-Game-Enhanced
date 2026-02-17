#ifndef SAVE_HPP
#define SAVE_HPP

#include <sol/sol.hpp>
#include <iostream>

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

#endif  // SAVE_HPP