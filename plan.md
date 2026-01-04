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