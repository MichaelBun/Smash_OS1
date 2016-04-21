/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.hpp"
#include "signals.hpp"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

using namespace std;
using std::list;


char lineSize[MAX_LINE_SIZE];
char* L_Fg_Cmd;
list<Var*> var_list;
int GPid;
int GPid_2=-1;
list<job>* job_list;
list<job> _job_list;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];
    job_list = &_job_list;

    L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
        if (L_Fg_Cmd == NULL)
            exit (-1);
    L_Fg_Cmd[0] = '\0';
	//signal declaretions

	sighandler(SIGTSTP, &ctrl_z); //TODO check parameters. do we need "&"?????
	sighandler(SIGINT, &ctrl_c);
    sighandler(SIGCHLD, &sigchld_handler);

	// Init globals
	GPid=-1;

	// ADDED BY US
	char* fwd=(char*)malloc(sizeof(char*)*MAX_LINE_SIZE); //Former Working Directory
	char* pwd=(char*)malloc(sizeof(char*)*MAX_LINE_SIZE); //Current Working Dir
	getcwd(fwd,MAX_LINE_SIZE); //At the start they are the same
	getcwd(pwd,MAX_LINE_SIZE);
	// ADDED BY US

    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);

		if(!ExeComp(lineSize)) continue;
                // background command
        if(!BgCmd(lineSize, (_job_list))) continue;
					// built in commands
		ExeCmd(lineSize, var_list, (_job_list),fwd,pwd);

		//cout<<"unknown command!"<<endl;

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

