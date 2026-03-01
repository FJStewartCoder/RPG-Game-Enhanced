#include "save.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "table.hpp"


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
        log_debug("Writing boolean TRUE to file");
        fwrite(&TRUE, sizeof(char), 1, fp);
    }
    // if false, write 0
    else {
        log_debug("Writing boolean FALSE to file");
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
        if ( c == EOF ) { 
            log_error("File ended before end of string");
            return 1;
        }

        // if not end of file, add the new character to the string
        dest += c;
    }

    log_debug("Recieved var=\"%s\"", dest.c_str());

    return 0;
}

int Read::Type(FILE *fp, char &dest) {
    log_trace("Called function \"%s( FILE, char& )\"", __FUNCTION__);

    char c = fgetc(fp);

    if ( c == EOF ) { return 1; }

    log_debug("Read type: %c", c);

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
        if ( c == EOF ) { 
            log_error("File ended before end of string");
            return 1;
        }

        // if not end of file, add the new character to the string
        dest += c;
    }

    log_debug("Recieved typeless string=\"%s\"", dest.c_str());

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
        if ( c == EOF ) { 
            log_error("File ended before end of string");
            return 1;
        }

        // if not end of file, add the new character to the string
        dest += c;
    }

    log_debug("Recieved string=\"%s\"", dest.c_str());

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
        log_error("Recieved a value that was not boolean");
        return 1;
    }

    log_debug("Read boolean: %d", dest);

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

                log_debug("Setting table data at \"%s\" to %d", var.c_str(), res.bools.back());

                // needs to be cast to bool otherwise it is considered "userdata"
                dest[var] = (bool)res.bools.back();
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
                log_error("Data attempting to be read is of a type not implemented");
                error = 1;
        }

        if ( error != 0 ) {
            log_error("An error has occurred");

            res.error = 1;
            return res;
        }
    }

    return res;
}


char TypeCharacter( sol::object &obj ) {
    log_trace("Called function \"%s( object )\"", __FUNCTION__);

    const sol::type type = obj.get_type();

    char res = engine::save::NIL;

    switch ( type ) {
        case sol::type::string:
            res = engine::save::STRING;
            break;

        case sol::type::number:
            res = engine::save::FLOAT;

            if ( obj.is<int>() ) {
                res = engine::save::INT;
            }

            break;
        
        case sol::type::boolean:
            res = engine::save::BOOLEAN;
            break;

        case sol::type::table:
            res = engine::save::TABLE;
            break;
        
        case sol::type::nil:
            res = engine::save::NIL;
            break;
        
        default:
            log_error("This object type does not a corresponding character");
            break;
    }

    return res;
}

void WriteV2::String( FILE *fp, sol::object &obj ) {
    log_trace("Called function \"%s( FILE, object )\"", __FUNCTION__);

    std::string val = obj.as<std::string>();
    int str_len = val.length();

    // write the string length then the string
    fwrite(&str_len, sizeof(int), 1, fp);
    fwrite(val.c_str(), sizeof(char), str_len, fp);
}

void WriteV2::Int( FILE *fp, sol::object &obj ) {
    log_trace("Called function \"%s( FILE, object )\"", __FUNCTION__);

    int val = obj.as<int>();
    fwrite( &val, sizeof(int), 1, fp );
}

void WriteV2::Float( FILE *fp, sol::object &obj ) {
    log_trace("Called function \"%s( FILE, object )\"", __FUNCTION__);

    float val = obj.as<float>();
    fwrite( &val, sizeof(float), 1, fp );
}

void WriteV2::Boolean( FILE *fp, sol::object &obj ) {
    log_trace("Called function \"%s( FILE, object )\"", __FUNCTION__);

    bool val = obj.as<bool>();

    const char TRUE = 1;
    const char FALSE = 0;

    if ( val ) { fwrite( &TRUE, sizeof(char), 1, fp ); }
    else { fwrite( &FALSE, sizeof(char), 1, fp ); }
}

void WriteV2::Table( FILE *fp, sol::object &obj ) {
    log_trace("Called function \"%s( FILE, object )\"", __FUNCTION__);

    const sol::table table = obj;

    // get the length of the table and write it
    int table_length = 0;
    for ( const auto &item : table ) { table_length++; }
    fwrite(&table_length, sizeof(int), 1, fp);

    for ( const auto item : table ) {
        sol::object key = item.first;
        sol::object val = item.second;

        // write the var then the key, var is as a var
        WriteV2::Write( fp, key, true );
        WriteV2::Write( fp, val, false );
    }
}

int WriteV2::Write( FILE *fp, sol::object &obj, bool isVar ) {
    log_trace(
        "Called function \"%s( FILE, object, %d )\"", 
        __FUNCTION__,
        isVar
    );

    // write the var character to the file
    if ( isVar ) { fputc('v', fp); }

    // get the character that corresponds to the type
    char typeChar = TypeCharacter( obj );

    // write the character for the type
    fputc(typeChar, fp);

    // write the data to the file
    switch ( typeChar ) {
        case engine::save::STRING:
            WriteV2::String( fp, obj );
            break;

        case engine::save::INT:
            WriteV2::Int( fp, obj );
            break;

        case engine::save::FLOAT:
            WriteV2::Float( fp, obj );
            break;
        
        case engine::save::BOOLEAN:
            WriteV2::Boolean( fp, obj );
            break;

        case engine::save::TABLE:
            WriteV2::Table( fp, obj );
            break;
        
        case engine::save::NIL:
            // don't do anything
            break;
    }

    return 0;
}

struct ReadV2::Item ReadV2::Read( FILE *fp ) {
    struct ReadV2::Item res;

    // get the type character and check if the data is valid
    char typeChar = fgetc( fp );

    if ( feof(fp) || ferror(fp) ) {
        log_error("File has unexpectedly errored");
        res.valid = false;

        return res;
    }

    // set the type to variable and get another character for actual type
    if ( typeChar == engine::save::VARIABLE ) {
        res.isVar = true;
        typeChar = fgetc( fp );
    }

    // set the type
    res.type = typeChar;

    int readRes = 0;

    // read the data as specified by the type character
    switch ( typeChar ) {
        case engine::save::STRING:
            readRes = Read::String( fp, res.value.strVal );
            break;

        case engine::save::INT:
            readRes = Read::Int( fp, res.value.intVal );
            break;

        case engine::save::FLOAT:
            readRes = Read::Float( fp, res.value.floatVal );
            break;
        
        case engine::save::BOOLEAN:
            readRes = Read::Boolean( fp, res.value.boolVal );
            break;

        case engine::save::TABLE:
            log_warn("This function can not read tables. Please call ReadV2::Table after this.");
            break;
        
        case engine::save::NIL:
            // don't do anything
            break;

        default:
            // not a type character we recognise
            res.valid = false;
            break;
    }

    // check if the read value is valid
    const bool readValid = readRes == 0;

    // tell the programmer that the item is valid
    res.valid = readValid;

    // return the item
    return res;
}

// WHEN WRITING THIS, ENSURE THAT IF SOME OPTION IS ENABLED, DON'T CARE IF THE VAR IS ACTUALLY A VAR TYPE BECAUSE IT ISN'T IN OLDER VERSIONS
// MAYBE JUST HAVE A WARNING
struct ReadV2::TableReturn ReadV2::Table(FILE *fp, sol::state &lua) {
    log_trace("Called function \"%s( FILE, state )\"", __FUNCTION__);

    ReadV2::TableReturn res;

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
        ReadV2::Item key = ReadV2::Read( fp );

        // if the key is invalid, return error
        if ( !key.valid ) {
            res.error = 1;
            return res;
        }

        // warning if the key is not a variable
        if ( !key.isVar ) {
            log_warn("Read key is not a variable");
        }

        // push the key to the vector
        res.items.push_back( key );

        // reference to the item
        auto valueRef = dest[ "temp" ];

        switch ( key.type ) {
            case engine::save::STRING:
                dest[ res.items.back().value.strVal ] = "TEMP";
                valueRef = dest[ res.items.back().value.strVal ];
                break;

            case engine::save::INT:
                dest[ res.items.back().value.intVal ] = "TEMP";
                valueRef = dest[ res.items.back().value.intVal ];
                break;
            
            case engine::save::FLOAT:
                dest[ res.items.back().value.floatVal ] = "TEMP";
                valueRef = dest[ res.items.back().value.floatVal ];
                break;
            
            case engine::save::BOOLEAN:
                dest[ res.items.back().value.boolVal ] = "TEMP";
                valueRef = dest[ res.items.back().value.boolVal ];
                break;

            // other types are not available as keys
            default:
                res.error = 1;
                break;
        }

        // check that there was not an error when loading the key
        if ( res.error ) {
            log_error("An error occured when loading the key");
            return res;
        }

        // read the value
        ReadV2::Item value = ReadV2::Read( fp );

        // if the value is a variable ( it shouldn't be if it is a value )
        // or the value is invalid, return error
        if ( value.isVar || !value.valid ) {
            log_error("An error occured when loading the value");

            res.error = 1;
            return res;
        }

        // handle tables differently to other types
        if ( value.type == engine::save::TABLE ) {
            ReadV2::TableReturn readTable = ReadV2::Table( fp, lua );

            if ( readTable.error ) {
                log_error("An error occured when reading inner table");

                res.error = 1;
                return res;
            }

            // add the value and assign it
            res.tables.push_back( readTable );
            valueRef = res.tables.back();

            log_trace("Successfully read inner table");

            // loop back to the start
            continue;
        }

        // add the item to the back of the list of items
        res.items.push_back( value );

        switch ( value.type ) {
            case engine::save::STRING:
                valueRef = res.items.back().value.strVal;
                break;

            case engine::save::INT:
                valueRef = res.items.back().value.intVal;
                break;
            
            case engine::save::FLOAT:
                valueRef = res.items.back().value.floatVal;
                break;
            
            case engine::save::BOOLEAN:
                valueRef = res.items.back().value.boolVal;
                break;
            
            case engine::save::NIL:
                valueRef = sol::nil;
                break;
            
            default:
                log_error("Data attempting to be read is of a type not implemented");
                res.error = 1;
        }

        log_debug(
            "Read data %s = %s",
            "VAR",
            ObjectToString( valueRef ).c_str()
        );

        if ( res.error ) {
            log_error("An error has occurred");
            return res;
        }
    }

    return res;
}