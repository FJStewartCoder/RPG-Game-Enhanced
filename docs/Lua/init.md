# General
## File Name
Place a file named "INIT.lua" in the root of your campaign directory.

# Settings
## Introduction
Settings are used to set some information about a campaign.  
Settings are not read if included in an INIT file in the "campaignless" directory.

## Campaign Name
### Usage
`string CAMPAIGN_NAME`   
Can be used to set the name of the campaign.  
By default, this is set to the name of the campaign directory.  

### Example
`CAMPAIGN_NAME = "Test Campaign"`  
Will set the name of the campaign to "Test Campaign".

## Use Campaignless Functions
### Usage
`boolean USE_GENERIC`  
Can be used to set if you want to use data defined in the "campaignless" directory.  
By default, this is set to FALSE.  

### Example
`USE_GENERIC = true`  
Will pull in functions and data from the campaignless directory for you to use.  

# Functions
## Introduction
There are three required functions in the INIT file of any campaign.  
The `environment` function is excluded from INIT files in the "campaignless" directory.

## Build Function
### Usage
`void build()`  
Is one of three required functions to create a campaign.  
The build function is responsible for creating the different node types.  
Node types can be created using the [`add_node` function](#adding-node-types).  

### Adding Node Types
#### Usage
`int add_node( table node_table )`
**node_table** is a table that stores data about the node type. The data stored in it is determined by the default node table and the extensions added.  
You can read about the default node table [here](./campaign.md#node-data).

#### Example
`add_node({
    name = "Shop",
    on_land = land_shop,
    on_leave = leave_shop
})`  

## Extend Function
### Usage
`void extend()`  
Is one of the three required functions to create a campaign.
The extend function is used to add node and player extensions to allow nodes to be more varied and the player to have more attributes.

### Extend Player
#### Usage
`int extend_player( table extension_table )`  
**extension_table** is a table that is used to extend the default player table with new data.

#### example
`extend_player({
    health = 100,
    damage = 5,
    shield = 20
})`  
Will add **health**, **damage** and **shield** to the player data table with the default values set.

### Extend Node
#### Usage
`int extend_node( table extension_table )`  
**extension_table** is a table that is used to extend the default node table with new data.

Every node gains these new fields regardless of type.  
The extension table is relevant to the node type and each instance of a node type can **NOT** have different data.  
[Unique Data](./campaign.md#unique-data) can be used to achieve this, instead.  

#### Example
`extend_node({
    show_player_data = false,
    mission = mission1
})`  
Will add **show_player_data** and **mission** to the default node data table with the default values set.

## Environment Function
### Usage
`void environment()`  
Is one of the three required functions to create a campaign. **not required for campaignless modules*
The environment function is responsible for actually creating the world.

### Adding a Node
#### Usage
`void build_node({
    string name,
    short x, y, z,
    table unique_data,
    string blocked
})`  
**name** is the name of the node you want to build. Each node made with [`add_node()`](#adding-a-node) is available for use here.  
**x**, **y**, **z** are the coordinates that the node will be built.  
**unique_data** is a table of data unique to this node. This is passed to the on_land and on_leave functions.  
**blocked** is a string of characters used to allow or disallow traversal between adjacent nodes. More can be read [here](#blocked-string).

#### Blocked String
The blocked string is made up of lower-case character to block certain directions.  
The characters: l, r, u, d, f, b, n and p can be used to block the directions: left, right, up, down, forward, back, next and previous respectively.  
By using any of these characters in the blocked string will block a connection from being possible in that direction.  
Use the ! character to invert the effect. By default ! will block all direction and each character will unblock a direction.  
Some special characters are available as aliases of other character strings.  
x, y, z and t ( t for teleport directions ) are used as such. x = lr, y = fb, z = ud and t = np.  
x, y, z and t are also compatible with !

#### Example
`build_node(
    "Shop",
    1, 0, 5,
    {
        shop-name = "Fish Market"
    },
    "!xz"
)`  
Will create a new node of type **Shop** at **(1, 0, 5)** with **unique data** where the **shop-name** is **"Fish Market"**. Connections will only be made in the **x** and **z** directions.

### Making Connections
#### Usage
`int make_connection(
    int x1, y1, z1,
    int x2, y2, z2,
    string link,
    boolean one_way,
    boolean override_blocked
)`  
**x1**, **y1** and **x1** make up the coordinates of the **source** node.  
**x2**, **y2** and **x2** make up the coordinates of the **destination** node.

**link** is the direction in which **destination** will be connected to **source**.  
Link can be any of the following ( brackets indicate different combinations e.g [(a)b]c means a, ab and abc are availible):  
(l)eft, (r)ight, (u)p, (d)own, (f)orward, (b)ack, (n)ext, [(p)rev]ious

**one_way** is if you want to create a connection but only one way.  
**override_blocked** is an option to allow you to bypass blocked parameters.  

#### Example
`make_connection(
    0, 0, 0,
    10, 5, 3,
    "next",
    false,
    false
)`  
Will create a connection between **(0, 0, 0)** and **(10, 5, 3)** whilst respecting blocking.
It will create a connection from **(0, 0, 0)**, in direction **next**, to **(10, 5, 3)**;
and a connection from **(10, 5, 3)**, in direction **previous**, to **(0, 0, 0)**.

An example of when you could use bypass block is if you have lots of houses but one happens to have a hole in the wall.
This would allow you to create a hole in the house wall without creating a seperate node type.
