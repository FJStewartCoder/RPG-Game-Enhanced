#ifndef INPUT_HPP
#define INPUT_HPP


#include <iostream>

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


#endif