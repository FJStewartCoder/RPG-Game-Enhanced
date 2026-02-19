# Developer Road Map
## Introduction
It is just the normal roadmap but with a clearer direction for a developer. It will be more technically detailed and may include sub-steps to each goal.

## Road Map
### Better Save File (CURRENT)
Combine the template and read data instead of overwriting. might help with corruption or invalid data types.  
Use the functions made in "table combination".

### Reworked Nodes
Instead of having a main node_template, each node type has its own unique_data template.  
This could allow for each node type to be better customisable.  
Why? because the current extend_node is useless. It's difficult to integrate because each node type requires seperate data fields.  
This would be fixed by unique_data templates for each node type.  
This could come with an upgrade to the add_node function.

The function could be changed to `new_node_type( string type_name, function on_land, function on_leave, table unique_data_template )`.  
Also change build_node to `build_node( string type, string location_name, coords, unique_data, blocked )`.  
**location_name** will be used in the where you want to go bit so that each specific location has an optional name instead of the node type.  
If left blank, use the node type instead.

Then, when a node is made use the table combination scripts to combine the tables.  
Could even allow for the removal of the node_queue thing.  

### Header Documentation
Add documentation to the entire program including better headers.

### Gameloop From Save
Need to load the position first before making any actions when loading from save file. Also don't run the landing function