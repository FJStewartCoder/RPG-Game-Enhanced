#include <sol/sol.hpp>
#include <iostream>

namespace Write {
    int String(FILE *fp, std::string var, std::string str);

    int Int(FILE *fp, std::string var, int number);

    int Boolean(FILE *fp, std::string var, bool boolean);

    int Nil(FILE *fp, std::string var);

    int Table(FILE *fp, std::string var, sol::table &table);
}

namespace Read {
    int String(FILE *fp);

    int Int(FILE *fp);

    int Boolean(FILE *fp);

    int Nil(FILE *fp);

    int Table(FILE *fp);
}