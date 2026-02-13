# Logging
## Functions
Every function should start and end with a suitable log trace describing the current function.
A useful example of this could be called \_\_FUNCTION\_\_ and ended \_\_FUNCTION\_\_.
So, all endpoints should be covered. This is helpful in debugging recursive programs.

## Log Types
INFO, WARN, ERROR and FATAL are messages for the user. TRACE and DEBUG are for the programmer.
Consider this when using each of the different levels.