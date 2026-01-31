Everything is Lua based with only minor implementations from the C++ to ensure maximum customisability
Lua will be responsible for player data and the node data split into two categories.
These will be core and additional. Core will have all data required by the game to function and additional will allow for extra functions.

Core node data is as such:
name: string -> used to find the node data from c++

on_land: function( player data, current node ) -> run when landing on this node ( player data and current node data is passed to the function )
on_leave: function( player data, current node ) -> run when leaving this node ( player data and current node data is passed to the function )

The extension folder is setup such that all lua scripts in either node or player will be all combined together to create the whole objects for each.
This will be done by the C++

To combine extension and core, we will have a build function somewhere which will be called to build all of the nodes.
Before we run this, we can combine all of the extensions together. Then, the build will be based on a modified template.

TODO: PREVENT DUPLICATE NODE NAMES

Add a Lua function from C++ that will allow for management of location from scripts.
Will have a piece of data in the player data that will be the player position as an index / node id.
If 0 then the C++ manages location. Else, the C++ will move the player to the node with the corresponding ID and set the position back to 0 to continue management.
The "id" will be the index + 1 since we know the 0 is ignore.

TODO:
The proceduce could be improved such as alternate check times and skipping the move choice if you have been moved.

LOAD STRUCTURE:
Create a lua state

Inject hardcoded core data for player and node
create a C++ queue of sol::tables that could become new node types

Create extension lua state

For each file in scripts dir
    Inject all building and extension functions into state

    - check has build function
    - ...

    - check has extend function
    - ...

    Reset the lua state
end

Iterate through the build queue and make use of the finished extension to verify which nodes are valid

This does not work because things get unloaded.


NEW LOAD STRUCTURE:
Create a lua state

Inject game API functions

For each file in scripts dir
    if file == .lua and not "BUILD_FILE" and not "CAMPAIGN FILE"
        Load file
    end if
end

Inject hardcoded core data for player and node

Load "BUILD_FILE"

This time everything remains loaded and there exists only one build and extend function

CAMPAIGN_FILE is where you can make an environment from lua

SOL ENVIRONMENTS COULD HELP WITH SOME OF THE DIFFERENT FILE THINGS



AIMS:
Have a clean base state that all other environments use for fallback globals

Hide all of the API functions in an API environment

Place all of the loaded files into a seperate environment that can all interact with each other
This state will have fallback to the API state so that we can access the API

Have a seperate environment for build and campaign file which houses their special scripts
This environment will also have fallback to the loaded file state so that it can access those functions

There is potential to also need a core environement where the player data and node data is stored



NEW DIFFERENT LOAD THING AGAIN:
Create base lua state

Create core environment
Inject core data into core

Create loaded files environment with access to base
Inject API functions into load files environment
<!-- This is because the loaded files need the API and do not need to be seperated from the others -->
<!-- whereas, build needs a seperate env because the loaded files CAN NOT have access to the build functions -->

Create build environment
Inject build functions into build

For each file in scripts dir
    if file == .lua and not "BUILD_FILE" and not "CAMPAIGN FILE"
        Load file into test environment
        Test file to ensure it has no globals with the same names as anything in build or already loaded files
        ( Technically can have same names as core because core is abstracted away into another environment )

        if success
            load file into loaded files environment
        else
            fail or throw error
        end if
    end if
end

Load BUILD_FILE ... as before
Load CAMPAIGN_FILE ... as before


**INJECTS:**
CORE:
player and node data

API:
all functions that can be called by scripts

BUILD:
all functions required to build nodes types and campaigns

New Directory Load System:
Have one directory that stores all of the campaigns
Have a special directory that stores scripts that are loaded regardless of campaign
( Toggled by settings in init file )
Each directory will have a single init file ( replaces BUILD_FILE and CAMPAIGN_FILE )
It will be a combination of both files + settings

Each directory in the campaign dir will be its own campaign. The dir name will be the name of the campaign unless specified otherwise in the init file.

Each campaign will be recursively searched up to some specified depth for all .lua files.
Only the initial directory should have an init file. Others will be ignored.

Each campaign name will be saved in a map in the C++ which will allow you to select which one to play.

INIT FILE:
- Build function ( build )
- Extend function ( extend )
- Environment function ( environment )

- Campaign Name ( CAMPAIGN_NAME = string )
- Use Generic Functions ( USE_GENERIC = bool ) ( to either enable or disable the use of the campaignless nodes )

Psuedo Psuedo Code:
map of string to string ( name to directory )

For each file/folder in campaigns dir
    if is file then
        skip or throw error
    end if

    // must be dir
    if dir does not have init file
        fail/skip or throw error
    end if

    if init file has campaign name
        add campaign name to map with dir
    else
        add directory name to map with dir
    end if
end for

campaign_to_play = Ask user for which campaign to play / load a random one for testing

if campaign_to_play still exists
    load process
else
    fail or error
end if

We could use a campaign class to manage all of the correct states for us
We could also just make a struct and initialisers.
...

TODO: NEED TO COME UP WITH A SYSTEM TO BUILD THE FILES USING SEVERAL INIT FILES
COULD EXPERIENCE ISSUES IN WHICH SEVERAL FILES SHARE FUNCTION NAMES

INIT LOADING SYSTEM
IT IS POSSIBLE TO PLACE LOCALLY STORED VARIABLES THAT ARE IN A FUNCTION INTO AN EXTERNAL PLACE WHERE THEY CAN REMAIN EXISTANT AFTER THE FUNCTION IS DESTROYED

SO, WE CAN RUN THE INIT FUNCTIONS THEN DELETE THEM SINCE THE DATA INSIDE WILL NOT BE DELETED

TODO: implement a system to only run the environment function once we have loaded everything else in
This may involve a seperate file again