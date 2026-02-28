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

## Saving
Only the player data is saved when saving a campaign. Extend the player data to save more data.

Only some data types are supported. Passing in a type that is not available will be saved as nil.
The types that can be saved are: int, float, str, bool, table, nil.  

# Node Data
## Type Data
``` lua
string name,
function on_land,
function on_leave,
table unique_data_template
```

**name** is the name of the node type.  
**on_land** is the function called when landing on a new node.  
**on_leave** is called after the player decides where they want to move to.  
**unique_data_template** is a table that is used as this node types's parameters.  

After both **on_land** and **on_leave** the player's script position is syncronised with the engine position.
**on_land** and **on_leave** recieve **unique_data** and **player_data** as arguments by default. These can optionally be accepted.  

## Unique Data
### Introduction
Unique data is a table passed to each individual node that is unique to the specific node.  
The unique data table is combined with the template to create a final table which is passed to on_land and on_leave.  

### Template
The unique data template is the argument template for the node type. Similar in nature to function arguments.  
An example of a template is defined below: **the template must be dictionary-like*

``` lua
{
    shop_name = "Default Shop Name",
    shop_items = {

    },
    minimum_level = 0
}
```
Each value set here is considered to the default value. This template will only be used for the respective node type.  

### Arguments
The arguments that are passed use the above base template and are per **node** rather than per **node type**.
Arguments can either be a list or dictionary-like. Dictionary-like is prefered for readability but lists are accepted.  
**one risk of using list is inconsitency. ONLY use this approach with templates with either one or two keys*  
\*\**despite examples showing more keys/value pairs are possible, it does not actually work*  

Arguments are merged into the template by only overwriting existing parameters. This is a deep merge.  
This means that tables in the main table also get merged using the same strategy into each other.
Also, you can not add new parameters but only overwrite existing ones.  
Additionally, types are not maintained. This means it is possible to overwrite strings with integers etc.

### Examples
#### Template
The template for these nodes is below throughout:
``` lua
{
    shop_name = "Default Shop Name",
    shop_items = {

    },
    minimum_level = 0
}
```

#### Example 1
To populate the template, you could pass the below data.
<!-- EARTHBOUND lamp -->
``` lua
{
    shop_name = "Bob's Shop",
    shop_items = {
        "Scary Lamp",
        "Candle"
    },
    minimum_level = 5
}
```

#### Example 2
You could also pass in the below data.
``` lua
{
    "Bob's Shop",
    {
        "Scary Lamp",
        "Candle"
    },
    5 
}
```

#### Example 3
Since the template is a template for the data passed in, you don't need to fill in each field.
``` lua
{
    shop_items = {
        "Scary Lamp",
        "Candle"
    },
    shop_name = "Bob's Shop"
}
```
Here, **minimum_level** would be defaulted to 0.  
This also shows that you do not need to pass in the keys in the order defined in the template.

#### Example 4
You can also use the template as below:
``` lua
{
    "Bob's Shop",
    {
        "Scary Lamp",
        "Candle"
    }
}
```
This also defaults **minimum_level** to 0 since it was not passed in.
Unlike the key approach, the arguments must be in the order defined in the template.

#### Example 5
The template, however, is strict so you can not pass in data not defined in the template.
``` lua
{
    shop_items = {
        "Scary Lamp",
        "Candle"
    },
    shop_name = "Bob's Shop",
    shop_keeper_name = "Bob"
}
```
As before, the template will default **minimum_level** but **shop_keeper_name** will be rejected.

#### Example 6
The merge system will also reject the below example.
``` lua
{
    "Bob's Shop",
    {
        "Scary Lamp",
        "Candle"
    },
    5,
    "Bob"
}
```
This example will reject **"Bob"** since it is beyond the end of the template.

#### Example 7
Merges are not type safe; This can lead to the below happening.
``` lua
{
    shop_name = 123,
    shop_items = "Scary Lamp",
    minimum_level = function () end
}
```
This will be accepted since the operation does not care about the types.  
This suggests that you should check types.  
This reduces some restrictions for the game since it is more variable.  

#### Example 8
For this example only, consider the following template:
``` lua
{
    shop_name = "Default Shop Name",
    shop_items = {
        { 
            item_name = "Default Item Name",
            cost = 100,
            required_level = 0
        }
    },
    minimum_level = 0
}
```
The above template wants to use the first table item as a template. However, tables are not deep merged.  
This means that only the main table has its values merged.

If we passed the below data to this template,
``` lua
{
    shop_name = "Bob's Shop",
    shop_items = {
        {
            item_name = "Scary Lamp",
            cost = 100
        },
        {
            "Candle",
            cost = 20
        }
    },
    minimum_level = 5
}
```

The expectation of output would be:
``` lua
{
    shop_name = "Bob's Shop",
    shop_items = {
        {
            item_name = "Scary Lamp",
            cost = 100,
            required_level = 0
        },
        {
            "Candle",
            cost = 20,
            required_level = 0
        }
    },
    minimum_level = 5
}
```

However, **shop_items** is copied from the passed-in data to the template. So, you would be left with:
``` lua
{
    shop_name = "Bob's Shop",
    shop_items = {
        {
            item_name = "Scary Lamp",
            cost = 100
        },
        {
            "Candle",
            cost = 20
        }
    },
    minimum_level = 5
}
```

This needs to be considered when using templates.

# API
## Virtual Events
### Notice
Virtual events probably don't work as expected. The virtual event is handled after either the **on_land** or **on_leave** function finishes.

### Quit
`void quit()`  
Calls the **quit** event. This terminates the current gameloop and saves.


# Moving the Player
The player can be moved by changing the **x**, **y** and **z** options in **player.position**.  