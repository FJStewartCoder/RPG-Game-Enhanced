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

#### example
`extend_node({
    show_player_data = false,
    mission = mission1
})`  
Will add **show_player_data** and **mission** to the default node data table with the default values set.

## Environment Function
### Usage
`void environment()`  
Is one of the three required functions to create a campaign. **not required for campaignless modules*
CURRENT

### Adding a Node
build_node

### Making Connections
make_connection
