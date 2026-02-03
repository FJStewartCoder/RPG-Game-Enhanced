#ifndef LUA_ENGINE_CONSTANTS_HPP
#define LUA_ENGINE_CONSTANTS_HPP

#include <string>


// THIS FILE INCLUDES LOTS OF IMPORTANT CONSTANTS FOR NAMES RELATING TO THE LUA ENGINE
// CHANGING THESE WILL MEAN ALL INSTANCES OF THIS IN THE C++ WILL CHANGE
// HOWEVER, THIS MAY CHANGE DATA NAMES IN THE LUA


namespace engine {
    namespace node {
        const std::string TEMPLATE = "NODE_DATA_TEMPLATE";
        const std::string AVAILABLE = "AVAILIBLE_NODES";
        const std::string QUEUE = "NODE_QUEUE";

        const std::string NAME = "name";
        const std::string LAND = "on_land";
        const std::string LEAVE = "on_leave";
    };

    namespace func {
        namespace extension {
            // function that is called that should contain the add_nodes
            const std::string BUILD = "build";
            // function that is called that should extend the player or node
            const std::string EXTEND = "extend";
            // function that is called that should create the environment
            const std::string ENVIRONMENT = "environment";
        };

        namespace api {
            // functions to extend the functionalities
            const std::string ADD_NODE_TYPE = "add_node";
            const std::string EXTEND_NODE = "extend_node";
            const std::string EXTEND_PLAYER = "extend_player";
            
            // functions to environment creation
            const std::string NEW_CAMPAIGN = "new_campaign";
            const std::string BUILD_NODE = "build_node";
            const std::string ARBITRARY_CONNECTION = "make_connection";
        }

        namespace scripts_api {
            // functions bound to menu creation
            const std::string BASIC_MENU = "basic_menu";
            const std::string ALTERNATE_MENU = "alt_menu";
        }
    };

    namespace player {
        // player constants
        const std::string DATA = "PLAYER_DATA";

        const std::string NAME = "name";
        const std::string POSITION = "position";
    };

    namespace file {
        const std::string BUILD = "BUILD_FILE.lua";
        const std::string CAMPAIGN = "CAMPAIGN_FILE.lua";

        const std::string INIT = "INIT.lua";
    };

    namespace settings {
        const std::string CAMPAIGN_NAME = "CAMPAIGN_NAME";
        const std::string USE_GENERIC = "USE_GENERIC";
    }

    namespace directories {
        const std::string CAMPAIGNS = "campaigns";
        const std::string GENERIC = "campaignless";

        const std::string SCRIPTS = "scripts";
    }
};

#endif // LUA_ENGINE_CONSTANTS_HPP