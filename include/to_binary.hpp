#ifndef INT_TO_BINARY_HPP
#define INT_TO_BINARY_HPP

#include <string>


template <typename T>
std::string ToBinary( T &val ) {
    std::string res = "";

    // sizeof gets number of bytes, num bits is bytes * 8 because 8 bits per byte
    size_t num_bits = sizeof(T) * 8;

    // says if we have seen a non zero character
    bool seen_non_zero = false;

    // start at n - 1 because that is the most left bit
    for ( size_t i = num_bits; i > 0; i-- ) {
        const int shift = 1 << ( i - 1 );
        const bool is_one = ( val & shift ) > 0;

        // if one, show one and say that we have now seen a now zero
        if ( is_one ) { 
            res += '1';
            seen_non_zero = true;
        }
        // else, is zero. if we have seen a non zero, show the zero
        else if ( seen_non_zero ) {
            res += '0';
        }
    }

    return res;
}

template <typename T>
std::string ToBinaryCopy( T val ) {
    return ToBinary( val );
}


#endif  // INT_TO_BINARY_HPP