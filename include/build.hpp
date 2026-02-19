#ifndef BUILD_HPP
#define BUILD_HPP

#include "nodes.hpp"
#include <vector>
#include <unordered_set>


using NODE_MAP = std::unordered_map<coord_hash, node_t*>;
using TYPE_MAP = std::unordered_map<std::string, node_type_t>;


class NodeManager {
    private:
        // the array of C++ nodes
        NODE_MAP environment;

        // all of the types of nodes which can be used for validation 
        TYPE_MAP all_node_types;

    public:
        // constructor and destructor
        NodeManager();
        ~NodeManager();

        // function to build a node
        void build_node(
            std::string node_type,  // the name of the type
            std::string location_name,  // a name of the specific node ( e.g name's house )
            coordinates_t coords,  // where the node should be placed
            sol::table unique_data,  // a table of data
            std::string blocked_directions = ""  // string of blocked directions e.g "lr" would be left and right is blocked
        );

        // function that creates all of the connections between all of the nodes based on the grid system
        int make_all_connections();

        int make_connection(
            coordinates_t node1,
            coordinates_t node2,
            node_directions link,
            bool one_way = false,
            bool override_blocked = false
        );

        // get a node from the environment
        node_t *get_node(coordinates_t coords);

        // function that is passed to lua to build new nodes
        int new_node_type(
            std::string type_name,  // the name of the type
            sol::function on_land,  // landing function
            sol::function on_leave,  // leaving function
            sol::table unique_data_template  // a template for unique data for this specific node type
        );

        TYPE_MAP &get_all_node_types();
};

node_directions str_to_direction(std::string dir);


#endif // BUILD_HPP