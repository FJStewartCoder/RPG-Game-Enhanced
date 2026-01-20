#ifndef BUILD_HPP
#define BUILD_HPP

#include "nodes.hpp"
#include <vector>

// function to build a node
int build_node(
    std::vector<std::string> node_types,
    std::string node_type,
    sol::table unique_data = sol::table(),
    int previous_node = -1,
    std::string relation = "",  // "" == NODE_NODE
    bool one_way = false  // defines whether or not the new node added should be able to link back to the previous node
);

// get a node from the environment
node_t *get_node(int id);

int free_nodes();

// function that is passed to lua to build new nodes
int new_node_type(sol::environment &core_env, sol::table node_table);

// function to build the node queue
int build_node_queue(sol::environment &core_env, sol::table node_template);

// function to inject the environment tools
int inject_environment_tools(sol::environment &build_env, std::vector<std::string> &node_types);

#endif // BUILD_HPP