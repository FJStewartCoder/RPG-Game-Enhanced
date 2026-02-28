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
### Usage
``` lua
table merge_table( 
    table source, 
    table other,
    string strategy
)
```
**source** is that table that is used as the table to copy the data to.  
**the original table is not modified*
**other** is the table that will be merged into **source**.
**strategy** is a string of characters used to define how to combine the tables.

### Strategy String
Include any of the below, lowercase characters to add to the strategy.

Use **t** to enable [type checking](#preseve-type).  
Use **o** to enable [overwriting](#overwrite-existing).  
Use **a** to enable [adding new](#add-new-properties).  
Use **d** to enable [deep](#deep).  

### Important
In Lua a table can either be list-like. That is, there are only consecutive numeric keys starting at 1.  
A table can also be dictionary-like. That is, keys can be of any type in no order.  
Another wierd point is that an empty table has neither of these specified types.

Anything that is not a list is considered to be dictionary-like.  

**Lists** and **Dictionaries** are distinguished when combining.  
This means that these are considered to be seperate types, when preserving types.

Deep combination only works when both tables are dictionary-like. However, an empty type is considered to be either.

For **DEEP** combination only, the following is how the system works (format is **source**, **other**):  
Both **DICTIONARY** -> Deep Combine.  
Both **LIST** -> **other** will overwrite **source**.  
Both **EMPTY** -> **other** will overwrite **source**.  

**DICTIONARY**, **LIST** -> (Incompatible types when preserving) **other** will overwrite **source**.  
**LIST**, **DICTIONARY** -> (Incompatible types when preserving) **other** will overwrite **source**.  

**EMPTY**, **DICTIONARY/LIST** -> (Compatible types when preserving) **other** will overwrite **source**.  

**LIST**, **EMPTY** -> (Compatible types when preserving) **other** will overwrite **source**.  
**DICTIONARY**, **EMPTY** -> (Compatible types when preserving) **other** will be deep combined with **source**.  

## Merge Strategies
### Preseve Type
Preseve types will only **overwrite** data if the type matches. This is only active when overwriting data.

### Overwrite Existing
Overwrite existing will overwrite data in the **source** data if both **source** and **other** have data with the same key.  
When overwriting the type will be checked if this option is enabled. If the type does not match, the default data will be kept.

### Add New Properties
Add new properties allows for adding data to **source** for a key that **other** has but **source** does not have. This is not affected by type checking.

### Deep
Deep will merge all tables within tables. This assumes both **source** and **other** have data at a similar key that is both the table type.  
The strategy used to merge these tables will be the same that was used for the original tables.