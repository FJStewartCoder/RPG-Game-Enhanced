#ifndef NODES_H
#define NODES_H

typedef struct node_t {
    // will be used to link to the data
    const char *node_type;

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
    NODE_PREV
} node_directions;

typedef enum {
    NODE_OK,
    NODE_ERROR,
    
    // error for if you achieve something impossible
    NODE_HOW
} node_errors;

#endif // NODES_H