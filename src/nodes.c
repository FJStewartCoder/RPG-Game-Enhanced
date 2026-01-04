#include "nodes.h"

// for NULL
#include <stdio.h>

node_errors traverse_helper(node_t *node, node_t *next_node) {
    // if NULL, do nothing else set *node to *next_node
    if ( next_node != NULL ) {
        node = next_node;
        return 0;
    }

    return 1;
}

// return error code
// sets the current node passed in to the new traversed node is no errors occur
node_errors traverse_node(node_t *node, node_directions direction) {
    node_errors res = NODE_OK;

    switch (direction) {
        // TODO: complete
        case NODE_LEFT:
            res = traverse_helper(node, node->left);
            break;

        // literally not even possible
        default:
            res = NODE_HOW;
            break;
    }
} 