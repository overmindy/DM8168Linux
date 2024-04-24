Shared Region Example

Program Logic:
The GPP creates a string buffer in shared region 1 heap. The string buffer is 
then populated with a lowercase string. The GPP then passes the string buffer 
address to the slave. The slave translates the lowercase string to uppercase and
notifies the GPP that it is done with its operation.

