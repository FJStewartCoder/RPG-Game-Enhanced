#ifndef BUILD_HPP
#define BUILD_HPP

#include "nodes.hpp"
#include <vector>
#include <unordered_set>


using NODE_MAP = std::unordered_map<long long int, node_t*>;


class NodeManager {
    private:
        // the array of C++ nodes
        NODE_MAP environment;

        // all of the types of nodes which can be used for validation 
        std::unordered_set<std::string> all_node_types;

        int build_single_node(
            sol::environment &core_env,
            sol::table node_template,
            sol::table node_table
        );

    public:
        // constructor and destructor
        NodeManager();
        ~NodeManager();

        // function to build a node
        void build_node(
            std::string node_type,
            coordinates_t coords,
            sol::table unique_data = sol::table(),
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
        int new_node_type(sol::environment &core_env, sol::table node_table);

        // function to build the node queue
        int build_node_queue(sol::environment &core_env, sol::table node_template);

        std::unordered_set<std::string> &get_all_node_types();
};

node_directions str_to_direction(std::string dir);


#endif // BUILD_HPP