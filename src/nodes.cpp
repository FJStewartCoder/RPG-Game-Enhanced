#include "nodes.hpp"

// for NULL
#include <stdio.h>

node_errors traverse_helper(node_t *node, node_t *next_node) {
    // if NULL, do nothing else set *node to *next_node
    if ( next_node != NULL ) {
        node = next_node;
        return NODE_OK;
    }

    return NODE_ERROR;
}

// return error code
// sets the current node passed in to the new traversed node is no errors occur
node_errors traverse_node(node_t *node, node_directions direction) {
    node_errors res = NODE_OK;

    switch (direction) {
        case NODE_LEFT:
            res = traverse_helper(node, node->left);
            break;
        
        case NODE_RIGHT:
            res = traverse_helper(node, node->right);
            break;
        
        case NODE_UP:
            res = traverse_helper(node, node->up);
            break;
        
        case NODE_DOWN:
            res = traverse_helper(node, node->down);
            break;
        
        case NODE_FORWARD:
            res = traverse_helper(node, node->forward);
            break;
        
        case NODE_BACK:
            res = traverse_helper(node, node->back);
            break;
        
        case NODE_NEXT:
            res = traverse_helper(node, node->next);
            break;
        
        case NODE_PREV:
            res = traverse_helper(node, node->previous);
            break;

        // literally not even possible
        default:
            res = NODE_HOW;
            break;
    }

    return res;
} 