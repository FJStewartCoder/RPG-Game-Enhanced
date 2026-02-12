#include "save.hpp"
#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}

int Write::Var(FILE *fp, std::string var) {
    log_trace("Called function \"%s( FILE, %s )\"",
        __FUNCTION__,
        var.c_str()
    );

    const int var_len = var.length();

    log_debug("Writing variable name: \"%s\" with length %d to file", var.c_str(), var_len);

    // write the string length then the string
    fwrite(&var_len, sizeof(int), 1, fp);
    fwrite(var.c_str(), sizeof(char), var_len, fp);

    return 0;
}

int Write::TypelessString(FILE *fp, std::string str) {
    log_trace("Called function \"%s( FILE, %s )\"",
        __FUNCTION__,
        str.c_str()
    );

    const int str_len = str.length();

    log_debug("Writing typeless string: \"%s\" with length %d to file", str.c_str(), str_len);

    // write the string length then the string
    fwrite(&str_len, sizeof(int), 1, fp);
    fwrite(str.c_str(), sizeof(char), str_len, fp);

    return 0;
}

int Write::String(FILE *fp, std::string str) {
    log_trace("Called function \"%s( FILE, %s )\"",
        __FUNCTION__,
        str.c_str()
    );

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
    log_trace("Called function \"%s( FILE, %d )\"",
        __FUNCTION__,
        number
    );

    // write the type character
    fputc(engine::save::INT, fp);

    log_debug("Writing int %d to file", number);

    // write 1 piece of data of size int with address number to the file fp
    fwrite(&number, sizeof(int), 1, fp);

    return 0;
}

int Write::Float(FILE *fp, float number) {
    log_trace("Called function \"%s( FILE, %lf )\"",
        __FUNCTION__,
        number
    );

    // write the type character
    fputc(engine::save::FLOAT, fp);

    log_debug("Writing float %lf to file", number);

    // write 1 piece of data of size double with address number to the file fp
    fwrite(&number, sizeof(float), 1, fp);

    return 0;
}

int Write::Boolean(FILE *fp, bool boolean) {
    log_trace("Called function \"%s( FILE, %d )\"",
        __FUNCTION__,
        boolean
    );

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
    log_trace("Called function \"%s( FILE )\"", __FUNCTION__);

    // write the type character
    fputc(engine::save::NIL, fp);

    return 0;
}

int Write::Table(FILE *fp, sol::table table) {
    log_trace("Called function \"%s( FILE, table )\"", __FUNCTION__);

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
                if ( data.is<int>() ) {
                    Write::Int(fp, data.as<int>());
                }
                else {
                    Write::Float(fp, data.as<float>());
                }
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
                log_warn("The data type \"%s\" (%d) is not supported.",
                    sol::type_name(NULL, data.get_type()).c_str(),  // get the type name for the type (lua state is NULL)
                    data.get_type()
                );
                Write::Nil(fp);
                break;
        }
    }

    return 0;
}

// --------------------------------------------------------------------------------

int Read::Var(FILE *fp, std::string &dest) {
    log_trace("Called function \"%s( FILE, str& )\"", __FUNCTION__);

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
    log_trace("Called function \"%s( FILE, char& )\"", __FUNCTION__);

    char c = fgetc(fp);

    if ( c == EOF ) { return 1; }

    dest = c;

    return 0;
}

int Read::TypelessString(FILE *fp, std::string &dest) {
    log_trace("Called function \"%s( FILE, str& )\"", __FUNCTION__);

    int str_len;
    fread(&str_len, sizeof(int), 1, fp);

    // if there is an error or end of file, return 1
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Reading typeless string with length %d", str_len);

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

int Read::String(FILE *fp, std::string &dest) {
    log_trace("Called function \"%s( FILE, str& )\"", __FUNCTION__);

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
    log_trace("Called function \"%s( FILE, int& )\"", __FUNCTION__);

    int data;

    fread(&data, sizeof(int), 1, fp);
    
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Read int %d", data);

    dest = data;

    return 0;
}

int Read::Float(FILE *fp, float &dest) {
    log_trace("Called function \"%s( FILE, float& )\"", __FUNCTION__);

    float data;

    fread(&data, sizeof(float), 1, fp);
    
    if ( feof(fp) || ferror(fp) ) { return 1; }

    log_debug("Read float %lf", data);

    dest = data;

    return 0;
}

int Read::Boolean(FILE *fp, bool &dest) {
    log_trace("Called function \"%s( FILE, bool& )\"", __FUNCTION__);

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
    log_trace("Called function \"%s( FILE )\"", __FUNCTION__);

    return 0;
}

struct Read::TableReturn Read::Table(FILE *fp, sol::state &lua) {
    log_trace("Called function \"%s( FILE, state )\"", __FUNCTION__);

    Read::TableReturn res;

    // initialiase some return variables
    res.value = lua.create_table();
    res.error = 0;

    // create a reference to the res' value
    sol::table &dest = res.value;

    int table_length;

    fread(&table_length, sizeof(int), 1, fp);

    log_debug("Reading table of length %d", table_length);

    if ( feof(fp) || ferror(fp) ) { 
        res.error = 1;
        return res;
    }

    // iterate table length items
    for ( int i = 0; i < table_length; i++ ) {
        // add one blank item to the back of the array
        res.vars.push_back("");
        
        // write to the new heap memory
        if ( Read::Var(fp, res.vars.back()) ) {
            res.error = 1;
            return res;
        };

        // reference to the current variable
        std::string &var = res.vars.back();
        
        log_debug("IN TABLE - Var recieved is %s. Total vars = %d", var.c_str(), res.vars.size());
        for ( const auto v : res.vars ) {
            log_debug("VAR: %s", v.c_str());
        }

        // return values for a possible table or bool
        bool bool_var;
        struct TableReturn table_var;

        char type;
        
        if ( Read::Type(fp, type) ) {
            res.error = 1;
            return res;
        }

        int error = 0;

        switch ( type ) {
            case engine::save::STRING:
                // push a blank value to the heap
                res.strs.push_back("");
                error = Read::String(fp, res.strs.back());

                log_debug("Setting table data at \"%s\" to \"%s\"", var.c_str(), res.strs.back().c_str());

                dest[var] = res.strs.back();
                break;

            case engine::save::INT:
                // add the value to the heap
                res.ints.push_back(0);
                error = Read::Int(fp, res.ints.back());

                log_debug("Setting table data at \"%s\" to %d", var.c_str(), res.ints.back());
                
                dest[var] = res.ints.back();
                break;
            
            case engine::save::FLOAT:
                // add the value to the heap
                res.floats.push_back(0);
                error = Read::Float(fp, res.floats.back());

                log_debug("Setting table data at \"%s\" to %lf", var.c_str(), res.floats.back());
                
                dest[var] = res.floats.back();
                break;
            
            case engine::save::BOOLEAN:
                // push a blank value and load the new value into this location
                error = Read::Boolean(fp, bool_var);
                res.bools.push_back(bool_var);

                log_debug("Setting table data at \"%s\" to %i", var.c_str(), res.bools.back());

                dest[var] = res.bools.back();
                break;
            
            case engine::save::NIL:
                error = Read::Nil(fp);

                log_debug("Setting table data at \"%s\" to nil", var.c_str());

                dest[var] = sol::nil;
                break;
            
            case engine::save::TABLE:
                table_var = Read::Table(fp, lua);
                error = table_var.error;

                // add the value to the heap
                res.tables.push_back(table_var);

                log_debug("Setting table data at \"%s\" to table", var.c_str());

                dest[var] = res.tables.back().value;
                break;
            
            default:
                log_warn("Data attempting to be read is of a type not implemented");
        }

        if ( error != 0 ) {
            res.error = 1;
            return res;
        }
    }

    return res;
}