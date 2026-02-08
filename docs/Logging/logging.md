# FATAL
## When to use:
The event that has occured is causing the program to completely shutdown.

## Examples
### Example 1:
An integral file is missing which means that the program is unable to continue.

# ERROR
## When to use:
The event that has occured has failed but this does not lead to the program shutting down.
User intervention would be required such as re-attempting the operation or ignoring that functionality.

## Examples
### Example 1:
Unable to connect to the internet to access a database. The program will still work just unable to use the functionality.

# WARN
## When to use:
An event has occured that could lead to an error occuring at a later point. 
This is typically in relation to a user choosing to continue with an event regardless of error potential.

## Examples
### Example 1:
A file is corrupted but the user chooses to attempt to load it anyway. This invokes a warning that this could cause an error.

# INFO
## When to use:
An event has occured and information about the event can be outputted.
This is easy to understand and typically useful to the average user.
The information provided is usually of a positive outcome or an error has been prevented. 

## Examples
### Example 1:
A file that the user is loading has been loaded. The outputted information may be a summary of the file content.

### Example 2:
A web API has returned an error 404. The program may output that the operation is being tried again.

# DEBUG
## When to use:
For displaying information that helps to debug the application.
This would typically used to output data stored within variables that could help to identify possible issues.
Could also be used to output caught error messages if the user does not need to see them.

## Examples
### Example 1:
Some faults have occured and you need to know the values stored with that variables that may be causing issues.

### Example 2:
More specific information on the current process occuring. This could involve line by line logging.

# TRACE
## When to use:
For displaying current information about the current processes of the program.
These outputs likely won't provide any value to a user and be only useful for getting the current status.
These outputs could help in debugging but simply make statements about something that happened.

## Examples
### Example 1:
You need to sort a list. So, you will trace when the sorting begins and ends.

# General Note:
Each decreasing precedence of log should provide increasingly less critical information.
Since it is possible to disable logs based on a level system, increasing the minimum level should reflect the information you recieve.
For example, a trace will provide the least critical information. If an "INFO" log provided less critical information than a "TRACE", either one is not used correctly.

Considering all levels at once, "FATAL" will provide information on a fatal error. 
If you then enabled "ERROR", you would increase the total information that could have lead to the cause. However, this is less critical information.
Then enabling "WARN" would then provide finer detail into the cause.
Continuing on, each level should provide finer detail into the what is happening with decreasing significance.