Randomly Generating Single Player RPG

Simple game making use of nodes that allow you to navigate through a randomly generating world.

Different types of nodes such as fights, locations, treasure, rest, etc.

Could have different preset environments that have different win conditions and possible nodes. For example, desert, village, mountain etc.

Available directions will be up, down, left, right, forwards, backwards, next and previous. Next and previous allow for not travelling along the expected grid such as teleporting.

Nodes will be set out as such:

ExampleNodeData { foo: bar bar: foo }

// possibility to include and action function pointer that takes args of player data that can begin new sequences etc. (returns void probably)

// makes fights etc easier without the use of a union and easier to modify

NodeData { msg: String (will be shown when arriving at this node)

type: NodeType enum

union { Ptr to each type of nodes } }

Node { id: Unique id or coords to know of visited here

data: NodeData All directions: ptr to node }

We also need player data to save spaces visited etc.

We can use Lua also to be able to easily modify the game and add new node types.

We need a lua accessible function that will allow for extending the main node data table with a new table. Same for player data. These can be run in a separate lua state to prevent any issues involved with variable overriding.

New build system: Each new node will be malloced then a pointer to it will be added to a vector. This will replace the system where it returns the node. The ptr to prev will be replaced by the index in the vector. This also allows to be used from lua

Extensions: Create c++ function bound to lua that allows for extending the player and node data instead of attempting to read random data from random files.

E.g extend_node_data(sol::table extension)

Todo: Check each script file to ensure it does not have a function with the name matching the extend or build function. Same for the API functions.

Ensure that files do not have variables and functions with the same name as other files.

Add main menu

Add more API functions

Allow for connecting existing nodes to other nodes since you may want to connect pre-existing nodes

Script directories: There could be a feature involving specific campaigns based around single implementations. Instead of loading everything, load only that directory and consider it to be its own campaign rather than combining everything into one. However, there could be a campaign-less directory of pure extensions that don’t have any related campaign. Such as: Node type fight. This can be used across many campaigns and will be loaded alongside all campaigns unless a special flag is set to only include the specific directory data. (That directory is all that is needed for that story)


TODO: EMPTY STRINGS DON'T SAVE AND LOAD CORRECTLY