CURRENT:
Get all campaigns
If no campaigns, return error

Check if requested campaign exists
If not, return error

Set campaignName to dir name
Load Init file
if fails, return error

if campaign uses generic
    load generic

load campaign directory

NEW:
Get all campaigns
If no campaigns, return error

Check if requested campaign exists
If not, return error

Set campaignName to dir name
Load Init file
if fails, return error

load campaign directory


MODULE LOAD:
Check campaignless for module directory
if not exists
    return error

Load directory