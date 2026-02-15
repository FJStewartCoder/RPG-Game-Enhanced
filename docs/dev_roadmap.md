# Developer Road Map
## Introduction
It is just the normal roadmap but with a clearer direction for a developer. It will be more technically detailed and may include sub-steps to each goal.

## Road Map
### Documentation (CURRENT)
Add documentation to the entire program including better headers and .md files for the API.

### Better Modules
Create a function `bool require_module( string module_name )` that will load in the required module from the campaignless directory.  
Rename "campaignless" to "modules".  
Take inspiration from how NeoVim works. That is, when the module is loaded, load the init file.  

Upgrade campaignless directory. Instead of a single directory, have separate modules. So each directory in campaignless is a different module with its own init.

### Table Combination Scripts
Create a combine_table function that combines two tables. This can take an argument of int ( bit field of enum ) for combination strategy

For example: an option to only override data and don’t include new fields or options to preserve original type etc

STRATEGIES:
Override:
Iterate the combining list ( not the original ) and place all of the data into the original overwriting any original data. This means that missing data in the combining list will have defaults from original.

Field Override:
Iterate the original table and if the combining table has that field, overwrite the data in the original. The prevents new fields being added but keeps defaults if not available.

Preserve Original:
Iterate the original, if the combining table has the same field with the same data type, overwrite the data. Same benefits as before but type is maintained as well

Additions Only:
Iterate combining list, if the original does not have the data, add it to the original table and ignore overwriting existing data. As the name suggests, additions only.

### Reworked Nodes
Instead of having a main node_template, each node type has its own unique_data template.  
This could allow for each node type to be better customisable.  
Why? because the current extend_node is useless. It's difficult to integrate because each node type requires seperate data fields.  
This would be fixed by unique_data templates for each node type.  
This could come with an upgrade to the add_node function.
The function could be changed to add_node( string name, function on_land, function on_leave, table unique_data_template ).  
Then, when a node is made use the table combination scripts to combine the tables.  
Could even allow for the removal of the node_queue thing.  

### Better Save File
Combine the template and read data instead of overwriting. might help with corruption or invalid data types.  
Use the functions made in [table combination](#table-combination-scripts).
