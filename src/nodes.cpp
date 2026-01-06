#include "nodes.hpp"


void node_init(node_t *node) {
    node->node_type = "";

    // create blank table
    node->unique_data = sol::table();

    // set all directions to null
    node->left = nullptr;
    node->right = nullptr;
    node->up = nullptr;
    node->down = nullptr;
    node->forward = nullptr;
    node->back = nullptr;
    node->next = nullptr;
    node->previous = nullptr;
}

node_errors can_traverse_direction(node_t *node) {
    // check if this direction can be traversed to which would be not a nullptr
    if ( node != nullptr ) {
        return NODE_OK;
    }

    return NODE_ERROR;
}

// checks if it is possible to traverse from this node
node_errors can_traverse(node_t *node) {
    // check each direction. if not null then it is possible to traverse

    if ( node->left != nullptr ) { return NODE_OK; }
    if ( node->right != nullptr ) { return NODE_OK; }
    if ( node->up != nullptr ) { return NODE_OK; }
    if ( node->down != nullptr ) { return NODE_OK; }
    if ( node->forward != nullptr ) { return NODE_OK; }
    if ( node->back != nullptr ) { return NODE_OK; }
    if ( node->next != nullptr ) { return NODE_OK; }
    if ( node->previous != nullptr ) { return NODE_OK; }

    return NODE_ERROR;
}

// return error code
// sets the current node passed in to the new traversed node is no errors occur
node_errors traverse_node(node_t *(&node), node_directions direction) {
    node_errors res = NODE_OK;

    node_t *cur = node;
    node_t *next = nullptr;

    switch (direction) {
        case NODE_LEFT:
            next = cur->left;
            break;
        
        case NODE_RIGHT:
            next = cur->right;
            break;
        
        case NODE_UP:
            next = cur->up;
            break;
        
        case NODE_DOWN:
            next = cur->down;
            break;
        
        case NODE_FORWARD:
            next = cur->forward;
            break;
        
        case NODE_BACK:
            next = cur->back;
            break;
        
        case NODE_NEXT:
            next = cur->next;
            break;
        
        case NODE_PREV:
            next = cur->previous;
            break;

        // literally not even possible
        default:
            res = NODE_HOW;
            break;
    }

    res = can_traverse_direction(next);

    if ( res == NODE_OK ) {
        // deference next for assignment
        node = next;
    }

    return res;
}