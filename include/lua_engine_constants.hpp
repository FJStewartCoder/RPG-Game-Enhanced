#ifndef LUA_ENGINE_CONSTANTS_HPP
#define LUA_ENGINE_CONSTANTS_HPP

#include <string>


// THIS FILE INCLUDES LOTS OF IMPORTANT CONSTANTS FOR NAMES RELATING TO THE LUA ENGINE
// CHANGING THESE WILL MEAN ALL INSTANCES OF THIS IN THE C++ WILL CHANGE
// HOWEVER, THIS MAY CHANGE DATA NAMES IN THE LUA


namespace engine {
    // version 1.0.8
    const unsigned int VERSION = 108;

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
            const std::string ADD_NODE_TYPE = "new_node_type";
            const std::string EXTEND_PLAYER = "extend_player";

            // functions to environment creation
            const std::string BUILD_NODE = "build_node";
            const std::string ARBITRARY_CONNECTION = "make_connection";
        };

        namespace scripts_api {
            // functions bound to menu creation
            const std::string BASIC_MENU = "basic_menu";
            const std::string ALTERNATE_MENU = "alt_menu";

            namespace virtual_events {
                const std::string QUIT = "quit";
            };

            namespace table {
                const std::string COPY = "copy_table";
                const std::string SHOW = "show_table";
                const std::string MERGE = "merge_table";
            };

            namespace node {
                const std::string GET_NODE = "get_node";
            };
        };
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
        const std::string MODULES = "MODULES";
    };

    namespace directories {
        const std::string CAMPAIGNS = "campaigns";

        const std::string MODULES = "modules";

        const std::string SAVEFILES = "savefiles";
    };

    namespace save {
        const std::string FILE_MAGIC = "RPenGineFILE";

        const char VARIABLE = 'v';
        const char NIL = 'n';
        const char STRING = 's';
        const char INT = 'i';
        const char FLOAT = 'f';
        const char BOOLEAN = 'b';
        const char TABLE = 't';
    };

    // gets the version as a string
    const std::string version_string();

    // check is ver is compatible with the current version
    const bool is_version_compatible(int ver);
};

enum class VirtualEvents {
    NONE,
    QUIT
};

#endif // LUA_ENGINE_CONSTANTS_HPP