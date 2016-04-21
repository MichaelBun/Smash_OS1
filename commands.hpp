#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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
using std::list;

typedef enum {suspended, working } jobStatus;
void remove_by_pid(int pid);


typedef struct _Var
{
	char* name;
	char* value;
}Var;


class job
{
	public:
		job(int _pid, jobStatus status, char* name)
		{
			pid = _pid;
			job_status = status;
			start_time = time(NULL);
			job_name = name;
		}
		int GetPid() const {return pid;}
		int GetTime() const {return (int)(time(NULL)-start_time);}
		char* GetName() const {return job_name;}
		jobStatus GetStatus() const {return job_status;}
		void ChangeStatus(jobStatus new_status) {job_status = new_status;}

	protected:
		int pid;
		time_t start_time;
		char * job_name;
		jobStatus job_status;
};

/*job::job(int pid, jobStatus status, char* name):pid(pid),job_status(working)
{
	start_time = time(NULL);
	job_name = name;
}
*/


/*Function Declarations*/
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, list<job>& job_list);
int ExeCmd(char* lineSize, list<Var*>& var_list, list<job>& job_list, char* fwd, char* pwd);
void ExeExternal(char *args[MAX_ARG],list<job>& job_list);

extern list<job> _job_list;
//Globals
extern int GPid; //to know which process is in foreground
extern char* L_Fg_Cmd;
extern list<job>* job_list;
extern int GPid_2;

#endif

