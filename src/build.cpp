#include "build.hpp"

#include "custom_exception.hpp"

#include "lua_engine_constants.hpp"

extern "C" {
    #include "log/log.h"
}
#include "settings.h"

#include "table.hpp"


// CLASSLESS -----------------------------------------------------------------------------------------------------------


node_directions str_to_direction(std::string dir) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        dir.c_str()
    );
    
    // create return value and set to default return
    node_directions res = NODE_NONE;

    if ( dir == "left" || dir == "l" ) { 
        res = NODE_LEFT;
    }
    if ( dir == "right" || dir == "r" ) {
        res = NODE_RIGHT;
    }
    if ( dir == "up" || dir == "u" ) {
        res = NODE_UP;
    }
    if ( dir == "down" || dir == "d" ) {
        res = NODE_DOWN;
    }

    if ( dir == "forward" || dir == "f" ) {
        res = NODE_FORWARD;
    }
    if ( dir == "back" || dir == "b" ) {
        res = NODE_BACK;
    }
    if ( dir == "next" || dir == "n" ) {
        res = NODE_NEXT;
    }
    if ( dir == "previous" || dir == "prev" || dir == "p" ) {
        res = NODE_PREV;
    }

    log_debug("String \"%s\" converts to \"%s\"",
        dir.c_str(),
        dir_to_string( res ).c_str()
    );

    // if nothing
    return res;
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

    log_trace("Destruction complete");
}

void NodeManager::build_node(
    sol::state &lua,

    std::string type_name,
    std::string location_name,
    coordinates_t coords,
    sol::table unique_data,
    std::string blocked_directions
) {
    log_trace("Called function \"%s( sol::state&, %s, %s, %s, table, %s )\"",
        __FUNCTION__,
        type_name.c_str(),
        location_name.c_str(),
        coords_to_str( &coords, true ).c_str(),
        blocked_directions.c_str()
    );

    // search for the node and store the result
    const auto node_search_res = all_node_types.find( type_name );

    // check if this node search is valid
    const bool type_exists = node_search_res != all_node_types.end();

    if ( !type_exists ) {
        throw CustomException("This node type: does not exist or is malformed.");
    }

    // check if the coordinates are taken
    // get the hash of the coordinates requests
    const coord_hash hash = coords.hash;
    const bool position_taken = environment.find(hash) != environment.end();

    if ( position_taken ) {
        throw CustomException("These coordinates are already taken");
    }

    // allocate heap space for the new node
    // this is done to ensure that the pointers always point to the same location
    node_t *new_node = new node_t;
    node_init(new_node);

    // add the node to the new environment is the coordinates are not already taken
    environment[hash] = new_node;

    // INITIALISE DATA ------------------------------------------------------------------------

    // initialise and set coordinates
    new_node->coords = create_coords( coords.x, coords.y, coords.z );

    // parse blocked directions
    new_node->blocked_directions = str_to_blocked_nodes( blocked_directions );

    // set the location name and type name
    new_node->unique_name = location_name;
    new_node->node_type = type_name;

    // PROCESS UNIQUE DATA

    // create a final table
    sol::table processed_unique_data;
    // get the node type data
    node_type_t node_type = ( *node_search_res ).second;

    // if list, iterate the unique data and set the data in the template to the result
    if ( IsList( unique_data ) ) {
        log_trace("Unique data is list type");

        // copy the template
        processed_unique_data = CopyTable( lua, node_type.unique_data_template );

        // idx stores the index in processed_unique_data
        size_t idx = 1;

        // iterate the data template
        // we need to iterate the template since when trying to index values, names values do not work
        // so, we must think the other way around becauae this gives us the correct keys
        for ( const auto &item : processed_unique_data ) {
            // the key and value from the template
            const auto key = item.first;
            const auto value = item.second;

            log_debug(
                "Got %s=%s while iterating unique data template",
                ObjectToString(key).c_str(),
                ObjectToString(value).c_str()
            );
            
            // the value recieved from the passed-in unique data
            auto recieved_value = unique_data[ idx ];

            log_debug(
                "Recieved unique data, at idx: %d = %s",
                idx,
                ObjectToString(recieved_value).c_str()
            );

            // check if at end
            const bool atEnd = recieved_value == sol::nil;

            // if at the end break the loop
            if ( atEnd ) { 
                log_warn("Got to end of unique data before end of template");
                break;
            }

            // if table, make a copy
            if ( recieved_value.get_type() == sol::type::table ) {
                // create a temporary table to be used as a reference
                sol::table t = recieved_value;
                processed_unique_data[ key ] = CopyTable( lua, t );
            }

            // else do a simple value copy
            else {
                // set the source value to the new value
                processed_unique_data[ key ] = recieved_value;
            }

            // increment the current index to get new data in the passed-in table
            idx++;
        }
    }

    // else, merge the tables based on names values
    else {
        log_trace("Unique data is dictionary-like");

        // combine the tables
        // only overwrite
        processed_unique_data = CombineTable::ToNew(
            lua,
            node_type.unique_data_template,
            unique_data,
            /*
            Won't be added because...
            DEEP - You would be unable to have a list as a parameter
            ADD_NEW - The script would not expect other data
            ENFORCE_TYPES - Why restrict the freedom? The user can type check themselves
            */
            CombineTable::OVERWRITE_EXISTING
        );
    }

#ifdef DEV
    log_debug("The final processed unique data table is as follows:");
    // debugging
    ShowTable(processed_unique_data);
    std::cout << std::endl;
#endif

    // set the unique data to the processed data
    new_node->unique_data = processed_unique_data;
}

int NodeManager::new_node_type(
    std::string type_name,
    sol::function on_land,
    sol::function on_leave,
    sol::table unique_data_template
) {
    log_trace("Called function \"%s( %s, function, function, table )\"",
        __FUNCTION__,
        type_name.c_str()
    );

    // check if the type already exists
    const bool type_exists = all_node_types.find( type_name ) != all_node_types.end();

    if ( type_exists ) {
        log_error(
            "Node type \"%s\" already exists",
            type_name.c_str()
        );

        return 1;
    }
    
    // only create the new type once we know it is valid
    node_type_t new_type = {
        type_name,
        on_land,
        on_leave,
        unique_data_template
    };

    // add the new type to the list
    all_node_types[ type_name ] = new_type;

    log_trace(
        "New node type added: \"%s\"",
        type_name.c_str()
    );

    return 0;
}

// get a node from the environment
node_t *NodeManager::get_node(coordinates_t coords) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        coords_to_str( &coords, true ).c_str()
    );

    // get the hash
    const coord_hash hash = coords.hash;

    // search for the hash
    auto search_res = environment.find(hash);

    // check for existance
    const bool node_found = search_res != environment.end();

    if ( !node_found ) {
        log_error("Node with coordinates %s does not exist",
            coords_to_str( &coords, true ).c_str()
        );

        return NULL;
    }   

    // does exist so get the node * from the pair
    node_t* node = search_res->second;

    log_trace("Node with coordinates %s exists",
        coords_to_str( &coords, true ).c_str()
    );

    // return the found node
    return node;
}

int NodeManager::make_connection(
    coordinates_t node1,
    coordinates_t node2,
    node_directions link,
    bool one_way,
    bool override_blocked
) {
    log_trace("Called function \"%s( %s, %s, %d (%s), %d, %d )\"",
        __FUNCTION__,
        coords_to_str( &node1, true ).c_str(),
        coords_to_str( &node2, true ).c_str(),

        link, dir_to_string( link ).c_str(),

        one_way,
        override_blocked
    );

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

    bool connection_blocked = false;

    // a pointer to the next and previous pointer
    // we want to be able to change the pointer so we need a pointer to a pointer
    node_t **cur = NULL;  // the pointer from the next node to the current node based on the opposite of the direction specified
    node_t **next = NULL;  // pointer from the current node to the next node in the direction specified

    switch ( link ) {
        case NODE_LEFT:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_LEFT) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_RIGHT)
            );

            next = &node1_ptr->left;
            cur = &node2_ptr->right;
            
            break;
        
        case NODE_RIGHT:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_RIGHT) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_LEFT)
            );

            next = &node1_ptr->right;
            cur = &node2_ptr->left;
            
            break;
        
        case NODE_FORWARD:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_FORWARD) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_BACK)
            );

            next = &node1_ptr->forward;
            cur = &node2_ptr->back;
            
            break;
        
        case NODE_BACK:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_BACK) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_FORWARD)
            );

            next = &node1_ptr->back;
            cur = &node2_ptr->forward;
            
            break;

        case NODE_UP:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_UP) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_DOWN)
            );

            next = &node1_ptr->up;
            cur = &node2_ptr->down;
            
            break;
        
        case NODE_DOWN:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_DOWN) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_UP)
            );

            next = &node1_ptr->down;
            cur = &node2_ptr->up;
            
            break;
        
        case NODE_PREV:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_PREV) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_NEXT)
            );

            next = &node1_ptr->previous;
            cur = &node2_ptr->next;
            
            break;

        case NODE_NEXT:
            connection_blocked = (
                is_dir_blocked(node1_ptr->blocked_directions, NODE_NEXT) ||
                is_dir_blocked(node2_ptr->blocked_directions, NODE_PREV)
            );

            next = &node1_ptr->next;
            cur = &node2_ptr->previous;
            
            break;
        
        default:
            break;
    }

    // if there are no pointers then we can't continue processing
    if ( cur == NULL || next == NULL ) {
        log_error("Either the current or next node does not exist. Likely due to invalid direction");
        return 1;
    }

    // only if the connection is blocked and we are overriding the block, we have to return
    if ( connection_blocked && !override_blocked ) {
        log_warn("Connection is blocked");
        return 1;
    }

    // IF EITHER OF NEXT OR CURRENT ARE NOT FREE, DISALLOW
    if ( !( *next == NULL && *cur == NULL ) ) {
        log_warn("A connection is trying to be established but a connection here is already made");
        return 1;
    }

    // the connection from the current to the next gets set
    *next = node2_ptr;

    // if one way, then we don't need to make the other connection
    if ( one_way ) { 
        log_trace("Successfully made one-way connection");
        return 0;
    }

    // if not one way, make the connection from the next node to the current node
    *cur = node1_ptr;

    log_trace("Successfully made connection");
    return 0;
}

int NodeManager::make_all_connections() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

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
                "Trying to make connection between (%d %d %d, %lld) and (%d %d %d, %lld)",
                cur_coords.x, cur_coords.y, cur_coords.z, cur_coords.hash,
                new_coords.x, new_coords.y, new_coords.z, new_coords.hash
            );

            make_connection( cur_coords, new_coords, item.second );
        }
    }

    return 0;
}

TYPE_MAP &NodeManager::get_all_node_types() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    return all_node_types;
}