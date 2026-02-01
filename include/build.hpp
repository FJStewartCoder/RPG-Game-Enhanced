#ifndef BUILD_HPP
#define BUILD_HPP

#include "nodes.hpp"
#include <vector>
#include <unordered_set>


class NodeManager {
    private:
        // the array of C++ nodes
        std::vector<node_t*> environment;

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
        int build_node(
            std::string node_type,
            sol::table unique_data = sol::table(),
            int previous_node = -1,
            std::string relation = "",  // "" == NODE_NODE
            bool one_way = false  // defines whether or not the new node added should be able to link back to the previous node
        );

        // get a node from the environment
        node_t *get_node(int id);

        // function that is passed to lua to build new nodes
        int new_node_type(sol::environment &core_env, sol::table node_table);

        // function to build the node queue
        int build_node_queue(sol::environment &core_env, sol::table node_template);

        std::unordered_set<std::string> &get_all_node_types();
};

node_directions str_to_direction(std::string dir);


#endif // BUILD_HPP