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
    
    struct ReturnVal<std::string> Var(FILE *fp);

    struct ReturnVal<char> Type(FILE *fp);

    struct ReturnVal<std::string> TypelessString(FILE *fp);

    struct ReturnVal<std::string> String(FILE *fp);

    struct ReturnVal<int> Int(FILE *fp);

    struct ReturnVal<bool> Boolean(FILE *fp);

    struct ReturnVal<char> Nil(FILE *fp);

    struct ReturnVal<sol::table> Table(FILE *fp, sol::state &lua);
}