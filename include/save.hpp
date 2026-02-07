#include <sol/sol.hpp>
#include <iostream>

namespace Write {
    int Var(FILE *fp, std::string var);

    int TypelessString(FILE *fp, std::string str);
    
    int String(FILE *fp, std::string str);

    int Int(FILE *fp, int number);

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
        std::vector<bool> bools;
        std::vector<char> chars;
        std::vector<struct TableReturn> tables;

        // the value is the final table
        sol::table value;
        // any error
        int error;
    }; 
    
    struct ReturnVal<std::string> Var(FILE *fp);

    struct ReturnVal<char> Type(FILE *fp);

    struct ReturnVal<std::string> TypelessString(FILE *fp);

    struct ReturnVal<std::string> String(FILE *fp);

    struct ReturnVal<int> Int(FILE *fp);

    struct ReturnVal<bool> Boolean(FILE *fp);

    struct ReturnVal<char> Nil(FILE *fp);

    struct TableReturn Table(FILE *fp, sol::state &lua);
}