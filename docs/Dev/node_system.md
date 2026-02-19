CURRENT:
Run extend function
    Build player and node extension by adding only new data

Run build function
    add_node
        adds the node table passed in into a list of new node types
        NODE_QUEUE

Build node queue
    Iterates each node_table in NODE_QUEUE and builds it

    build_single_node
        Combines the node_table with the node template
        Adds the currently parsed table to a lua table of available nodes
        Inserts the node name into a C++ list of node_types


Run environment function
    build_node
        check if node type exists
        initialise new node
        set all of the data about the node
        add the node to a hashset of nodes using the coords hash as the hash

Make all connections
    iterate each node and try to make all connections 

NEW:
Run extend function
    Build player extension

Run build function
    new_node_type( string type_name, function on_land, function on_leave, table unique_data_template )
        Initialise a new node_type_t struct with the above data.
        Add the node type to a hashset of nodes where hash is type_name

Run environment function
    build_node( string type, string location_name, coords, unique_data, blocked )
        location_name is used as an individual name of a location ( can be left blank )

        check if node type exists
        initialise new node

        combine unique_data with the node_type unique_data template and copy to new list 

        if unique_data is a list
            assume that the data is incremental
        else
            combine using new combination scripts

        set all of the data about the node
        add the node to a hashset of nodes using the coords hash as the hash

Make all connections