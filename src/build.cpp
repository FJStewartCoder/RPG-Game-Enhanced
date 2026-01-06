#include "build.hpp"

#include "custom_exception.hpp"
#include <unordered_set>
#include "log/log.h"

// the vector which will store all of the nodes
std::vector<node_t*> environment;


int free_nodes() {
    for ( const auto &item : environment ) {
        delete item;
    }

    return 0;
}

node_t *get_node(int id) {
    if ( id >= environment.size() || id < 0 ) {
        throw CustomException("The node selected does not exist.");
    }

    // get the previous node based on the id
    return environment.at(id);
}

int build_node(
    std::vector<std::string> node_types,
    std::string node_type,
    sol::table unique_data,
    int previous_node_id,
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
        throw CustomException("This node type: does not exist or is malformed.");
    }

    // allocate heap space for the new node
    // this is done to ensure that the pointers always point to the same location
    node_t *new_node = new node_t;

    if ( new_node == NULL ) {
        std::cout << "Node did not allocate" << std::endl; 
        return -1;
    }

    node_init(new_node);

    // add the node to the new environment
    environment.push_back(new_node);

    // set the name and unique data
    new_node->node_type = node_type;
    new_node->unique_data = unique_data;

    // done at this point
    if ( previous_node_id <= -1 ) {
        // return the index of the previous node
        return environment.size() - 1;
    }

    // get the previous node based on the id
    node_t *previous_node = get_node(previous_node_id);

    switch (relation) {
        case NODE_LEFT:
            previous_node->left = new_node;

            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->right = previous_node; }
            break;
        
        case NODE_RIGHT:
            previous_node->right = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->left = previous_node; }
            break;
        
        case NODE_UP:
            previous_node->up = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->down = previous_node; }
            break;
        
        case NODE_DOWN:
            previous_node->down = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->up = previous_node; }
            break;
        
        case NODE_FORWARD:
            previous_node->forward = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->back = previous_node; }
            break;
        
        case NODE_BACK:
            previous_node->back = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->forward = previous_node; }
            break;
        
        case NODE_NEXT:
            previous_node->next = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->previous = previous_node; }
            break;
        
        case NODE_PREV:
            previous_node->previous = new_node;
            
            // only allowing backtracking if not one way
            if ( !one_way ) { new_node->next = previous_node; }
            break;

        default:
            break;
    }

    // return the index of the new node
    return environment.size() - 1;
}

int new_node_type(sol::state &core, sol::table node_table) {
    sol::table new_copy = core.create_table();

    sol::table node_queue = core["NODE_QUEUE"];

    for ( const auto &item : node_table ) {
        new_copy[item.first] = item.second;
    }

    // add the node table to the new lua queue
    node_queue.add(new_copy);

    return 0;
}

int build_single_node(sol::state &lua, sol::table node_template, sol::table node_table) {
    // create the new table
    sol::table new_table = lua.create_table();
    std::unordered_set<std::string> availible_keys;

    // copy the template into the new table
    for ( const auto &pair : node_template ) {
        new_table[pair.first] = pair.second;

        // add the key to the set
        availible_keys.insert(pair.first.as<std::string>());
    }

    // iterate all of the new pairs
    for ( const auto &new_pair : node_table ) {
        // if the key already exists in the table, set the value at that key to the new value passed in
        if ( availible_keys.find( new_pair.first.as<std::string>() ) != availible_keys.end() ) {
            new_table[new_pair.first] = new_pair.second;
        }
        else {
            // TODO: FIX
            // log_warn("Script tried to pass key to node with key %s that is not in the template.", new_pair.first.as<std::string>() );
        }
    }

    std::cout << "1" << std::endl;

    // insert the new element to the node_types
    // all_node_types[new_table["name"].get<std::string>()] = new_table;

    // insert back into lua
    auto avail = lua[LUA_NODE_AVAILABLE].get<sol::table>();
    avail.add(new_table);

    // log
    std::cout << "Added new node type with name " << new_table["name"].get<std::string>() << std::endl;

    return 0;
}

int build_node_queue(sol::state &lua, sol::table node_template) {
    sol::table build_queue = lua["NODE_QUEUE"];

    std::cout << "Build queue has size " << build_queue.size() << std::endl;

     for ( const auto &table : build_queue ) {
        sol::table t = table.second;

        std::cout << t["name"].get<std::string>() << std::endl;
    }

    for ( const auto &table : build_queue ) {
        // second because first is the index
        build_single_node(lua, node_template, table.second);
    }

    return 0;
}