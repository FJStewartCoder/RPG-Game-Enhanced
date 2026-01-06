#ifndef NODES_H
#define NODES_H

#include <string>

#include "sol/sol.hpp"


// some special constants for names of variables in lua
const std::string LUA_NODE_NAME = "name";
const std::string LUA_NODE_LAND = "on_land";
const std::string LUA_NODE_LEAVE = "on_leave";

const std::string LUA_NODE_TEMPLATE = "NODE_DATA_TEMPLATE";
const std::string LUA_NODE_AVAILABLE = "AVAILIBLE_NODES";

const std::string LUA_CORE_NODE_FILE = "core/node_data.lua";


typedef struct node_t {
    // will be used to link to the data
    std::string node_type;

    // table of data that is unique to this node
    sol::optional<sol::table> unique_data;

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
typedef enum {
    NODE_LEFT,
    NODE_RIGHT,
    NODE_FORWARD,
    NODE_BACK,
    NODE_UP,
    NODE_DOWN,
    NODE_NEXT,
    NODE_PREV,

    // use as a default
    NODE_NONE,
    // quit signal
    NODE_QUIT
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

#endif // NODES_H