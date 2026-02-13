# Road Map
## Documentation (CURRENT)
Add documentation to the entire program including better headers and .md files for the API.

## Reworked Nodes
Instead of having a main node_template, each node type has its own unique_data template.  
This could allow for each node type to be better customisable.  
Why? because the current extend_node is useless. It's difficult to integrate because each node type requires seperate data fields.  
This would be fixed by unique_data templates for each node type.  
This could come with an upgrade to the add_node function.
The function could be changed to add_node( string name, function on_land, function on_leave, table unique_data_template ).

## Better Modules
Better require system for modules similar to how NeoVim works.  

Upgrade campaignless directory. Instead of a single directory, have separate modules. So each directory in campaignless is a different module with its own init.

Then, make each dir in campaignless requirable so that you don’t have to add as much stuff to your campaign. When required, automatically run the init file.

Could overwrite the require function for custom functionality or do some other thing.

## Better Save File
Combine the template and read data instead of overwriting. might help with corruption or invalid data types

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