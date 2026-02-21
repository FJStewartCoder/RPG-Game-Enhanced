# Table Functions
## Show Table
### Usage
``` lua
void show_table( table t )
```  
**t** is the table to show.

The function has no return type.

The table is outputted including the keys and values. Tables within tables are shown.  
The format for this is **key** = **value**

### Supported Types
Integers, float/doubles, booleans, strings and tables are supported.  
If data of another type is found, the name of the type will be shown.

## Copy Table
### Usage
```
table copy_table( table t )
```
**t** is the table to be copied.

The function return the copied table.  
The function creates a deep copy of the table. That is tables within tables are also copied.

### Supported Types
Integers, float/doubles, booleans, strings and tables are supported.  
Other types may not be fully copied. They will be added to the table as their original type but this may be a reference.

## Merge Table
...
