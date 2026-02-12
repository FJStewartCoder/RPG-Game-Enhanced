#ifndef NODES_H
#define NODES_H

#include <string>

#include "sol/sol.hpp"

typedef long long int coord_hash;

// NOTE TO READER: HASH IS STORED AS IT WANTS TO BE
// THIS HAPPENS TO BE IN THE FORMAT Z Y X
// IF THE HASH SEEMS WRONG, CONSIDER THIS
typedef union {
    struct {
        short x;
        short y;
        short z;
    };

    coord_hash hash;
} coordinates_t;

coordinates_t add_coords(coordinates_t a, coordinates_t b);

int init_coords(coordinates_t *coords);

// HASH IS FORMATTED Z Y X. CONSIDER THIS WHEN READING THE HASH
std::string coords_to_str( coordinates_t *coords, bool show_hash = false );


typedef struct node_t {
    // will be used to link to the data
    std::string node_type;

    // essentially just the index in the array
    coordinates_t coords;

    // table of data that is unique to this node
    sol::optional<sol::table> unique_data;

    int blocked_directions;

    // pointers to other nodes for traversal
    struct node_t *left;
    struct node_t *right;
    struct node_t *forward;
    struct node_t *back;
    struct node_t *up;
    struct node_t *down;

    // pointers for special traversal
    struct node_t *next;
    struct node_t *previous;
} node_t;


// enum of directions you can go
// ENUM HAS INT VALUES AS SUCH TO BE ABLE TO CREATE THE BLOCKED DIRECTIONS USING INT
typedef enum {
    NODE_LEFT = 1 << 0,
    NODE_RIGHT = 1 << 1,
    NODE_FORWARD = 1 << 2,
    NODE_BACK = 1 << 3,
    NODE_UP = 1 << 4,
    NODE_DOWN = 1 << 5,
    NODE_NEXT = 1 << 6,
    NODE_PREV = 1 << 7,

    // use as a default
    NODE_NONE = 1 << 8,
    // quit signal
    NODE_QUIT = 1 << 9
} node_directions;

typedef enum {
    NODE_OK,
    NODE_ERROR,
    
    // error for if you achieve something impossible
    NODE_HOW
} node_errors;

// initialises a node
void node_init(node_t *node);

// checks for if you can traverse a node in a specific direction or generally
node_errors can_traverse_direction(node_t *node);
node_errors can_traverse(node_t *node);

// node traversal function
node_errors traverse_node(
    node_t *(&node),  // to change where the pointer is pointing, we need reference to ptr
    node_directions direction
);

// BASIC BLOCKING (use char in brackets)
// (l)eft, (r)ight, (u)p, (d)own, (f)orward, (b)ack, (n)ext and (p)revious

// UNBLOCKING
// use ! as the FIRST character to invert the operation i.e (l)eft will only allow left

// SPECIAL
// x, y, z and (t)eleport are for corresponding directions i.e x = lr, y = fb, z = ud and t = np
int str_to_blocked_nodes( std::string str );

bool is_dir_blocked( int blocked_str, node_directions dir );

const std::string dir_to_string( node_directions direction );

#endif // NODES_H