# Menus
## Menu Items
Menu Items have 2 available fields: **name** and **description**.  
Menu Items can either be a `string` or a `table`.  

For a `string` item, simply write the item **name**.  
For a `table` item, use the following format: `{ string name, string description }`.  

Use a string if you only need the name. Use a table if you need to include a description.


## Basic Menu
### Usage
`string basic_menu( string name, string message, table options )`  
**name** is the name of the menu. This will be shown at the top of the menu.  
**message** is the message that is used for the input field.  
**options** is a list of [Menu Items](#menu-items) which are used for output.  

The function returns a the name given to the [Menu Item](#menu-items) selected.

### Example
`choice = basic_menu("Main Menu", "Which option would you like to pick", { {"Start", "Start a new game"}, "Settings", "Quit" })`  
-- Main Menu --  
1 - Start  
    Start a new game  
2 - Settings  
3 - Quit  

Which option would you like to pick:

### Output
-- *name* --  
1 - *Option Name*  
    *Option Description*  

*Message*:  

## Alternative Menu
### Usage
`string alt_menu( string name, string message, table options )`.  
**name** is the name of the menu. This is not shown on this menu type.  
**message** is the message that is used for the input field.  
**options** is a list of [Menu Items](#menu-items) which are used for output.  

The function returns a the name given to the [Menu Item](#menu-items) selected.

### Example
`choice = alt_menu( "Yes or No", "Would you like to eat some ice cream?", { "Yes", "No" } )`  
Would you like to eat some ice cream?: (y)es, (n)o:  

### Output
*message*: *(i)tem 1*, *(it)em 2*, *(ite)m 3*:  