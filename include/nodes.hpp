#ifndef NODES_H
#define NODES_H

#include <string>
#include <vector>

#include "sol/sol.hpp"

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

// function to build a node
node_t build_node(
    std::vector<std::string> node_types,
    std::string node_type,
    sol::table unique_data = sol::table(),
    node_t *previous_node = nullptr,
    node_directions relation = NODE_NONE,
    bool one_way = false  // defines whether or not the new node added should be able to link back to the previous node
);

#endif // NODES_H