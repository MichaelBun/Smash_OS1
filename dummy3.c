#include <stdio.h> /* standard I/O functions */
#include <unistd.h> /* standard unix functions, like getpid() */
#include <sys/types.h> /* various type definitions, like pid_t */
#include <signal.h> /* signal name macros, and the signal() prototype */

void catch_int(int sig_num)
{
printf("~Don't do that~");
fflush(stdout);
}

int main(int argc, char *argv[])
{
	signal(SIGTERM, catch_int);
	while(1);
	return 0;
}