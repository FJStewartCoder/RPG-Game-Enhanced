#ifndef NODES_H
#define NODES_H

#include <string>

typedef struct node_t {
    // will be used to link to the data
    std::string node_type;

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
    NODE_NONE
} node_directions;

typedef enum {
    NODE_OK,
    NODE_ERROR,
    
    // error for if you achieve something impossible
    NODE_HOW
} node_errors;

// node traversal function
node_errors traverse_node(
    node_t **node,  // to change where the pointer is pointing, we need ptr to ptr
    node_directions direction
);

// function to build a node
node_t build_node(
    std::string node_type,
    node_t *previous_node = nullptr,
    node_directions relation = NODE_NONE,
    bool one_way = false  // defines whether or not the new node added should be able to link back to the previous node
);

#endif // NODES_H