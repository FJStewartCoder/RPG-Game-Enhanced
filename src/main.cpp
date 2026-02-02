#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// nodes file
#include "nodes.hpp"

// required for the node build scripts
#include "build.hpp"
#include "build_help.hpp"

#include "extension.hpp"

// include log.h as a C lib
extern "C" {
    #include "log/log.h"
}

#include "menus/menus.hpp"

// for all of the constants
#include "lua_engine_constants.hpp"

#include <filesystem>
// required for a BFS
#include <queue>

// inject headers
#include "inject/inject_api.hpp"
#include "inject/inject_core.hpp"
#include "inject/inject_build.hpp"


#define MAX_FILE_SYSTEM_DEPTH 5


// all errors in a namespace
enum class errors {
    OK,
    FILE,
    BUILD,
    EXTEND
};

// equivalent to enum
namespace Ignore {
    int BUILD = 1;
    int EXTEND = 1 << 1;
    int ENVIRONMENT = 1 << 2;
};


class Campaign {
    public:
        // create lua interpreter
        sol::state lua;

        // create and configure environments  --------------------------------------------------------------------------------------------
        sol::environment core_env;
        sol::environment scripts_env;
        sol::environment build_env;

        // -------------------------------------------------------------------------------------------------------------------------------

        // required for management of the current nodes
        NodeManager nodeManager;

        // -------------------------------------------------------------------------------------------------------------------------------

    private:
        // settings ----------------------------------------------------------------------------------------------------------------------

        // just the name
        std::string CAMPAIGN_NAME = "";

        // whether or not use the generic directory
        bool USE_GENERIC = false;

        // -------------------------------------------------------------------------------------------------------------------------------

        // deletes all of the init variables and data from the build environment
        void deleteInit() {
            const std::string initVars[5] = {
                engine::func::extension::BUILD,
                engine::func::extension::ENVIRONMENT,
                engine::func::extension::EXTEND,
                engine::settings::CAMPAIGN_NAME,
                engine::settings::USE_GENERIC
            };

            for ( const auto &item : initVars ) {
                log_trace("Deleting \"%s\" from build environment", item.c_str());
                build_env[item] = sol::nil;
            }

            log_trace("Successfully deleted all init data from build environment");
        }

        bool initExists(std::string campaignPath) {
            const std::string initPath = campaignPath + "/" + engine::file::INIT;
            return std::filesystem::exists(initPath);
        }

        // function that check for the existance of the init file in a directory
        // if it exists, reads and loads the init file of a campaign dir
        // loads into a new lua state to prevent issues with running init files

        // campaignPath -> the path to the campaign

        // returns 0 for good or 1 for bad result
        int LoadInitSettings(std::string campaignPath) {
            log_trace("Loading init settings");

            sol::state initFileState;

            const std::string initPath = campaignPath + "/" + engine::file::INIT;

            log_debug("Init file path to test is \"%s\"", initPath.c_str());

            if ( !initExists(campaignPath) ) {
                log_error("Init file does not exist");
                return 1;
            }

            // load the file into the new state
            int res = load_file(initFileState, initPath);

            // if not ok return fail
            if ( res != 0 ) {
                log_error("Init file failed to load");

                // return bad
                return 1;
            } 

            // extract data out of the init file

            // if the data exists, then update the options
            sol::optional<bool> useGeneric = initFileState[engine::settings::USE_GENERIC];
            if ( useGeneric ) { 
                USE_GENERIC = useGeneric.value();
                log_debug("\"%s\" setting found and set to: %d", engine::settings::USE_GENERIC.c_str(), USE_GENERIC);
            }
            else {
                log_trace("Setting \"%s\" was not found", engine::settings::USE_GENERIC.c_str());
            }

            // if the data exists, then update the options
            sol::optional<std::string> campaignName = initFileState[engine::settings::CAMPAIGN_NAME];
            if ( campaignName ) { 
                CAMPAIGN_NAME = campaignName.value();
                log_debug("\"%s\" setting found and set to: %s", engine::settings::CAMPAIGN_NAME.c_str(), CAMPAIGN_NAME.c_str());
            }
            else {
                log_trace("Setting \"%s\" was not found", engine::settings::CAMPAIGN_NAME.c_str());
            }
            
            // return ok
            return 0;
        }

        int RunFunctionIfExists(sol::environment &env, std::string funcName) {
            log_debug("Checking for function \"%s\"", funcName.c_str());

            // run process
            if ( has_func(env, funcName) ) {
                log_trace("Function \"%s\" exists", funcName.c_str());

                sol::protected_function func = env[funcName];

                auto res = func();
                if ( !res.valid() ) {
                    // print error and continue to failure
                    sol::error e = res;
                    log_error("%s function ran with error:\n%s", funcName.c_str(), e.what());

                    return 1;
                }               
            }
            else {
                log_error("Function does not exists \"%s\"", funcName.c_str());
                return 1;
            }

            log_trace("Function \"%s\" ran successfully", funcName.c_str());
            return 0;
        }

        // function that checks for the expected functions and runs them
        // this then deletes the reference to the function to allow more inits to be run

        // campaignDir -> a directory iterator
        // ignore -> an integer which represents which functions to ignore based on the Ignore enum class.

        // returns 0 for good or 1 for bad result
        int RunInit(std::string campaignPath, int ignore = 0) {
            log_trace("Running init file");

            const std::string initPath = campaignPath + "/" + engine::file::INIT;

            if ( !initExists(campaignPath) ) {
                log_error("Init file does not exist");
                return 1;
            }

            // load the file into the build environment
            int res = load_file(lua, build_env, initPath);

            // if not ok return fail
            if ( res != 0 ) {
                log_error("Failed to load init file. Deleting init data");

                // delete init just in case
                deleteInit();

                // return bad
                return 1;
            }

            const bool ignoreExtend = ignore & Ignore::EXTEND;
            const bool ignoreBuild = ignore & Ignore::BUILD;
            const bool ignoreEnvironment = ignore & Ignore::ENVIRONMENT;

            if ( !ignoreExtend ) {

                res = RunFunctionIfExists(build_env, engine::func::extension::EXTEND);
                if ( res != 0 ) {
                    log_error("Extend function failed");

                    deleteInit();
                    return 1;
                }

            }
            else {
                log_trace("Ignoring extend function in init file");
            }

            if ( !ignoreBuild ) {

                res = RunFunctionIfExists(build_env, engine::func::extension::BUILD);
                if ( res != 0 ) {
                    log_error("Build function failed");

                    deleteInit();
                    return 1;
                }
            
            }
            else {
                log_trace("Ignoring build function in init file");
            }

            if ( !ignoreEnvironment ) {

                // build the nodes
                // MUST BE PERFORMED BEFORE RUNNING THE ENVIRONMENT FUNCTION
                log_trace("Building node queue");
                nodeManager.build_node_queue(core_env, core_env[engine::node::TEMPLATE]);

                res = RunFunctionIfExists(build_env, engine::func::extension::ENVIRONMENT);
                if ( res != 0 ) {
                    log_error("Environment function failed");

                    deleteInit();
                    return 1;
                }

                log_trace("Making all connections between nodes");

                // make all of the connections
                nodeManager.make_all_connections();

            }
            else {
                log_trace("Ignoring environment function in init file");
            }

            log_info("Successfully ran init file");

            // delete all init data from the build env to prevent being overwritten later
            // or causing issues where the same function is run several times
            deleteInit();
            return 0;
        }

        // recursive function that loads a directory into the lua states and environments
        // then, builds this directory based on the init file in the directory

        // campaignDir -> a directory iterator
        // initIgnore -> a pass through to the run init function ( used to optionally ignore certain functions )

        // returns 0 for good or 1 for bad result
        int LoadDirectory( std::string campaignPath, int initIgnore = 0 ) {
            // ignore typename
            std::filesystem::__cxx11::directory_iterator campaignsDir;

            // create a directory iterator to iterate through the files and directories
            try {
                campaignsDir = std::filesystem::directory_iterator(campaignPath);
            }
            catch ( const std::filesystem::__cxx11::filesystem_error &error ) {
                log_error("Campaigns directory \"%s\" does not exist.", campaignPath.c_str());
                return 1;
            }

            log_trace("Campaign directory \"%s\" exists", campaignPath.c_str());
            
            // if there is no init file we fail
            if ( !initExists( campaignPath ) ) {
                log_error("Init file does not exist for this campaign");
                return 1;
            }

            log_trace("Init file found");

            // used to store the path then the current depth
            using dirPair = std::pair<std::string, int>;

            log_trace("Beginning file search");

            // use BFS to load all files and directories up to a certain depth
            std::queue<dirPair> loadQueue;

            // queue the main directory
            loadQueue.push( dirPair(campaignPath, 0) );

            // while the queue is not empty keep loading
            while ( loadQueue.size() >= 1 ) {

                const dirPair currentDir = loadQueue.front();
                loadQueue.pop();

                log_trace("Current directory is %s", currentDir.first.c_str());

                // create a directory iterator from the first item in the pair
                auto curIter = std::filesystem::directory_iterator( currentDir.first );

                for ( const auto &file : curIter ) {
                    log_trace("Entry \"%s\" has been found", file.path().c_str());

                    // if directory, queue the item's path with depth as current + 1 unless we are at the max depth
                    if ( file.is_directory() ) {
                        
                        // file system depth validation
                        const bool atMaxDepth = currentDir.second >= MAX_FILE_SYSTEM_DEPTH;
                        if ( atMaxDepth ) {
                            log_warn("Entry has exceeded the max directory depth and will not be processed");
                            continue;
                        }

                        // get the directory path
                        const std::string dirpath = std::filesystem::canonical( file );

                        // queue the item
                        dirPair newItem(dirpath, currentDir.second + 1);
                        loadQueue.push(newItem);

                        log_debug("Entry queued with path=\"%s\", depth=%d", newItem.first.c_str(), newItem.second);

                    }

                    // if is lua file, load the file into the scripts environment
                    // file is file, is lua and is not the init file
                    else if ( file.is_regular_file() &&
                              ( file.path().extension() == ".lua" ) &&
                              ( file.path().filename() != engine::file::INIT ) 
                    ) {

                        // get the file path and load it
                        const std::string filepath = std::filesystem::canonical( file );

                        log_trace("File \"%s\" is being loaded as a script", filepath.c_str());

                        const bool valid = load_file(lua, scripts_env, filepath) == 0;

                        if ( !valid ) { log_warn("File did not successfully load."); }
                        else { log_trace("File loaded successfully"); }

                    }
                    else {
                        log_warn("File \"%s\" has not been processed", file.path().c_str());
                    }

                }

            }

            log_trace("Running init file");

            // run the init file once all files are loaded in
            int res = RunInit( campaignPath, initIgnore );

            // if we fail to run the init file return 1
            if ( res != 0 ) {
                log_error("Init file failed to execute");
                return 1;
            }

            log_info("Successfully loaded campaign directory: \"%s\"", campaignPath.c_str());

            // return ok
            return 0;
        }

    public:
        // gets all of the campaign names and directories
        // returns a map of campaign names : directory location
        static std::unordered_map<std::string, std::string> GetCampaigns() {
            std::unordered_map<std::string, std::string> campaigns;

            // ignore typename
            std::filesystem::__cxx11::directory_iterator campaigns_dir;

            try {
                campaigns_dir = std::filesystem::directory_iterator(engine::directories::CAMPAIGNS);
            }
            catch ( const std::filesystem::__cxx11::filesystem_error &error ) {
                log_error("Campaigns directory does not exist.");
                return campaigns;
            }

            log_trace("Campaigns directory found.");

            for ( const auto &item : campaigns_dir ) {
                const std::string cur_filename = item.path().filename();

                log_trace("Analysing \"%s\"", cur_filename.c_str());

                if ( !item.is_directory() ) {
                    log_trace("%s is not a directory", cur_filename.c_str());
                    continue;
                }

                const std::string init_file_path = item.path().generic_string() + "/" + engine::file::INIT;

                log_trace("Searching for init file: \"%s\"", init_file_path.c_str());

                const bool campaign_has_init = std::filesystem::exists(init_file_path);

                // the current campaign name is the name of the directory
                std::string campaign_name = item.path().filename();

                if ( campaign_has_init ) {
                    log_trace("Campaign has init file.");

                    sol::state lua;

                    int res = load_file(lua, init_file_path);

                    const bool file_success = res == 0;

                    // jump out of the if statement since the file is broken
                    if ( !file_success ) {
                        log_error("init file is not valid.");
                        goto add_file;
                    }

                    sol::optional<std::string> check_name = lua[engine::settings::CAMPAIGN_NAME];

                    // if the name exists
                    if ( check_name ) {
                        campaign_name = check_name.value();
                        log_trace("init file has campaign name: \"%s\"", campaign_name);
                    }
                }
                // the campaign is not valid if it does not have an init file
                else {
                    log_error("Campaign \"%s\" does not have an init file", campaign_name.c_str());
                    continue;
                }

                // goto for escaping the if block
                add_file:

                const bool campaign_exists = campaigns.find(campaign_name) != campaigns.end();

                // if the campaign does not already exist, add it to the map
                // else show error message
                if ( !campaign_exists ) {
                    log_info("Adding campaign with name: \"%s\"", campaign_name.c_str());
                    
                    // map the campaign name to the full file path to the directory
                    campaigns[campaign_name] = std::filesystem::canonical(item.path());
                }
                else {
                    log_error("Can not add this campaign; another campaign already has this name.");
                }
            }

            return campaigns;
        }

        int LoadCampaign(std::string campaignName) {
            log_trace("Getting campaigns");
            auto campaigns = GetCampaigns();
            
            log_trace("Checking if campaign \"%s\" exists", campaignName.c_str());

            const bool campaignExists = campaigns.find(campaignName) != campaigns.end();
            if ( !campaignExists ) {
                log_error("Campaign does not exist");
                return 1;
            }

            log_trace("Campaign \"%s\" exists", campaignName.c_str());

            // get the campaign path
            const std::string campaignPath = campaigns[campaignName];

            log_debug("Campaign path is: \"%s\"", campaignPath.c_str());
    
            // recursively search that directory to load all of the data in to the environements
            const auto campaignDir = std::filesystem::directory_iterator(campaignPath);

            // load the main campaign dir
            log_trace("Loading Init settings");

            int res = LoadInitSettings( campaignPath );
            if ( res != 0 ) {
                log_error("Loading init settings failed");
                return 1;
            }

            // if we want to use the generic functions, load them in
            if ( USE_GENERIC ) {
                log_trace("Campaign uses general functions. Loading generic functions");

                res = LoadDirectory( engine::directories::GENERIC, Ignore::ENVIRONMENT );

                if ( res != 0 ) {
                    log_error("Generic directory failed to load");
                    return 1;
                }
            }

            log_trace("Loading \"%s\"", campaignPath.c_str());
            res = LoadDirectory( campaignPath );
            if ( res != 0 ) {
                log_error("Loading directory failed");
                return 1;
            }

            log_info("Successfully loaded campaign: \"%s\"", campaignName.c_str());
            return 0;
        }

        // constructor
        Campaign() {
            // open libs so we have access to print --------------------------------------------------------------------------------------

            lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table);

            // initialise the environments -----------------------------------------------------------------------------------------------

            core_env = sol::environment(lua, sol::create);
            scripts_env = sol::environment(lua, sol::create, lua.globals());  // NEEDS LUA GLOBALS

            const sol::basic_reference scripts_fallback = scripts_env;
            build_env = sol::environment(lua, sol::create, scripts_fallback);  // NEEDS THE FUNCTIONS FROM SCRIPTS SO SET AS FALLBACK

            // inject functions and data into relevant environments  ---------------------------------------------------------------------

            inject_core(core_env);
            inject_build_tools(build_env, core_env, nodeManager);
            inject_api(scripts_env);

            // ---------------------------------------------------------------------------------------------------------------------------
        }

        // destructor
        ~Campaign() {
            log_trace("Destructing campaign");
        }
};

// could return none
sol::optional<sol::table> get_node_data(sol::environment &core_env, std::string name) {
    sol::table node_options = core_env[engine::node::AVAILABLE];

    // currently is empty until we find the table
    sol::optional<sol::table> found_table;

    // iterate the list of nodes and check if the node has the name that we are looking for
    for ( const auto &node : node_options ) {
        auto node_table = node.second.as<sol::optional<sol::table>>();

        // skip if the node table does not exist as a table or is not a valid table/
        if ( !node_table ) {
            continue;
        }

        // if the names match, set found to the current table and break
        if ( node_table.value()[engine::node::NAME] == name ) {
            found_table = node_table;
            break;
        }
    }

    // return no matter what is found
    return found_table;
}

node_directions get_player_input(node_t *node) {
    Menu menu("", "Select a direction");

    // get each option
    if ( node->left != nullptr ) { menu.AddItem("left"); }
    if ( node->right != nullptr ) { menu.AddItem("right"); }
    if ( node->up != nullptr ) { menu.AddItem("up"); }
    if ( node->down != nullptr ) { menu.AddItem("down"); }
    if ( node->forward != nullptr ) { menu.AddItem("forward"); }
    if ( node->back != nullptr ) { menu.AddItem("back"); }
    if ( node->next != nullptr ) { menu.AddItem("next"); }
    if ( node->previous != nullptr ) { menu.AddItem("previous"); }

    menu.AddItem("quit");
    
    std::string input = menu.ShowAlt();

    log_trace("Menu returned: %s.", input.c_str());

    // if block again for correct return
    if ( input == "left" ) { return NODE_LEFT; }
    else if ( input == "right" ) { return NODE_RIGHT; }
    else if ( input == "up" ) { return NODE_UP; }
    else if ( input == "down" ) { return NODE_DOWN; }
    else if ( input == "forward" ) { return NODE_FORWARD; }
    else if ( input == "back" ) { return NODE_BACK; }
    else if ( input == "next" ) { return NODE_NEXT; }
    else if ( input == "previous" ) { return NODE_PREV; }

    return NODE_QUIT;
}

// TODO: fix script handled movement for new system
void gameloop(Campaign &campaign, node_t *(&start_node)) {
    sol::environment &core_env = campaign.core_env;

    // reassign the name
    node_t *cur_node = start_node;

    // the running state
    bool running = true;

    // boolean to silence the stuck message
    bool silence_stuck = false;

    uint stuck_count = 0;
    const uint max_stuck_cycles = 100;

    // STEPS:
    // get node data
    // on land ( ensures that the start activates )
    // check if possible to move
    // ask direction ( if we did this first the start would not work )
    // on leave
    // traverse

    while ( running ) {
        // get the player data table
        sol::table player_data = core_env[engine::player::DATA];

        // stores the current location of the player
        int script_player_pos = 0;  // player_data[engine::player::POSITION]["x"];

        // check if the script is attempting to manage the player's position
        if ( script_player_pos >= 1 ) {
            try {
                // TODO: fix this
                node_t *new_pos = campaign.nodeManager.get_node({0, 0, 0});

                // set the current node to be the pointer to the new position
                cur_node = new_pos;

                // message to inform me the script moved the player
                log_trace("Script moved player to node ID: %lld.", get_coords_hash(cur_node->coords));
            }
            catch (std::exception &e) {
                log_warn("Script attempted to manage position but the operation failed.");
            }
        }

        // regardless of attempt, this should be 0
        // set the script location to 0 to ensure that we know C++ is managing position
        // TODO: fix later
        // player_data[engine::player::POSITION]["x"] = 0;

        // get the current node data
        auto cur_node_data = get_node_data(core_env, cur_node->node_type);

        log_info("Landed on node with type: \"%s\", with ID: %lld", cur_node->node_type.c_str(), get_coords_hash(cur_node->coords));
    
        // if data exists run on land
        if ( cur_node_data ) {
            sol::protected_function on_land = cur_node_data.value()[engine::node::LAND];

            // pass in the unique data, the node data and the player data
            auto res = on_land(cur_node->unique_data, cur_node_data.value(), player_data);

            if ( !res.valid() ) {
                log_error("Landing function failed.");

                // error details
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // ask direction
        node_directions chosen_direction = NODE_NONE;

        if ( can_traverse(cur_node) == NODE_ERROR ) {
            stuck_count++;

            // user input for quitting or not
            bool will_quit = false;

            if ( stuck_count >= max_stuck_cycles ) {
                log_info("Exceeded max stuck limit. Automatically exiting.");
                std::cout << "[ERROR] You have been stuck longer than the stuck limit. Aborting..." << std::endl;
                
                // force quit
                will_quit = true;
                silence_stuck = true;
            }

            log_error("Player is stuck. Count: %d/%d", stuck_count, max_stuck_cycles);

            if ( !silence_stuck ) {
                std::cout << "[ERROR] You may be stuck as you are unable to move. It may be possible that external scripts are handling movement." << std::endl;
            
                std::cout << "Type q to quit or s to silence stuck message: ";
                // get use input
                std::string input;
                std::cin >> input;

                for ( const auto &c : input ) {
                    if ( c == 'q' ) {
                        will_quit = true;
                        break;
                    }
                    else if ( c == 's' ) {
                        silence_stuck = true;
                    }
                }
            } 
            
            // quit
            if ( will_quit ) {
                running = false;
                break;
            }

            // will not run any further until can move or quit
            continue;
        }

        // if we escape, then reset the stuck counter
        stuck_count = 0;

        // get the direction that the player want to move.
        chosen_direction = get_player_input(cur_node);

        // quit if chosen to quit
        if ( chosen_direction == NODE_QUIT ) {
            running = false;
            break;
        }

        // if data exists run on leave
        if ( cur_node_data ) {
            sol::protected_function on_leave = cur_node_data.value()[engine::node::LEAVE];

            // pass in the unique data, the node data and the player data
            auto res = on_leave(cur_node->unique_data, cur_node_data.value(), player_data);

            if ( !res.valid() ) {
                log_error("Leaving function failed.");

                // error message
                sol::error error = res;
                log_debug("\n%s", error.what());
            }
        }

        // actually traverse
        node_errors res = traverse_node(cur_node, chosen_direction);

        // if there is an error when attempting to log
        if ( res == NODE_ERROR ) {
            log_error("Node traversal failed.");
        }
    }
}


void new_campaign() {
    Menu menu("Campaign Selection");

    auto campaigns = Campaign::GetCampaigns();

    const bool empty = campaigns.size() == 0;

    // if there are no campaigns return to main menu
    if ( empty ) {
        log_error("No campaigns availible.");
        return;
    }

    for ( const auto &item : campaigns ) {
        menu.AddItem(item.first);
    }

    menu.ShowStandard();
}


int main_menu() {
    Menu menu("Main Menu", "Welcome to the game!");

    menu.AddItem("New Campaign");
    menu.AddItem("Load Campaign");
    menu.AddItem("Quit");

    std::string res = menu.ShowStandard();

    // log the option the user chose
    log_trace("User selected %s.", res.c_str());

    // NEW CAMPAIGN
    if ( res == "New Campaign" ) {
        new_campaign();
    }
    else if ( res == "Load Campaign" ) {

    }
    else if ( res == "Quit" ){

    }
    else {
        log_fatal("Whoever wrote the code to allow this is bad.");
    }

    return 0;
}


int main() {
    // open the log file
    FILE *fp = fopen("log.txt", "w");

    // will always log
    // log_add_fp(fp, 0);
    // log_set_quiet(true);

    Campaign campaign;
    Campaign campaign2;
    Campaign campaign3;

    campaign.LoadCampaign( "test_campaign" );
    campaign2.LoadCampaign( "Hello123" );

    // -------------------------------------------------------------------------------------------------------------------------------

    // test if the data is found from the injection
    std::cout << campaign.core_env[engine::node::TEMPLATE][engine::node::NAME].get<std::string>() << std::endl;
    std::cout << campaign.core_env[engine::player::DATA][engine::player::NAME].get<std::string>() << std::endl;

    // test a function
    campaign.core_env[engine::node::TEMPLATE][engine::node::LAND]();

    // get all keys and values for template debug
    /* for ( const auto &t : lua[engine::node::TEMPLATE].get<sol::table>() ) {
        std::cout << "Node template has " << t.first.as<std::string>() << " with value type " << (int)t.second.get_type() << std::endl;
    } */

    // show nodes
    for ( const auto &node : campaign.nodeManager.get_all_node_types() ) {
        log_trace("Found node with name \"%s\"", node.c_str());
    }

    // build test
    /*
    sol::table start_table = lua.create_table();
    start_table["data1"] = "123";

    sol::table start_table2 = lua.create_table();
    start_table2["data1"] = "234";
    */

    /*
    int node1 = build_node("Start", start_table);
    int node2 = build_node("2", sol::table(), node1, NODE_RIGHT, false);
    int node3 = build_node("Start", start_table2, node2, NODE_UP, false);

    // traversal test
    node_t *cur = get_node(node1);

    int res = traverse_node(cur, NODE_RIGHT);
    int res2 = traverse_node(cur, NODE_LEFT);

    std::cout << res << " " << res2 << std::endl;
    */

    // main_menu();

    node_t *cur = campaign.nodeManager.get_node({0, 0, 0});

    if ( cur == NULL ) {
        fclose(fp);
        return 1;
    }

    // the main game loop
    gameloop(campaign, cur);

    // test a second campaign
    cur = campaign2.nodeManager.get_node({0, 0, 0});

    if ( cur == NULL ) {
        fclose(fp);
        return 1;
    }

    // the main game loop
    gameloop(campaign2, cur);


    fclose(fp);
    return 0;
}