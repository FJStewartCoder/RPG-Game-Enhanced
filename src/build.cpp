#include "build.hpp"

#include "custom_exception.hpp"

#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}


// CLASSLESS -----------------------------------------------------------------------------------------------------------


node_directions str_to_direction(std::string dir) {
    if ( dir == "left" || dir == "l" ) { return NODE_LEFT; }
    if ( dir == "right" || dir == "r" ) { return NODE_RIGHT; }
    if ( dir == "up" || dir == "u" ) { return NODE_UP; }
    if ( dir == "down" || dir == "d" ) { return NODE_DOWN; }

    if ( dir == "forward" || dir == "f" ) { return NODE_FORWARD; }
    if ( dir == "back" || dir == "b" ) { return NODE_BACK; }
    if ( dir == "next" || dir == "l" ) { return NODE_NEXT; }
    if ( dir == "previous" || dir == "prev" || dir == "p" ) { return NODE_PREV; }

    // if nothing
    return NODE_NONE;
}

// checks to see if a node table is valid
int check_default_node_table(sol::table &table) {
    sol::optional<std::string> name = table[engine::node::NAME];
    sol::optional<sol::function> on_land = table[engine::node::LAND];
    sol::optional<sol::function> on_leave = table[engine::node::LEAVE];

    if ( !name ) {
        log_error("Node data does not contain name field.");
        return 1;
    }
    else if ( !on_land ) {
        log_error("Node with name \"%s\" does not have landing function.", name.value().c_str());
        return 1;
    }
    else if ( !on_leave ) {
        log_error("Node with name \"%s\" does not have leaving function.", name.value().c_str());
        return 1;
    }

    return 0;
}


// CLASS ---------------------------------------------------------------------------------------------------------------


// constructor and destructor
NodeManager::NodeManager() {

}

NodeManager::~NodeManager() {
    log_trace("Destructing node manager by deleting nodes");

    log_debug("There are %d nodes to delete", environment.size());

    for ( const auto &item : environment ) {
        log_debug("Deleting node: %s", item->node_type.c_str());

        delete item;
    }
}

int NodeManager::build_node(
    std::string node_type,
    sol::table unique_data,
    int previous_node_id,
    std::string relation,
    bool one_way  // defines whether or not the new node added should be able to link back to the previous node
) {
    bool type_exists = all_node_types.find(node_type) != all_node_types.end();

    if ( !type_exists ) {
        throw CustomException("This node type: does not exist or is malformed.");
    }

    // allocate heap space for the new node
    // this is done to ensure that the pointers always point to the same location
    node_t *new_node = new node_t;

    if ( new_node == NULL ) {
        log_error("Node did not allocate"); 
        return -1;
    }

    node_init(new_node);

    // add the node to the new environment
    environment.push_back(new_node);

    // set the name and unique data
    new_node->node_type = node_type;
    new_node->unique_data = unique_data;
    new_node->id = environment.size() - 1;

    // done at this point
    if ( previous_node_id <= -1 ) {
        // return the index of the previous node
        return environment.size() - 1;
    }

    // get the previous node based on the id
    node_t *previous_node = get_node(previous_node_id);

    // get the direction of the node by string
    node_directions direction = str_to_direction(relation);

    switch (direction) {
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

int NodeManager::new_node_type(sol::environment &core_env, sol::table node_table) {
    // add the node table to the new lua queue
    core_env[engine::node::QUEUE].get<sol::table>().add(node_table);

    return 0;
}

int NodeManager::build_single_node(sol::environment &core_env, sol::table node_template, sol::table node_table) {
    // create the new table
    sol::table new_table = core_env.create();
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

    // validation of the default node table

    // FIXES BELOW ERROR:
    //     could experience an error where the node_table that is passed in overrights default parameters with other invalid types
    //     ensure that when the node table is passed in, all integral data is of the correct type.

    const bool default_table_invalid = check_default_node_table(new_table) != 0;

    if ( default_table_invalid ) {
        return 1;
    }

    // insert the new element to the node_types
    const std::string node_name = new_table["name"].get<std::string>();
    const bool node_name_found = all_node_types.find(node_name) != all_node_types.end();

    if ( node_name_found ) {
        log_error("Node \"%s\" already exists", node_name.c_str());
        return 1;
    }
    
    // add the new node_name into the node_types list
    all_node_types.insert(node_name);

    // insert back into lua
    sol::table avail = core_env[engine::node::AVAILABLE];
    avail.add(new_table);

    // log
    log_info("Added new node type with name %s", new_table[engine::node::NAME].get<std::string>().c_str());

    return 0;
}

// get a node from the environment
node_t *NodeManager::get_node(int id) {
    if ( id >= environment.size() || id < 0 ) {
        throw CustomException("The node selected does not exist.");
    }

    // get the previous node based on the id
    return environment.at(id);
}

// function to build the node queue
int NodeManager::build_node_queue(sol::environment &core_env, sol::table node_template) {
    sol::table node_queue = core_env[engine::node::QUEUE];

    log_debug("Node queue has length %d", node_queue.size());

    for ( const auto &table : node_queue ) {
        // second because first is the index
        build_single_node(core_env, node_template, table.second);
    }

    return 0;
}

std::unordered_set<std::string> &NodeManager::get_all_node_types() {
    return all_node_types;
}