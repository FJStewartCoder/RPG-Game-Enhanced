#include "inject_core.hpp"

int inject_core_node_data(sol::environment &env) {
    // create a new table with the following data
    env[engine::node::TEMPLATE] = env.create_with(
        engine::node::NAME, "Node Name",
        engine::node::LAND, []() {},
        engine::node::LEAVE, []() {}
    );

    // create empty list for the availible nodes
    env[engine::node::AVAILABLE] = env.create();

    // persitently keep the new nodes in lua space
    env[engine::node::QUEUE] = env.create();

    return 0;
}

int inject_core_player_data(sol::environment &env) {
    env[engine::player::DATA] = env.create_with(
        engine::player::NAME, "Player Name",
        engine::player::POSITION, env.create_with(
            "x", 0,
            "y", 0,
            "z", 0
        )
    );

    return 0;
}

int inject_core(sol::environment &env) {
    inject_core_node_data(env);
    inject_core_player_data(env);

    return 0;
}