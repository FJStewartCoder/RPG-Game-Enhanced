#ifndef CAMPAIGN_HPP
#define CAMPAIGN_HPP

#include <iostream>

#include "sol/sol.hpp"

#include "build.hpp"
#include "lua_engine_constants.hpp"
#include "save.hpp"


enum class Ignore {
    BUILD = 1,
    EXTEND = 1 << 1,
    ENVIRONMENT = 1 << 2
};

typedef struct {
    std::string filemagic;
    unsigned int version;
    std::string campaign_name;
    int error;
} file_metadata;


file_metadata read_file_metadata(FILE *fp);


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

        VirtualEvents event = VirtualEvents::NONE;

        // -------------------------------------------------------------------------------------------------------------------------------

    private:
        // settings ----------------------------------------------------------------------------------------------------------------------

        // just the name
        std::string CAMPAIGN_NAME = "";

        // list of module names to be loaded
        std::vector<std::string> MODULES;

        // the name of the savefile
        // if blank, no file is selected
        std::string SAVEFILE = "";

        // -------------------------------------------------------------------------------------------------------------------------------

        // deletes all of the init variables and data from the build environment
        void deleteInit();

        bool initExists(std::string campaignPath);

        // function that check for the existance of the init file in a directory
        // if it exists, reads and loads the init file of a campaign dir
        // loads into a new lua state to prevent issues with running init files

        // campaignPath -> the path to the campaign

        // returns 0 for good or 1 for bad result
        int LoadInitSettings(std::string campaignPath);

        int RunFunctionIfExists(sol::environment &env, std::string funcName);

        // function that checks for the expected functions and runs them
        // this then deletes the reference to the function to allow more inits to be run

        // campaignDir -> a directory iterator
        // ignore -> an integer which represents which functions to ignore based on the Ignore enum class.

        // returns 0 for good or 1 for bad result
        int RunInit(std::string campaignPath, int ignore = 0);

        // recursive function that loads a directory into the lua states and environments
        // then, builds this directory based on the init file in the directory

        // campaignDir -> a directory iterator
        // initIgnore -> a pass through to the run init function ( used to optionally ignore certain functions )

        // returns 0 for good or 1 for bad result
        int LoadDirectory( std::string campaignPath, int initIgnore = 0 );

    public:
        // gets all of the campaign names and directories
        // returns a map of campaign names : directory location
        std::unordered_map<std::string, std::string> GetCampaigns();

        int SetSavefile(std::string filename);

        int SaveToFile();

        int LoadCampaign(std::string campaignName);
        int LoadFromFile();

        // constructor
        Campaign();

        // destructor
        ~Campaign();
};

#endif  // CAMPAIGN_HPP