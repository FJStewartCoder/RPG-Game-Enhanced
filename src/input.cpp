#include "input.hpp"

std::string ReadStdin() {
    std::string newStr;

    // currently read character
    char c;
    
    // read while character is not '\n' ( user pressed enter )
    while ( (c = getchar()) != '\n' ) {
        // add the read character to the string
        newStr += c;
    }

    // return the string
    return newStr;
}