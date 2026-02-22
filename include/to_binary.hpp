#ifndef INT_TO_BINARY_HPP
#define INT_TO_BINARY_HPP

#include <string>


template <typename T>
std::string ToBinary( T &val ) {
    std::string res = "";

    // sizeof gets number of bytes, num bits is bytes * 8 because 8 bits per byte
    size_t num_bits = sizeof(T) * 8;

    // start at n - 1 because that is the most left bit
    for ( size_t i = num_bits; i > 0; i-- ) {
        const int shift = 1 << ( i - 1 );

        // if val at i binary location set 1 else 0
        res += ( (val & shift) > 0 ) ? '1' : '0';
    }

    return res;
}

template <typename T>
std::string ToBinaryCopy( T val ) {
    return ToBinary( val );
}


#endif  // INT_TO_BINARY_HPP