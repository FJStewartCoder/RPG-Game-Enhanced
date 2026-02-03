#include "save.hpp"

int Write::String(FILE *fp, std::string var, std::string str) {
    const int var_len = var.length();
    const int str_len = str.length();

    fwrite(&var_len, sizeof(int), 1, fp);
    fwrite(var.c_str(), sizeof(char), var_len, fp);

    fwrite(&str_len, sizeof(int), 1, fp);
    fwrite(str.c_str(), sizeof(char), str_len, fp);

    return 0;
}

int Write::Int(FILE *fp, std::string var, int number) {
    return 0;
}

int Write::Boolean(FILE *fp, std::string var, bool boolean) {
    return 0;
}

int Write::Nil(FILE *fp, std::string var) {
    return 0;
}

int Write::Table(FILE *fp, std::string var, sol::table &table) {
    return 0;
}

// --------------------------------------------------------------------------------

int Read::String(FILE *fp) {
    return 0;
}

int Read::Int(FILE *fp) {
    return 0;
}

int Read::Boolean(FILE *fp) {
    return 0;
}

int Read::Nil(FILE *fp) {
    return 0;
}

int Read::Table(FILE *fp) {
    return 0;
}