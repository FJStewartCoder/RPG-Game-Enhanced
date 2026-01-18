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
<!-- build needs a seperate env, whereas, because the loaded files CAN NOT have access to the build functions -->

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

