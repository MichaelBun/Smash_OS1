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

char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE]; 
list<Var*> var_list;
list<job> job_list;
int GPid;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
   // char cmdString[MAX_LINE_SIZE]; 	   
	char* cmdString = (char*)malloc(sizeof(char)*MAX_LINE_SIZE);
	
	//signal declaretions

	sighandler(SIGTSTP, &ctrl_z); //TODO check parameters. do we need "&"?????
	sighandler(SIGINT, &ctrl_c);

	/************************************/

	/************************************/
	// Init globals 


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);  
		strcpy(cmdString[strlen(lineSize)-1],"\0");
		printf("%s\n%s\n",lineSize,cmdString);
		printf("%d\n",strlen(cmdString));
		//cmdString[strlen(lineSize)-1]='\0'; //SEGMENTATION FAULT
					// perform a complicated Command
		printf("%s",cmdString);
		if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(lineSize, job_list)) continue; 
					// built in commands
		ExeCmd(lineSize, cmdString, var_list, job_list);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

