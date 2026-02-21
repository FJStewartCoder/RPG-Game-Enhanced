# Node API
## Introduction
The node function allows you to get some information about certain nodes.

## Get Node
### Usage
``` lua
table get_node( table coords )
```
**coords** is a coordinates table. This is in the format {**x**, **y**, **z**} or {x=..., y=..., z=...}.

The function returns a table with some data which can be seen below.

### Return Value
``` lua
{
    constant bool exists,
    constant string type,
    constant string unique_name,
    table value
}
```

**exists** is a boolean that informs you if the node was found. If this is false, the node does not exist; else, the data is valid.  
**type** is the node's type. The default for this is "NONE" if the data is not found.  
**unique_name** is the name that identifies this specific node. This is by default "" if the node is not found.  
**value** is the unique data table for this node. This is a blank table by default, if the node is not found.  
**modifying these values will update the data at this node for this session; however, unique data is not saved*
