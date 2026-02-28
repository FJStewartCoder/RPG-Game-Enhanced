# Developer Road Map
## Introduction
It is just the normal roadmap but with a clearer direction for a developer. It will be more technically detailed and may include sub-steps to each goal.

## Road Map
### Better Tables (CURRENT, TESTED)
Better table combination (only deep combine dictionary-like tables instead of list type tables)
NEEDS FIXING AGAIN
What if original data type is table but new is empty, we do need to deep combine this because it could be an inventory
JUST DEEP COMBINE IF BOTH ARE TABLES. NEED TO GO THROUGH ALL POSSIBLE SCENARIOS (ADD TESTS FOR EACH ONE)
+update docs for api

Update uses of combine table to now be appropriate for this.
build.cpp @ 216 - 226
campaign.cpp @ 716 - 720
+ updates does for where these are combined
+ relevant to unique data combined into the template (NOW DEEP COMBINES TABLES BECAUSE LISTS NOW WORK AS INTENDED)
+ relevant to how data is reloaded

### Testing
Add testing to the entire program
Test loading table from file where data type is not implemented

### Campaign Improvements
Add gameloop to the campaign. So, it does not need to be run externally.
Add a setting in INIT to allow for specification of the start node.

## Future Considerations
None