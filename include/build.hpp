#ifndef BUILD_HPP
#define BUILD_HPP

#include "nodes.hpp"
#include <vector>
#include <unordered_set>

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

int free_nodes();

// function that is passed to lua to build new nodes
int new_node_type(sol::environment &core_env, sol::table node_table);

// function to build the node queue
int build_node_queue(sol::environment &core_env, sol::table node_template);

std::unordered_set<std::string> &get_all_node_types();

#endif // BUILD_HPP