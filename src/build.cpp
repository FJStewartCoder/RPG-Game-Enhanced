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
    log_trace("Initialising node manager.");
}

NodeManager::~NodeManager() {
    log_trace("Destructing node manager by deleting nodes");

    log_debug("There are %d nodes to delete", environment.size());

    for ( const auto &item : environment ) {
        const node_t *node = item.second;

        log_debug("Deleting node: %s", node->node_type.c_str());

        delete node;
    }
}

void NodeManager::build_node(
    std::string node_type,
    coordinates_t coords,
    sol::table unique_data,
    std::string blocked_directions
) {
    const bool type_exists = all_node_types.find(node_type) != all_node_types.end();

    if ( !type_exists ) {
        throw CustomException("This node type: does not exist or is malformed.");
    }

    // allocate heap space for the new node
    // this is done to ensure that the pointers always point to the same location
    node_t *new_node = new node_t;
    node_init(new_node);

    // get the hash of the coordinates requests
    const coord_hash hash = get_coords_hash(coords);
    const bool position_taken = environment.find(hash) != environment.end();

    if ( position_taken ) {
        throw CustomException("These coordinates are already taken");
    }

    // add the node to the new environment is the coordinates are not already taken
    environment[hash] = new_node;

    // set the name, unique data and coordinates
    new_node->node_type = node_type;
    new_node->unique_data = unique_data;

    new_node->coords = coords;

    // end
    return;
}

int NodeManager::new_node_type(sol::environment &core_env, sol::table node_table) {
    log_trace("New node type being created");

    // add the node table to the new lua queue
    core_env[engine::node::QUEUE].get<sol::table>().add(node_table);

    return 0;
}

int NodeManager::build_single_node(
    sol::environment &core_env,
    sol::table node_template,
    sol::table node_table
) {
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
node_t *NodeManager::get_node(coordinates_t coords) {
    // get the hash
    const coord_hash hash = get_coords_hash(coords);

    // search for the hash
    auto search_res = environment.find(hash);

    // check for existance
    const bool node_found = search_res != environment.end();

    if ( !node_found ) {
        return NULL;
    }   

    // does exist so get the node * from the pair
    node_t* node = search_res->second;

    // return the found node
    return node;
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

int NodeManager::make_connection(
    coordinates_t node1,
    coordinates_t node2,
    node_directions link,
    bool one_way,
    bool override_blocked
) {
    node_t *node1_ptr = get_node(node1);
    node_t *node2_ptr = get_node(node2);

    // if the node does not exist return invalid
    if ( node1_ptr == NULL ) {
        return 1;
    }

    // if the node does not exist return invalid
    if ( node2_ptr == NULL ) {
        return 1;
    }

    // switch the link direction

    // check if the connection is valid by checking if node already has that direction full
    // also check if it is blocked and if we are overriding block and the one way thing

    // TODO: implement blocking and validation to prevent overwriting an existing connection

    bool connection_blocked = false;

    // a pointer to the next and previous pointer
    // we want to be able to change the pointer so we need a pointer to a pointer
    node_t **cur = NULL;  // the pointer from the next node to the current node based on the opposite of the direction specified
    node_t **next = NULL;  // pointer from the current node to the next node in the direction specified

    switch ( link ) {
        case NODE_LEFT:
            /*
            if (
                node1_ptr->blocked_directions has 'l' or
                node2_ptr->blocked_direction has 'r'
            ) {
                connection_blocked = true;
            }
            */

            next = &node1_ptr->left;
            cur = &node2_ptr->right;
            
            break;
        
        case NODE_RIGHT:
            next = &node1_ptr->right;
            cur = &node2_ptr->left;
            
            break;
        
        case NODE_FORWARD:
            next = &node1_ptr->forward;
            cur = &node2_ptr->back;
            
            break;
        
        case NODE_BACK:
            next = &node1_ptr->back;
            cur = &node2_ptr->forward;
            
            break;

        case NODE_UP:
            next = &node1_ptr->up;
            cur = &node2_ptr->down;
            
            break;
        
        case NODE_DOWN:
            next = &node1_ptr->down;
            cur = &node2_ptr->up;
            
            break;
        
        case NODE_PREV:
            next = &node1_ptr->previous;
            cur = &node2_ptr->next;
            
            break;

        case NODE_NEXT:
            next = &node1_ptr->next;
            cur = &node2_ptr->previous;
            
            break;
        
        default:
            break;
    }

    // if there are no pointers then we can't continue processing
    if ( cur == NULL || next == NULL ) {
        return 1;
    }

    // only if the connection is blocked and we are overriding the block, we have to return
    if ( connection_blocked && !override_blocked ) {
        return 1;
    }

    // only make a connection if a connection is not already established
    // this would mean that the next would be NULL

    // TODO: this needs to be improved to be more readable and accurate
    if ( *next == NULL ) {
        *next = node2_ptr;

        if ( !one_way ) {
            if ( *cur == NULL ) {
                *cur = node1_ptr;
            }
        }
    }

    return 0;
}

int NodeManager::make_all_connections() {
    log_debug("Found %d nodes to make connections.", environment.size());

    for ( const auto &node_pair : environment ) {
        auto node = node_pair.second;

        // get the coordinates
        coordinates_t cur_coords = node->coords;

        using offset_t = std::pair<coordinates_t, node_directions>;

        // a list of add offsets and their corresponding direction
        const offset_t offsets[] = {
            offset_t({1, 0, 0}, NODE_RIGHT),
            offset_t({-1, 0, 0}, NODE_LEFT),
            offset_t({0, 0, 1}, NODE_FORWARD),
            offset_t({0, 0, -1}, NODE_BACK),
            offset_t({0, 1, 0}, NODE_UP),
            offset_t({0, -1, 0}, NODE_DOWN)
        };

        for ( const auto &item : offsets ) {
            const coordinates_t new_coords = add_coords(cur_coords, item.first);

            log_debug(
                "Trying to make connection between (%d %d %d) and (%d %d %d)",
                cur_coords.x,
                cur_coords.y,
                cur_coords.z,
                new_coords.x,
                new_coords.y,
                new_coords.z
            );

            make_connection(
                cur_coords,
                new_coords,
                item.second
            );

        }
    }

    return 0;
}

std::unordered_set<std::string> &NodeManager::get_all_node_types() {
    return all_node_types;
}