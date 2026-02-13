TODO: Upgrade campaignless directory. Instead of a single directory have separate modules. So each directory in campaignless is a different module with its own init.

Then, make each dir in campaignless requirable so that you don’t have to add as much stuff to your campaign

ROAD MAP:
-Documentation
-unique_data templates
-Better require system for modules
-Better load from save file ( combine the template and read data instead of overwriting ). might help with corruption or invalid data types

For last point: Create a combine_table function that combines two tables. This can take an argument of int ( bit field of enum ) for combination strategy

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