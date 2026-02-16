# Player Data
## Core Data
``` lua
table PLAYER_DATA = {
    string name = "Player Name",
    table position = {
        short x = 0,
        short y = 0,
        short z = 0
    }
}
```  
**name** is the player's name.  
**position** allows for location sync between the engine and the scripts.

Only the player data is saved when saving a campaign. Extend the player data to save more data.

# Node Data
## Core Data
``` lua
table NODE_DATA_TEMPLATE = {
    string name = "",
    function on_land = function () end,
    function on_leave = function () end
}
```  
**name** is the name of the node type.  
**on_land** is the function called when landing on a new node.  
**on_leave** is called after the player decides where they want to move to.  

After both **on_land** and **on_leave** the player's script position is syncronised with the engine position.

## Unique Data
Unique data is a table passed to each individual node that is unique to the specific node.

# API
## Virtual Events
### Notice
Virtual events probably don't work as expected. The virtual event is handled after either the **on_land** or **on_leave** function finishes.

### Quit
`void quit()`  
Calls the **quit** event. This terminates the current gameloop and saves.


# Moving the Player
The player can be moved by changing the **x**, **y** and **z** options in **player.position**.  