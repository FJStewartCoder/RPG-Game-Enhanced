#include <sol/sol.hpp>
#include <iostream>

namespace Write {
    int Var(FILE *fp, std::string var);

    int String(FILE *fp, std::string str);

    int Int(FILE *fp, int number);

    int Boolean(FILE *fp, bool boolean);

    int Nil(FILE *fp);

    int Table(FILE *fp, sol::table &table);
}

namespace Read {
    int Var(FILE *fp, std::string &dest);

    int Type(FILE *fp, char &dest);

    int String(FILE *fp, std::string &dest);

    int Int(FILE *fp, int &dest);

    int Boolean(FILE *fp, bool &dest);

    int Nil(FILE *fp);

    int Table(FILE *fp, sol::table &dest);
}