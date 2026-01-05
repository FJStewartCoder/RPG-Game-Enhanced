#include "nodes.hpp"
#include "custom_exception.hpp"


void node_init(node_t *node) {
    node->node_type = "";

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

node_errors can_traverse(node_t *next_node) {
    // if NULL, do nothing else set *node to *next_node
    if ( next_node != nullptr ) {
        return NODE_OK;
    }

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

    res = can_traverse(next);

    if ( res == NODE_OK ) {
        // deference next for assignment
        node = next;
    }

    return res;
}

node_t build_node(
    std::vector<std::string> node_types,
    std::string node_type,
    node_t *previous_node,
    node_directions relation,
    bool one_way  // defines whether or not the new node added should be able to link back to the previous node
) {
    bool type_exists = false;

    for ( const auto &node_name : node_types ) {
        if ( node_type == node_name ) {
            type_exists = true;
            break;
        }
    }

    if ( !type_exists ) {
        throw CustomException("This node type does not exist.");
    }

    node_t new_node;
    node_init(&new_node);

    // set the name
    new_node.node_type = node_type;

    // done at this point
    if ( previous_node == nullptr ) {
        return new_node;
    }

    switch (relation) {
        case NODE_LEFT:
            previous_node->left = &new_node;

            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.right = previous_node; }
            break;
        
        case NODE_RIGHT:
            previous_node->right = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.left = previous_node; }
            break;
        
        case NODE_UP:
            previous_node->up = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.down = previous_node; }
            break;
        
        case NODE_DOWN:
            previous_node->down = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.up = previous_node; }
            break;
        
        case NODE_FORWARD:
            previous_node->forward = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.back = previous_node; }
            break;
        
        case NODE_BACK:
            previous_node->back = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.forward = previous_node; }
            break;
        
        case NODE_NEXT:
            previous_node->next = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.previous = previous_node; }
            break;
        
        case NODE_PREV:
            previous_node->previous = &new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node.next = previous_node; }
            break;

        default:
            break;
    }

    return new_node;
}