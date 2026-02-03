# Save Format
## Description
The save format needs to save the name of the campaign and the current player data into a file with a relevant name.

## Where
...

## Base Format
### File Magic
The file magic will allow us to know if any file regardless of extension is a file that we can read.
The file magic for this file will be "RPenGineFILE".

### Version
The next piece of data to store in the file is the file version. This will be used to load legacy save files in the event that the save format changes.
This will be stored as a 32 bit integer in the format version * 100. For example: version 2.7.2 would be 272

## Version 1 Format
### Campaign Name
The campaign name will be the first piece of data stored in the file.
It will be stored by storing the length as a 32 bit integer followed by the name
For example: "test_campaign" would be stored as
13 (as bytes) test_campaign 

### Save Time
The next piece of data will be the time that the file was saved as a 64 bit integer.
Time refers to the Unix epoch.

### Player Data
The player data table will then be stored in the format as specified below. This technically allows for reducing any lua data to string interpretations.
The following types are availible in Lua: none, lua_nil, string, number, thread, boolean, function, userdata, lightuserdata, table
This save method will support: lua_nil, string, number, boolean, table

#### Basis
Each different type will be saved as such:

variable length: 32 bit integer
variable name: string
type: char
data: per type

#### lua_nil (n)
No data will be stored for this. So, skip to next data

For a variable named "abc":
3 (as bytes) abcn

#### string (s)
string length: 32 bit integer
string: string

For a variable named "abc" and data "hello":
3 (as bytes) abcs5 (as bytes) hello

#### number (i)
number: 32 signed int

For a variable named "abc" and data 129:
3 (as bytes) abci129 (as bytes)

#### boolean (b)
boolean: 1 byte

0 is false, 1 is true

For a variable named "abc" and data true:
3 (as bytes) abcb1 (as byte)

#### table (t)
table length: 32 bit integer
table data: list of above types

For a variable named "abc" and data {
    "name" = "Eva Smith"
    "age" = 24
}:
3 (as bytes) abct2 (as bytes) 4 (as bytes) names9 (as bytes) Eva Smith3 (as bytes) agei24 (as bytes)
