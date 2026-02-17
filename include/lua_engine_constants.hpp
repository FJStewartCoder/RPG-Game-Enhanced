#ifndef LUA_ENGINE_CONSTANTS_HPP
#define LUA_ENGINE_CONSTANTS_HPP

#include <string>


// THIS FILE INCLUDES LOTS OF IMPORTANT CONSTANTS FOR NAMES RELATING TO THE LUA ENGINE
// CHANGING THESE WILL MEAN ALL INSTANCES OF THIS IN THE C++ WILL CHANGE
// HOWEVER, THIS MAY CHANGE DATA NAMES IN THE LUA


namespace engine {
    // version 1.0.0
    const unsigned int VERSION = 100;

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
            const std::string BUILD_NODE = "build_node";
            const std::string ARBITRARY_CONNECTION = "make_connection";
        }

        namespace scripts_api {
            // functions bound to menu creation
            const std::string BASIC_MENU = "basic_menu";
            const std::string ALTERNATE_MENU = "alt_menu";

            namespace virtual_events {
                const std::string QUIT = "quit";
            }
        }
    };

    namespace player {
        // player constants
        const std::string DATA = "PLAYER_DATA";

        const std::string NAME = "name";
        const std::string POSITION = "position";
    };

    namespace file {
        const std::string INIT = "INIT.lua";
    };

    namespace settings {
        const std::string CAMPAIGN_NAME = "CAMPAIGN_NAME";
        const std::string USE_GENERIC = "USE_GENERIC";
        const std::string MODULES = "MODULES";
    };

    namespace directories {
        const std::string CAMPAIGNS = "campaigns";
        const std::string GENERIC = "campaignless";

        const std::string MODULES = "modules";

        const std::string SAVEFILES = "savefiles";
    };

    namespace save {
        const std::string FILE_MAGIC = "RPenGineFILE";

        const char NIL = 'n';
        const char STRING = 's';
        const char INT = 'i';
        const char FLOAT = 'f';
        const char BOOLEAN = 'b';
        const char TABLE = 't';
    };
};

enum class VirtualEvents {
    NONE,
    QUIT
};

#endif // LUA_ENGINE_CONSTANTS_HPP