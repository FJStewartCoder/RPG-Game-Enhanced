#include "save.hpp"
#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}

int Write::Var(FILE *fp, std::string var) {
    const int var_len = var.length();

    log_debug("Writing variable name: \"%s\" with length %d to file", var.c_str(), var_len);

    // write the string length then the string
    fwrite(&var_len, sizeof(int), 1, fp);
    fwrite(var.c_str(), sizeof(char), var_len, fp);

    return 0;
}

int Write::String(FILE *fp, std::string str) {
    const int str_len = str.length();

    log_debug("Writing string: \"%s\" with length %d to file", str.c_str(), str_len);

    // write the type character
    fputc(engine::save::STRING, fp);

    // write the string length then the string
    fwrite(&str_len, sizeof(int), 1, fp);
    fwrite(str.c_str(), sizeof(char), str_len, fp);

    return 0;
}

int Write::Int(FILE *fp, int number) {
    // write the type character
    fputc(engine::save::INT, fp);

    log_debug("Writing int %d to file", number);

    // write 1 piece of data of size int with address number to the file fp
    fwrite(&number, sizeof(int), 1, fp);

    return 0;
}

int Write::Boolean(FILE *fp, bool boolean) {
    // write the type character
    fputc(engine::save::BOOLEAN, fp);

    const char TRUE = 1;
    const char FALSE = 0;

    // if true, write 1
    if ( boolean ) {
        fwrite(&TRUE, sizeof(char), 1, fp);
    }
    // if false, write 0
    else {
        fwrite(&FALSE, sizeof(char), 1, fp);
    }
    
    return 0;
}

int Write::Nil(FILE *fp) {
    // write the type character
    fputc(engine::save::NIL, fp);

    return 0;
}

int Write::Table(FILE *fp, sol::table &table) {
    // write the type character
    fputc(engine::save::TABLE, fp);

    return 0;
}

// --------------------------------------------------------------------------------

int Read::Var(FILE *fp, std::string &dest) {
    int var_len;
    fread(&var_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Reading variable with length %d", var_len);

    // set the destination to blank
    dest = "";

    // the current character
    char c;

    for (int i = 0; i < var_len; i++) {
        // get the character
        c = fgetc(fp);

        // check if c is end of file, return
        if ( c == EOF ) { return 1; }

        // if not end of file, add the new character to the string
        dest += c;
    }

    return 0;
}

int Read::Type(FILE *fp, char &dest) {
    char c = fgetc(fp);

    if ( c == EOF ) { return 1; }

    dest = c;

    return 0;
}

int Read::String(FILE *fp, std::string &dest) {
    int str_len;
    fread(&str_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Reading string with length %d", str_len);

    // set the destination to blank
    dest = "";

    // the current character
    char c;

    for (int i = 0; i < str_len; i++) {
        // get the character
        c = fgetc(fp);

        // check if c is end of file, return
        if ( c == EOF ) { return 1; }

        // if not end of file, add the new character to the string
        dest += c;
    }

    return 0;
}

int Read::Int(FILE *fp, int &dest) {
    int data;

    fread(&data, sizeof(int), 1, fp);
    
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Read int %d", data);

    dest = data;

    return 0;
}

int Read::Boolean(FILE *fp, bool &dest) {
    char c = fgetc(fp);

    if ( c == 0 ) {
        dest = false;
    }
    else if ( c == 1 ) {
        dest = true;
    }
    else {
        return 1;
    }

    return 0;
}

int Read::Nil(FILE *fp) {
    return 0;
}

int Read::Table(FILE *fp, sol::table &dest) {
    return 0;
}