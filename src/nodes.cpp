#include "nodes.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "table.hpp"
#include "custom_exception.hpp"

#include "to_binary.hpp"


coordinates_t add_coords(coordinates_t a, coordinates_t b) {
    log_trace("Called function \"%s( %s, %s )\"",
        __FUNCTION__,
        coords_to_str( &a, true ).c_str(),
        coords_to_str( &b, true ).c_str()
    );
    
    coordinates_t new_coords;
    init_coords( &new_coords );
    
    // must initialise coords then set the values otherwise the hash will be wrong
    new_coords.x = (short)(a.x + b.x);
    new_coords.y = (short)(a.y + b.y);
    new_coords.z = (short)(a.z + b.z);

    log_debug("Added coordinations are: %s",
        coords_to_str( &new_coords, true ).c_str()
    );

    return new_coords;
}

int init_coords(coordinates_t *coords) {
    log_trace("Called function \"%s( coords )\"", __FUNCTION__);

    // initialise the hash to 0 since x, y, z don't cover the full 64 bits
    coords->hash = 0;

    coords->x = 0;
    coords->y = 0;
    coords->z = 0;

    return 0;
}

coordinates_t create_coords( short x, short y, short z ) {
    coordinates_t coords;
    init_coords( &coords );

    coords.x = x;
    coords.y = y;
    coords.z = z;

    return coords;
}

std::string coords_to_str( coordinates_t *coords, bool show_hash ) {

#ifndef REMOVE_FREQUENT_LOGS 
    // THIS TRACE IS VERY ANNOYING
    log_trace("Called function \"%s( coords, %d )\"",
        __FUNCTION__,
        show_hash
    );

#endif // REMOVE_FREQUENT_LOGS

    // this currently is as such "(x, y, z"
    std::string res = "(" + std::to_string(coords->x) + ", " + std::to_string(coords->y) + ", " + std::to_string(coords->z);

    if ( show_hash ) {
        // convert the hash into a hex representation since it is shorter
        char hex_buf[64];
        sprintf(hex_buf, "%lx", coords->hash);

        // if chosen to show hash add on ", h=h"
        res += ", h=" + std::string(hex_buf);
    }
    
    // finalise the string with ")"
    return res + ")";
}

void node_init(node_t *node) {
    log_trace("Called function \"%s( node )\"", __FUNCTION__);

    // set the node type name to blank
    node->node_type = "";

    // set the name to blank
    node->unique_name = "";

    // initialise coordinates
    init_coords( &node->coords );

    // block no directions
    node->blocked_directions = 0;

    // set all travel pointers to NULL
    node->left = NULL;
    node->right = NULL;
    node->forward = NULL;
    node->back = NULL;
    node->up = NULL;
    node->down = NULL;
    node->next = NULL;
    node->previous = NULL;
}

// I don't know what this is meant to do but I don't think that it does it
node_errors can_traverse_direction(node_t *node) {
    log_trace("Called function \"%s( node )\"", __FUNCTION__);

    // check if this direction can be traversed to which would be not a nullptr
    if ( node != NULL ) {
        return NODE_OK;
    }

    return NODE_ERROR;
}

// checks if it is possible to traverse from this node
node_errors can_traverse(node_t *node) {
    log_trace("Called function \"%s( node )\"", __FUNCTION__);

    // check each direction. if not null then it is possible to traverse

    if ( node->left != NULL ) { return NODE_OK; }
    if ( node->right != NULL ) { return NODE_OK; }
    if ( node->up != NULL ) { return NODE_OK; }
    if ( node->down != NULL ) { return NODE_OK; }
    if ( node->forward != NULL ) { return NODE_OK; }
    if ( node->back != NULL ) { return NODE_OK; }
    if ( node->next != NULL ) { return NODE_OK; }
    if ( node->previous != NULL ) { return NODE_OK; }

    return NODE_ERROR;
}

// return error code
// sets the current node passed in to the new traversed node is no errors occur
node_errors traverse_node(node_t *(&node), node_directions direction) {
    log_trace("Called function \"%s( node, %d (%s) )\"",
        __FUNCTION__,
        direction, dir_to_string( direction ).c_str()
    );

    node_errors res = NODE_OK;

    node_t *next = get_node_in_direction( node, direction );

    res = can_traverse_direction(next);

    if ( res == NODE_OK ) {
        // deference next for assignment
        node = next;
    }

    return res;
}


// BASIC BLOCKING (use char in brackets)
// (l)eft, (r)ight, (u)p, (d)own, (f)orward, (b)ack, (n)ext and (p)revious

// UNBLOCKING
// use ! as the FIRST character to invert the operation i.e (l)eft will only allow left

// SPECIAL
// x, y, z and (t)eleport are for corresponding directions i.e x = lr, y = fb, z = ud and t = np

int set_blocked_state( int &dest, node_directions dir, bool blocking_mode ) {
    log_trace("Called function \"%s( %s, %s (%s), %d )\"",
        __FUNCTION__,
        ToBinary( dest ).c_str(),
        ToBinary( dir ).c_str(),
        dir_to_string( dir ).c_str(),
        blocking_mode
    );

    if ( blocking_mode ) {
        // adding left with or
        dest |= dir;
    }
    else {
        // to remove, use and 0 with all 1s
        // i.e not the value
        dest &= ~dir;
    }

    return 0;
}

int str_to_blocked_nodes( std::string str ) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        str.c_str()
    );

    // the buffer to store the current character in
    char c;

    int res = 0;
    bool blocking_mode = true;

    for ( int i = 0; i < str.length(); i++ ) {
        // set the character buffer to the current character
        c = str[i];

        // if the first character is !
        // set the res
        if ( i == 0 && c == '!' ) {
            // set res to all 1s
            res = ~0;

            // set the mode to unblocking mode
            blocking_mode = false;

            // don't need to continue this loop
            continue;
        }

        switch ( c ) {
            case 'l':
                set_blocked_state( res, NODE_LEFT, blocking_mode );
                break;

            case 'r':
                set_blocked_state( res, NODE_RIGHT, blocking_mode );
                break;

            case 'u':
                set_blocked_state( res, NODE_UP, blocking_mode );
                break;                

            case 'd':
                set_blocked_state( res, NODE_DOWN, blocking_mode );
                break; 

            case 'f':
                set_blocked_state( res, NODE_FORWARD, blocking_mode );
                break; 

            case 'b':
                set_blocked_state( res, NODE_BACK, blocking_mode );
                break; 

            case 'n':
                set_blocked_state( res, NODE_NEXT, blocking_mode );
                break; 
                
            case 'p':
                set_blocked_state( res, NODE_PREV, blocking_mode );
                break; 
                
            case 'x':
                set_blocked_state( res, NODE_LEFT, blocking_mode );
                set_blocked_state( res, NODE_RIGHT, blocking_mode );
                break; 

            case 'y':
                set_blocked_state( res, NODE_UP, blocking_mode );
                set_blocked_state( res, NODE_DOWN, blocking_mode );
                break;

            case 'z':
                set_blocked_state( res, NODE_FORWARD, blocking_mode );
                set_blocked_state( res, NODE_BACK, blocking_mode );
                break; 

            case 't':
                set_blocked_state( res, NODE_NEXT, blocking_mode );
                set_blocked_state( res, NODE_PREV, blocking_mode );
                break; 

            default:
                break;
        }
    }

    return res;
}

bool is_dir_blocked( int blocked_str, node_directions dir ) {
    log_trace("Called function \"%s( %s, %d (%s) )\"",
        __FUNCTION__,
        ToBinary( blocked_str ).c_str(),
        dir, dir_to_string( dir ).c_str()
    );

    return ( blocked_str & dir ) > 0;
}

// converts a direction to a string
// lower decides if lower case
const std::string dir_to_string( node_directions direction, bool lower ) {
    log_trace("Called function \"%s( %d )\"",
        __FUNCTION__,
        direction
    );

    std::string res = "";

    switch ( direction ) {
        case NODE_LEFT:
            if ( lower ) { res = "left"; }
            else { res = "Left"; }
            break;

        case NODE_RIGHT:
            if (lower) { res = "right"; }
            else { res = "Right"; }
            break;

        case NODE_UP:
            if (lower) { res = "up"; }
            else { res = "Up"; }
            break;
        
        case NODE_DOWN:
            if (lower) { res = "down"; }
            else { res = "Down"; }
            break;

        case NODE_FORWARD:
            if (lower) { res = "forward"; } 
            else { res = "Forward"; }
            break;

        case NODE_BACK:
            if (lower) { res = "back"; } 
            else { res = "Back"; }
            break;

        case NODE_NEXT:
            if (lower) { res = "next"; } 
            else { res = "Next"; }
            break;

        case NODE_PREV:
            if (lower) { res = "previous"; } 
            else { res = "Previous"; }
            break;

        case NODE_QUIT:
            if (lower) { res = "quit"; } 
            else { res = "Quit"; }
            break;

        case NODE_NONE:
            if (lower) { res = "none"; } 
            else { res = "None"; }
            break;
        
        default:
            res = "<UNKNOWN>";
            break;
    }

    return res;
}

node_t *get_node_in_direction( node_t *node, node_directions dir ) {
    node_t *res = NULL;

    switch ( dir ) {
        case NODE_LEFT:
            res = node->left;
            break;

        case NODE_RIGHT:
            res = node->right;
            break;

        case NODE_UP:
            res = node->up;
            break;

        case NODE_DOWN:
            res = node->down;
            break;

        case NODE_FORWARD:
            res = node->forward;
            break;

        case NODE_BACK:
            res = node->back;
            break;

        case NODE_NEXT:
            res = node->next;
            break;

        case NODE_PREV:
            res = node->previous;
            break;
    }

    return res;
}


// ----------------------------------------------------------------------------------------------


// max and min are 32767 -32768 this is 0b0111 1111 1111 1111 and 0b1000 0000 0000 0000

// the biggest possible short 0xffff >> 1 because all 1s except the first (left to right)
const static inline int max_short = (short)(0xffff >> 1); 
// min short is 0b 1000 0000 0000 0000
const static inline int min_short = (short)(1 << 15);


bool is_valid_short( int num ) {
    return ( num <= max_short ) && ( num >= min_short );
}

coordinates_t parse_coordinate_table(sol::table &coords) {
    log_trace("Called function \"%s( table )\"", __FUNCTION__);

    // initialise x, y, and z
    int x, y, z; 

    // get the values from the table
    if ( IsList( coords ) ) {
        // if list, assume that the first 3 values are x, y, z
        x = coords[1].get_or( 0 );
        y = coords[2].get_or( 0 );
        z = coords[3].get_or( 0 );
    }
    else {
        // if dict-like, assume there are keys x, y, z
        x = coords["x"].get_or( 0 );
        y = coords["y"].get_or( 0 );
        z = coords["z"].get_or( 0 );
    }

    // if any are invalid, throw an exception
    if ( !is_valid_short(x) ) { throw CustomException("x must be between 32767 and -32768"); }
    if ( !is_valid_short(y) ) { throw CustomException("y must be between 32767 and -32768"); }
    if ( !is_valid_short(z) ) { throw CustomException("z must be between 32767 and -32768"); }

    // only valid shorts are not shoved in
    coordinates_t res = create_coords( x, y, z );

    log_debug("Parsed table to coords %s", coords_to_str(&res, true).c_str() );

    return res;
}
