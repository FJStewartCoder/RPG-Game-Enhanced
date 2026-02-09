#ifndef INPUT_H
#define INPUT_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// how many new characters to allocate for the new buffer
#define MEM_SIZE_INCREMENT 5
// how big of a buffer to initially allocate
#define MEM_SIZE_START 20


typedef enum {

    ORDER_FIRST,
    ORDER_LAST

} read_order_t;

int stdin_to_buf( char *buf, size_t size, read_order_t order ) {
    // read buffer
    char c;

    // the current write position
    size_t read_idx = 0;

    // the index at which we need to stop reading
    const size_t read_end = size - 1;

    // read differently depending on the read order
    if ( order == ORDER_FIRST ) {
        // READ THE FIRST CHARACTERS AND IGNORE ANY OTHER CHARACTERS

        // read until end
        while ( ( c = getchar() ) != '\n' ) {
            // once we are at the end, read into the void
            if ( read_idx >= read_end ) {
                continue;
            }

            // copy the character to the buffer and increment the read index
            buf[read_idx++] = c;
        }

        // terminate the string at where ever we stop reading
        buf[read_idx] = '\0';
    }

    else if ( order == ORDER_LAST ) {
        // READ ALL OF THE CHARACTERS FROM STDIN BUT ONLY KEEP THE MOST RECENTLY ENTERED CHARACTERS

        // read until end
        while ( ( c = getchar() ) != '\n' ) {
            // once we are at the end, read into the void
            if ( read_idx >= read_end ) {
                // buffer of length 5
                // read end = 4
                // buffer is filled [abcd ]
                // shift from buffer start ( the ptr ) + 1 to the start of the buffer
                // ( shift buf + 1 to buf ) 
                // we shift the length of the filled buffer ( size - 1 ) - 1
                // so shift size - 2 chars
                // then, read the new char to where? the size - 2 again 
                // shift from 

                memmove( buf, buf + 1, size - 2 );
                buf[size - 2] = c;

                continue;
            }

            // copy the character to the buffer and increment the read index
            buf[read_idx++] = c;
        }

        // terminate the string at where ever we stop reading
        buf[read_idx] = '\0';
    }

    return 0;
}

// reads the entire stdin to a piece of heap allocated memory
char *stdin_to_new_buf() {
    // the buffer for the current character
    char c;

    size_t buf_len = MEM_SIZE_START;

    char *buf = (char *) malloc( buf_len * sizeof(char) );

    // the current read position
    size_t read_idx = 0;

    while ( ( c = getchar() ) != '\n' ) {
        // if the buffer is invalid, write to the void
        if ( buf == NULL ) {
            continue;
        }

        // once we are at the end, read into the void
        if ( read_idx >= buf_len ) {
            // increase the memory size
            buf_len += MEM_SIZE_INCREMENT;

            buf = (char *) realloc( buf, buf_len * sizeof(char) );
        }

        // copy the character to the buffer and increment the read index
        buf[read_idx++] = c;
    }

    // allocate enough space for one more character ( the null terminator )
    buf = (char *) realloc( buf, (buf_len + 1) * sizeof(char) );

    // if the buffer is valid, add a null terminator
    if ( buf != NULL ) {
        // terminate the string at where ever we stop reading
        buf[read_idx] = '\0';
    }

    return buf;
}


#endif  // INPUT_H