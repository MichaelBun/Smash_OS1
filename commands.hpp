#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <iostream>
#include <stdio.h> 
#include <list>
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef enum { FALSE , TRUE } bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);

typedef struct _Var
{
	char* name;
	char* value;
}Var;


class job
{
	public:
		job(int pid, running_status status);
		int GetPid() const {return pid;}
		double GetTime() const {return (double)(start_time - time(NULL))}
		
	protected:
		int pid;
		time_t start_time;
		char * job_name;
}

job::job(int pid, char* name):pid(pid)
{
	start_time = time(NULL); 
	job_name = name;
}



#endif

