# Making a Campaign
## Introduction
This is a simple guide to creating a simple campaign and using the functionality available.  
Most of this guide applies to creating a module but some functions are not used.

A sample/example campaign is included [here](../../campaigns/Sample/).  
The sample does **NOT** mirror this guide.

If you want to see the limits of what is available, see the [test campaign](../../campaigns/test_campaign).  
This shows everything you could do including every functions and its options, modules and deep directories.

## Campaign Directory
To begin creating a new campaign, you need to create you campaign directory.  
You can do this by creating a new directory in the "campaigns" directory.  
The name of the directory is the default campaign name. This can be changed within the INIT file.  
<!-- Using campaignless functionality -->

There is only one required file in your campaign. That is the [INIT](#simplified-init-file) file.  

Every directory and file in the campaign directory is considered to be a script. Each is combined into your campaign.  
So, it is possible to include your scripts within other directories.  

## Simplified INIT File
### Introduction
To begin actually creating your campaign, create a file in the root of your campaign called **INIT.lua**.  
> You should currently have a directory in campaigns, for example Aliens, like "./campaigns/Aliens".  
> In this directory you should have INIT.lua as such "./campaigns/Aliens/INIT.lua"

If you are looking for a full run down of everything you can put in this file, refer to the [INIT documentation](./init.md).

### Campaign Settings
An optional step that you can take is to name your campaign to something other than your directory name.  
You can do this with the following code; Assume we are creating a campaign called "Spaceship Game".  
`CAMPAIGN_NAME = "Spaceship Game"`  

If you want to use functions that are non-specific to a campaign, you can include modules from the modules directory. This can be done as shown below. **This is optional and no modules are imported*  
``` lua
MODULES = {
    "module_name_1",
    "module_name_2"
}
```  

### Required Functions
There are 3 required functions for the campaign to work. Each one should only contain the functions specified for it.  
If you choose to ignore this advice, expect your campaign not to work.

To begin, you will need a **build**, **extend** and **environment** function. This is shown below:  
``` lua
function build()
end
    
function extend()
end 

function environment()
end
```  

Each one runs its respective functionality to create your campaign.

## Extending Templates
### Introduction
A major functionality that you will need it template extensions. These allow you to add new data to the base player and node templates.  
This can allow you to give the player new attributes or nodes new data.

### Player
You will likely extend the player a lot. The player data is the only area of the program that gets saved.  
So, it is important that anything that needs to be saved is stored in the player data.

Assuming we want to add a health field with default value 100 and damage field with default value 5 to our player, we could do it as such:

``` lua
function extend()
    extend_player({
        health = 100,
        damage = 5
    })
end 
``` 

Here a table is passed to the function with the data to be extended. **You can not override existing data fields*

## Node Types
### Introduction
To actually begin building up your toolkit of nodes, you need to create a new node type.
Node types are created in the **build** function using the **new_node_type** function as such:  

``` lua
function build()
    add_node(
        "Spaceship",
        land_spaceship,
        leave_spaceship,
        {
            mission = "No Mission"
        }
    )
end
```  
Here, a new node type has been added with name **Spaceship**.

**Spaceship** is the node name.  
**land_spaceship** is the landing function. This is called when you land on this node. More will be discussed on these later.  
**leave_spaceship** is the leaving function. This is called when you leave this node.  
The table is the unique data template. This is the data that will be passed to your functions. **data will be filled in differently but will use these fields*

### Making a Landing and Leaving Function
It would now be recommended to create a new file in your campaign directory for this node type.  
For example, I will name this "spaceship.lua". In this file we will implement **land_spaceship** and **leave_spaceship**.  
The names of these functions can be anything but this example uses the scheme **land_*nodename*** and **leave_*nodename***.  

In our new file create 2 functions as such:
``` lua
function land_spaceship( unique, player )
    -- YOUR CODE HERE
end

function leave_spaceship( unique, player )
    -- YOUR CODE HERE
end
```

The arguments **unique** and **player** are passed in by the game engine. These can be used to manipulate the relevant data.  
**unique** is the unique data for this node. This is based on the unique data template.
**player** is the current player data.  

There is no specified return type for these functions but the return value is **NOT** read.  
**if you don't want a landing or leaving function you can pass `function () end` to on_land or on_leave*

## Building Your Environment
### Introduction
To build the environment, you will likely need some more node types first. Assuming we have implemented **House**, **Path**, **Shop** and **Mothership**, we can continue.  

Building the environment is done in the **environment** function with the **build_node** function, as shown below:
``` lua
function environment()
    build_node(
        "House",
        "Bob's House"
        0, 0, 0,
        {

        },
        ""
    )
end
```  
The above code will create a new **House** node at **(0, 0, 0)** with no [unique data](#unique-data) and no [blocking data](#blocking-directions).  
The **House**'s unique name is **"Bob's House"** this will be seen by the player when navigating.  

**IMPORTANT NOTICE:** Whichever node is at **(0, 0, 0)** is considered to be the **start** node. When loading in the first time, you will be placed there.

### Unique Data
<!-- This is actually now a template REWRITE -->
Unique data is a templateless table of data that is passed to each individual node. Each node's unique data is unique to that node.  
This can be used to create nodes with different data. For example, a "Shop" node type could optionally have unique data to define the shop's items and name.  

### Blocking Directions
The blocking string blocks certain directions when making connections.

Use the first letter of a direction to block that direction. (left, right, up, down, forward, back, next, previous) or x, y, z and t for combined directions.  
Use ! as the first character to invert the operation.

For full details on the blocking string, read [here](./init.md#blocked-string).  

### Complete Example
<!--Add unique names for each node and mention the templates TODO -->
``` lua
function environment()
    build_node(
        "Path",
        0, 0, 0,
        {

        },
        ""
    )

    build_node(
        "House",
        1, 0, 0,
        {
            owner = "Bob"
        },
        "!l"
    )

    build_node(
        "House",
        -1, 0, 0,
        {
            owner = "Lucy"
        },
        "!r"
    )
end
```  
The above example shows a path in the middle and two houses at each side. The house on the right can only be entered from the left and the house on the left can only be entered from the left.  
The house on the left has the **owner** "Lucy" and the house on the right has an **owner** "Bob".