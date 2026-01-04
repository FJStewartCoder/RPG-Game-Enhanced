Everything is Lua based with only minor implementations from the C++ to ensure maximum customisability
Lua will be responsible for player data and the node data split into two categories.
These will be core and additional. Core will have all data required by the game to function and additional will allow for extra functions.

Core node data is as such:
name: string -> used to find the node data from c++

on_land: function( player data ) -> run when landing on this node ( player data is passed to the function )
on_leave: function( player data ) -> run when leaving this node ( player data is passed to the function )