#include "campaign.hpp"

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
#include "settings.h"

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

#include "save.hpp"

#include "input.hpp"

#include "table.hpp"

#include "to_binary.hpp"


// CODE STARTS HERE --------------------------------------------------------------------------

file_metadata read_file_metadata(FILE *fp) {
    log_trace("Called function \"%s( FILE )\"", __FUNCTION__);

    file_metadata res = {
        "",
        0,
        "",
        0
    };

    // buffer to store the current character in
    char c;

    // read the file magic from the file
    for ( int i = 0; i < engine::save::FILE_MAGIC.length(); i++ ) {
        c = fgetc(fp);
        if ( c == EOF ) {
            res.error = 1;
            return res;
        }
        
        res.filemagic += c;
    }

    log_debug("Read file magic \"%s\"", res.filemagic.c_str());
    
    // if the file magic is not present, close the file and error
    if ( res.filemagic != engine::save::FILE_MAGIC ) {
        res.error = 1;
        return res;
    }
    
    // read the file version
    fread(&res.version, sizeof(int), 1, fp);
    log_debug("Read file version %d", res.version);

    // read the campaign name from the file
    if ( Read::TypelessString(fp, res.campaign_name) ) {
        res.error = 1;
        return res;
    }

    log_debug("Read campaign name \"%s\"", res.campaign_name.c_str());

    return res;
}


void Campaign::deleteInit() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    const std::string initVars[5] = {
        engine::func::extension::BUILD,
        engine::func::extension::ENVIRONMENT,
        engine::func::extension::EXTEND,
        engine::settings::CAMPAIGN_NAME,
    };

    for ( const auto &item : initVars ) {
        log_trace("Deleting \"%s\" from build environment", item.c_str());
        build_env[item] = sol::nil;
    }

    log_trace("Successfully deleted all init data from build environment");
}

bool Campaign::initExists(std::string campaignPath) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        campaignPath.c_str()
    );

    const std::string initPath = campaignPath + "/" + engine::file::INIT;
    return std::filesystem::exists(initPath);
}

// function that check for the existance of the init file in a directory
// if it exists, reads and loads the init file of a campaign dir
// loads into a new lua state to prevent issues with running init files

// campaignPath -> the path to the campaign

// returns 0 for good or 1 for bad result
int Campaign::LoadInitSettings(std::string campaignPath) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        campaignPath.c_str()
    );

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
    sol::optional<std::string> campaignName = initFileState[engine::settings::CAMPAIGN_NAME];
    if ( campaignName ) { 
        CAMPAIGN_NAME = campaignName.value();
        log_debug("\"%s\" setting found and set to: %s", engine::settings::CAMPAIGN_NAME.c_str(), CAMPAIGN_NAME.c_str());
    }
    else {
        log_trace("Setting \"%s\" was not found", engine::settings::CAMPAIGN_NAME.c_str());
    }
    
    // if the data exists, then update the options
    sol::optional<sol::table> modules = initFileState[engine::settings::MODULES];
    if ( modules ) {
        log_debug("\"%s\" setting found", engine::settings::MODULES.c_str());

        // empty the array
        MODULES.clear();

        for ( const auto &item : modules.value() ) {
            const std::string mod_name = item.second.as<std::string>();

            MODULES.push_back(mod_name);
            log_debug("Requested module: \"%s\"", mod_name.c_str());
        }
    }
    else {
        log_trace("Setting \"%s\" was not found", engine::settings::MODULES.c_str());
    }

    // return ok
    return 0;
}

int Campaign::RunFunctionIfExists(sol::environment &env, std::string funcName) {
    log_trace("Called function \"%s( env, %s )\"",
        __FUNCTION__,
        funcName.c_str()
    );

    // run process
    if ( has_func(env, funcName) ) {
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
int Campaign::RunInit(std::string campaignPath, int ignore) {
    log_trace("Called function \"%s( %s, %s )\"",
        __FUNCTION__,
        campaignPath.c_str(),
        ToBinary(ignore)
    );

    const std::string initPath = campaignPath + "/" + engine::file::INIT;

    if ( !initExists(campaignPath) ) {
        log_error("Init file does not exist");
        return 1;
    }

    // load the file into the build environment
    int res = load_file(lua, build_env, initPath);

    // if not ok return fail
    if ( res != 0 ) {
        log_error("Failed to load init file");

        // delete init just in case
        deleteInit();

        // return bad
        return 1;
    }

    const bool ignoreExtend = ignore & (int)Ignore::EXTEND;
    const bool ignoreBuild = ignore & (int)Ignore::BUILD;
    const bool ignoreEnvironment = ignore & (int)Ignore::ENVIRONMENT;

    if ( !ignoreExtend ) {

        res = RunFunctionIfExists(build_env, engine::func::extension::EXTEND);
        if ( res != 0 ) {
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
            deleteInit();
            return 1;
        }
    
    }
    else {
        log_trace("Ignoring build function in init file");
    }

    if ( !ignoreEnvironment ) {

        res = RunFunctionIfExists(build_env, engine::func::extension::ENVIRONMENT);
        if ( res != 0 ) {
            deleteInit();
            return 1;
        }

        // make all of the connections
        res = nodeManager.make_all_connections();
        if ( res != 0 ) {
            deleteInit();
            return 1;
        }

    }
    else {
        log_trace("Ignoring environment function in init file");
    }

    log_trace("Successfully ran init file");

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
int Campaign::LoadDirectory( std::string campaignPath, int initIgnore ) {
    log_trace("Called function \"%s( %s, %d )\"",
        __FUNCTION__,
        campaignPath.c_str(),
        initIgnore
    );

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

    // run the init file once all files are loaded in
    int res = RunInit( campaignPath, initIgnore );

    // if we fail to run the init file return 1
    if ( res != 0 ) {
        return 1;
    }

    log_info("Successfully loaded campaign directory: \"%s\"", campaignPath.c_str());

    // return ok
    return 0;
}

// gets all of the campaign names and directories
// returns a map of campaign names : directory location
std::unordered_map<std::string, std::string> Campaign::GetCampaigns() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

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
                continue;
            }

            sol::optional<std::string> check_name = lua[engine::settings::CAMPAIGN_NAME];

            // if the name exists
            if ( check_name ) {
                campaign_name = check_name.value();
                log_trace("init file has campaign name: \"%s\"", campaign_name.c_str());
            }
        }
        // the campaign is not valid if it does not have an init file
        else {
            log_error("Campaign \"%s\" does not have an init file", campaign_name.c_str());
            continue;
        }

        const bool campaign_exists = campaigns.find(campaign_name) != campaigns.end();

        // if the campaign does not already exist, add it to the map
        // else show error message
        if ( !campaign_exists ) {
            log_trace("Adding campaign with name: \"%s\"", campaign_name.c_str());
            
            // map the campaign name to the full file path to the directory
            campaigns[campaign_name] = std::filesystem::canonical(item.path());
        }
        else {
            log_error("Can not add this campaign; another campaign already has this name.");
        }
    }

    return campaigns;
}

int Campaign::LoadCampaign(std::string campaignName) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        campaignName.c_str()
    );

    auto campaigns = GetCampaigns();

    const bool noCampaigns = campaigns.empty();
    if ( noCampaigns ) {
        log_error("No campaigns exist");
        return 1;
    }

    log_trace("Checking if campaign \"%s\" exists", campaignName.c_str());

    const bool campaignExists = campaigns.find(campaignName) != campaigns.end();
    if ( !campaignExists ) {
        log_error("Campaign does not exist");
        return 1;
    }

    log_trace("Campaign \"%s\" exists", campaignName.c_str());

    // write the campaignName loaded to the CAMPAIGN_NAME as backup incase init does not have campaign name
    CAMPAIGN_NAME = campaignName;

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

    // if the script wants modules
    const bool wantsModules = !MODULES.empty();
    if ( wantsModules ) {
        log_trace("Loading %d modules", MODULES.size());

        for ( const auto &module : MODULES ) {
            const std::string modulePath = engine::directories::MODULES + "/" + module;

            log_debug("Loading module \"%s\" with path \"%s\"", module.c_str(), modulePath.c_str());

            res = LoadDirectory( modulePath, (int)Ignore::ENVIRONMENT );

            if ( res != 0 ) {
                log_error("Module \"%s\" failed to load.", module.c_str());
                return 1;
            }
        }
    }

    res = LoadDirectory( campaignPath );
    if ( res != 0 ) {
        log_error("Loading directory failed");
        return 1;
    }

    log_info("Successfully loaded campaign: \"%s\"", campaignName.c_str());
    return 0;
}

int Campaign::SetSavefile(std::string filename) {
    log_trace("Called function \"%s( %s )\"",
        __FUNCTION__,
        filename.c_str()
    );

    if ( filename == "" ) {
        log_error("No filename");
        return 1;
    }

    SAVEFILE = engine::directories::SAVEFILES + "/" + filename;

    return 0;
}

int Campaign::SaveToFile() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    // recusively save the player data table

    if ( SAVEFILE == "" ) {
        log_error("No savefile selected");
        return 1;
    }

    // open a new file for writing
    FILE *fp = fopen(SAVEFILE.c_str(), "wb");

    if ( fp == NULL ) {
        log_error("Unable to write to or make file \"%s\"", SAVEFILE.c_str());
        return 1;
    }

    // write the file magic to the save file
    fputs(engine::save::FILE_MAGIC.c_str(), fp);
    
    // write the version number into the file
    fwrite(&engine::VERSION, sizeof(int), 1, fp);

    Write::TypelessString(fp, CAMPAIGN_NAME);

    Write::Var(fp, engine::player::DATA);
    Write::Table(fp, core_env[engine::player::DATA]);

    // inform the user that the campaign has been saved successfully
    log_info("Successfully saved campaign");

    fclose(fp);
    return 0;
}

int Campaign::LoadFromFile() {
    log_trace("Called function \"%s()\"", __FUNCTION__);

    if ( SAVEFILE == "" ) { 
        log_error("No save file selected");
        return 1;
    }

    // load magic or something
    FILE *fp = fopen(SAVEFILE.c_str(), "rb");

    if ( fp == NULL ) {
        log_error("File \"%s\" does not exist", SAVEFILE.c_str());
        return 1;
    }

    file_metadata metadata = read_file_metadata(fp);
    if ( metadata.error != 0 ) {
        log_error("File metadata is not valid");

        fclose(fp);
        return 1;
    }

    CAMPAIGN_NAME = metadata.campaign_name;

    // load the campaign
    int res = LoadCampaign( CAMPAIGN_NAME );
    if ( res != 0 ) {
        log_error("Campaign did not successfully load");
        return 1;
    }

    // read the table and var
    std::string var;
    char type;

    if ( Read::Var(fp, var) ) {
        fclose(fp);
        return 1;
    }

    if ( Read::Type(fp, type) ) {
        fclose(fp);
        return 1;
    }

    auto player_data = Read::Table(fp, lua);

    if ( player_data.error != 0 ) {
        fclose(fp);
        return 1;
    }

    // get the default table
    sol::table default_table = core_env[var];

    // deep combine whilst presevering types and overwriting defaults and add new
    // add new because inventory system
    const int combination_rules =
        CombineTable::DEEP | CombineTable::PRESERVE_TYPES | CombineTable::OVERWRITE_EXISTING | CombineTable::ADD_NEW_PROPERTIES;

    // combine the tables to the source of default table
    CombineTable::ToSource( lua, default_table, player_data.value, combination_rules );

#ifdef DEV
    // show the player table and final table
    log_debug("The saved table is shown below:");
    ShowTable( player_data.value );
    std::cout << std::endl;

    log_debug("The processed and loaded table is shown below:");
    ShowTable( default_table );
    std::cout << std::endl;
#endif

    fclose(fp);
    return 0;
}

// constructor
Campaign::Campaign() {
    // open libs so we have access to print --------------------------------------------------------------------------------------

    lua.open_libraries(
        sol::lib::base,
        sol::lib::io,
        sol::lib::table,
        sol::lib::math
    );

    // initialise the environments -----------------------------------------------------------------------------------------------

    core_env = sol::environment(lua, sol::create);
    scripts_env = sol::environment(lua, sol::create, lua.globals());  // NEEDS LUA GLOBALS

    const sol::basic_reference scripts_fallback = scripts_env;
    build_env = sol::environment(lua, sol::create, scripts_fallback);  // NEEDS THE FUNCTIONS FROM SCRIPTS SO SET AS FALLBACK

    // inject functions and data into relevant environments  ---------------------------------------------------------------------

    inject_core(core_env);
    inject_build_tools(*this);
    inject_api(*this);

    // ---------------------------------------------------------------------------------------------------------------------------
}

// destructor
Campaign::~Campaign() {
    log_trace("Destructing campaign");
}