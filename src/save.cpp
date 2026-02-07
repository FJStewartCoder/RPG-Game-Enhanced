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

int Write::TypelessString(FILE *fp, std::string str) {
    const int str_len = str.length();

    log_debug("Writing typeless string: \"%s\" with length %d to file", str.c_str(), str_len);

    // write the string length then the string
    fwrite(&str_len, sizeof(int), 1, fp);
    fwrite(str.c_str(), sizeof(char), str_len, fp);

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

int Write::Table(FILE *fp, sol::table table) {
    // write the type character
    fputc(engine::save::TABLE, fp);

    // if table is invalid, return error
    if ( !table.valid() ) { return 1; }

    // write the table length to the file
    int table_length = 0;
    for ( const auto &item : table ) { table_length++; }
    fwrite(&table_length, sizeof(int), 1, fp);

    log_debug("Writing table to file with length %d", table_length);

    for ( const auto &item : table ) {
        // create variables to store the variable and the data
        const auto var = item.first.as<std::string>();
        const auto data = item.second;

        Write::Var(fp, var);

        switch (data.get_type()) {
            case sol::type::boolean:
                Write::Boolean(fp, data.as<bool>());
                break;
            
            case sol::type::number:
                Write::Int(fp, data.as<int>());
                break;
            
            case sol::type::string:
                Write::String(fp, data.as<std::string>());
                break;

            case sol::type::nil:
                Write::Nil(fp);
                break;

            case sol::type::table:
                Write::Table(fp, data.as<sol::table>());
                break;
            
            default:
                log_warn("The data type is not supported.");
                Write::Nil(fp);
                break;
        }
    }

    return 0;
}

// --------------------------------------------------------------------------------

struct Read::ReturnVal<std::string> Read::Var(FILE *fp) {
    struct Read::ReturnVal<std::string> res = {
        0, "" 
    };

    int var_len;
    fread(&var_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) {
        res.error = 1;
        return res;
    }

    log_debug("Reading variable with length %d", var_len);

    // the current character
    char c;

    for (int i = 0; i < var_len; i++) {
        // get the character
        c = fgetc(fp);

        // check if c is end of file, return
        if ( c == EOF ) {
            res.error = 1;
            return res;
        }

        // if not end of file, add the new character to the string
        res.value += c;
    }

    return res;
}

struct Read::ReturnVal<char> Read::Type(FILE *fp) {
    struct Read::ReturnVal<char> res = {
        0, ' ' 
    };

    char c = fgetc(fp);

    if ( c == EOF ) {
        res.error = 1;
        return res;
    }

    res.value = c;

    return res;
}

struct Read::ReturnVal<std::string> Read::TypelessString(FILE *fp) {
    struct Read::ReturnVal<std::string> res = {
        0, "" 
    };

    int str_len;
    fread(&str_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) {
        res.error = 1;
        return res;
    }

    log_debug("Reading typeless string with length %d", str_len);

    // the current character
    char c;

    for (int i = 0; i < str_len; i++) {
        // get the character
        c = fgetc(fp);

        // check if c is end of file, return
        if ( c == EOF ) {
            res.error = 1;
            return res;
        }

        // if not end of file, add the new character to the string
        res.value += c;
    }

    return res;
}

struct Read::ReturnVal<std::string> Read::String(FILE *fp) {
    struct Read::ReturnVal<std::string> res = {
        0, "" 
    };

    int str_len;
    fread(&str_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) {
        res.error = 1;
        return res;
    }

    log_debug("Reading string with length %d", str_len);

    // the current character
    char c;

    for (int i = 0; i < str_len; i++) {
        // get the character
        c = fgetc(fp);

        // check if c is end of file, return
        if ( c == EOF ) {
            res.error = 1;
            return res;
        }

        // if not end of file, add the new character to the string
        res.value += c;
    }

    return res;
}

struct Read::ReturnVal<int> Read::Int(FILE *fp) {
    struct Read::ReturnVal<int> res = {
        0, 0
    };

    int data;

    fread(&data, sizeof(int), 1, fp);
    
    if ( feof(fp) || ferror(fp) ) {
        res.error = 1;
        return res;
    }

    log_debug("Read int %d", data);

    res.value = data;

    return res;
}

struct Read::ReturnVal<bool> Read::Boolean(FILE *fp) {
    struct Read::ReturnVal<bool> res = {
        0, false 
    };
    
    char c = fgetc(fp);

    if ( c == 0 ) {
        res.value = false;
    }
    else if ( c == 1 ) {
        res.value = true;
    }
    else {
        res.error = 1;
    }

    return res;
}

struct Read::ReturnVal<sol::type> Read::Nil(FILE *fp) {
    struct Read::ReturnVal<sol::type> res = {
        0, sol::nil
    };

    return res;
}

struct Read::ReturnVal<sol::table> Read::Table(FILE *fp) {
    struct Read::ReturnVal<sol::table> res = {
        0, sol::table()
    };

    // create a reference to the res' value
    sol::table &dest = res.value;

    dest["SomeOtherRandomThing"] = "HELLOORSOMETHING";

    int table_length;

    fread(&table_length, sizeof(int), 1, fp);

    log_debug("Reading table of length %d", table_length);

    if ( feof(fp) || ferror(fp) ) { return 1; }

    // iterate table length items
    while ( !feof(fp) ) {
        std::string var;

        if ( Read::Var(fp, var) ) {
            break;
        };

        dest[var] = "HELLOORSOMETHING";
        std::cout << "TESTING " << dest[var].get<std::string>() << std::endl;

        std::string str_var = "";
        int int_var = 0;
        char char_var = ' ';
        bool bool_var = false;
        // create a new table in the table
        sol::table table_var = dest.create();

        if ( Read::Type(fp, char_var) ) {
            break;
        }

        int res = 0;

        switch (char_var) {
            case engine::save::STRING:
                res = Read::String(fp, str_var);
                log_debug("Setting table data at \"%s\" to \"%s\"", var.c_str(), str_var.c_str());

                dest[var] = str_var;
                dest.set(var, str_var);
                break;

            case engine::save::INT:
                res = Read::Int(fp, int_var);
                log_debug("Setting table data at \"%s\" to %d", var.c_str(), int_var);
                
                dest[var] = int_var;
                dest.set(var, int_var);
                break;
            
            case engine::save::BOOLEAN:
                res = Read::Boolean(fp, bool_var);
                log_debug("Setting table data at \"%s\" to %i", var.c_str(), bool_var);

                dest[var] = bool_var;
                dest.set(var, bool_var);
                break;
            
            case engine::save::NIL:
                res = Read::Nil(fp);
                log_debug("Setting table data at \"%s\" to nil", var.c_str());

                dest[var] = sol::nil;
                dest.set(var, sol::nil);
                break;
            
            case engine::save::TABLE:
                res = Read::Table(fp, table_var);
                log_debug("Setting table data at \"%s\" to table", var.c_str());

                break;
            
            default:
                log_warn("Data attempting to be read is of a type not implemented");
        }

        if ( res ) {
            break;
        }
    }

    return res;
}