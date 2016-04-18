//		commands.c
//********************************************
#include "commands.hpp"
#include <sys/stat.h>
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************

using namespace std;
using std::list;


int ExeCmd(char* lineSize, char* cmdString, list<Var*>& var_list, list<job>& job_list)
{
	char* cmd; 
	char* args[MAX_ARG];
	char* pwd;
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	// ADDED BY US
	char* fwd; //Former Working Directory
	getcwd(fwd,MAX_LINE_SIZE); //At the start they are the same
	getcwd(pwd,MAX_LINE_SIZE);
	// ADDED BY US
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		getcwd(pwd,MAX_LINE_SIZE);
		if(!strcmp(args[1],"-")) //We want to go to the former path
		{
			if (!chdir(fwd)) // We Succeeded
			{
				strcpy(fwd,pwd);
				getcwd(pwd,MAX_LINE_SIZE);
			}
			
		}
		else //Want to change to a given path
		{
			if(!chdir(args[1])) // We Succeeded
			{
				strcpy(fwd,pwd);
				getcwd(pwd,MAX_LINE_SIZE);
			}
			else // We Failed
			{
				if(errno == ENOENT || errno == ENOTDIR) // We Failed 
				//ENOENT = No such directory 
				//ENOTDIR = A component of the path does not exist
				{
					printf("smash error: > “%s” - path not found\n", args[1]);
				}
			}
		}

	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		getcwd(pwd,MAX_LINE_SIZE);
		printf("%s\n", pwd);
		
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "mkdir"))
	{
 		getcwd(pwd,MAX_LINE_SIZE);
		if(mkdir(args[1],0777) == -1)
		{
			if(errno == EEXIST) // EEXIST - This directory already exists
			{
				printf("smash error: > “%s” – directory already exists\n", args[1]);
			}
			printf("smash error: > “%s” – cannot create directory\n", args[1]);
		}
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "set")) 
	{
		Var* new_var = (Var*)malloc(sizeof(Var));
 		char* var_name = (char*)malloc(sizeof(args[1]));  // Setting up the new item in the var list
		char* var_value = (char*)malloc(sizeof(args[2]));
		new_var->name = var_name;
		new_var->value = var_value;
		var_list.push_back(new_var);
		
	}
	/*************************************************/
		
	else if (!strcmp(cmd, "unset")) 
	{
 		for(std::list<Var*>::iterator i = var_list.begin(); i<=var_list.end(); var++)
		{
			if(!strcmp((*i)->name,args[1]))
			{
				free((*i)->name);
				free((*i)->value);
				free((*i));
				var_list.remove(i);
			}
		}
		printf("smash error: > “%s” - variable not found\n",args[1]);
	}
	/*************************************************/
		
	else if (!strcmp(cmd, "show")) 
	{
		if(!strcmp(args[1],"")) //Print all
		{
			for(std::list<Var*>::iterator i = var_list.begin(); i<=var_list.end(); var++)
			{
				printf("%s := %s\n",(*i)->name,(*i)->value);
			}
		}
		else //Print only one
		{
			for(std::list<Var*>::iterator i = var_list.begin(); i<=var_list.end(); i++)
			{
				if(!strcmp((*i)->name,args[1]))
				{
					printf("%s := %s\n",(*i)->name,(*i)->value);
				}
			}
		}
 		
	}
	/*************************************************/
	
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		for(std::list<Var*>::iterator i = var_list.begin(); i<=var_list.end(); i++)
		{
			int pid = (*i).GetPid();
			double time = (*i).GetTime();
			char* name = (*i).GetName();
			printf("[%d] %s : %d %f secs\n",counter,name,pid,time);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		int my_pid = (int)getpid();
		printf("smash pid is %d", my_pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		int job_num = atoi(args[2]);
   		if(job_num>job_list.length()) // job does not exist
		{
			printf("smash error: > kill %s – job does not exist",args[2]);
		}
		else
		{
			int system_call;
			system_call = (-1)*atoi(args[1]);
			int counter = 1;
			for(std::list<job>::iterator i = job_list.begin(); i<=job_list.end(); i++)
			{
				if(counter == job_num ) // This is the one we need
				{
					if(kill((*i).GetPid(),system_call) == -1) // we failed in sending the sig
					{
						printf("smash error: > kill %s – cannot send signal",args[2]);
					}
					else
					{
						if(system_call == SIGSTOP || system_call == SIGTSTP) //We want to suspend
						{
							jobStatus new_status = suspended; //This process is now suspended
							(*i).ChangeStatus(new_status);
						}
						else if(system_call == SIGCONT)
						{
							jobStatus new_status = working; //This process is now suspended
							(*i).ChangeStatus(new_status);
						}
						else if(system_call == SIGKILL || system_call == SIGQUIT) // We want to delete the item from the job list
						{
							free((*i).GetName());
							job_list.remove(i);
						}
					}
					break;
				}
			}
		}
	} 
	/*************************************************/
	
	else if (!strcmp(cmd, "fg")) //NEED TO ADD LIST REMOVAL
	{
		int job_num = atoi(args[1]);
		int counter = 1;
		for(std::list<job>::iterator i = job_list.begin(); i<=job_list.end(); i++)
			{
				if(counter == job_num)
				{
					int job_pid = (*i).GetPid();
					if((*i).GetStatus() == suspended)
					{
						//Move to foreground
						if(kill(job_pid,SIGCONT) == -1) //it was suspended
						{
							printf("Error continueing suspended in foreground\n");
						}
						else
						{
							free((*i).GetName());
							job_list.remove(i);
							waitpid(job_pid,NULL,WUNTRACED); // wait untill we done	
						}
					}
					else //it was running in the background
					{
						free((*i).GetName());
						job_list.remove(i);
						waitpid(job_pid,NULL,WUNTRACED); // wait untill we done
					}
					break;
				}
			}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		int job_num = atoi(args[1]);
		int counter = 1;
		for(std::list<job>::iterator i = job_list.begin(); i<=job_list.end(); i++)
			{
				if(counter == job_num)
				{
					int job_pid = (*i).GetPid();
					if((*i).GetStatus() == suspended)
					{
						//Move to background with status working
						if(kill(job_pid,SIGCONT) == -1) //it was suspended
						{
							printf("Error continueing suspended in foreground\n");
						}
						else
						{
							(*i).ChangeStatus(working);
						}
					}
					break;
				}
			}
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		time_t start_time[job_list.length()];
		if(args[1] =="") //regular quit
		{
			exit(0);
		}
		else if(args[1] == "kill")
		{
			int counter_SetTime = 1;
			for(std::list<job>::iterator i = job_list.begin(); i<=job_list.end(); i++)
			{
				if(kill((*i).GetPid(),SIGTERM)==-1)
				{
					printf("Error with sending signal\n");
				}
				else
				{
					start_time[counter_SetTime] = time(NULL);
				}
				counter_SetTime++;
			}
			while(1) //Now check if 5 seconds past, and delete whatever is not needed
			{
				int counter_delete=1; //For start time array
				for(std::list<job>::iterator i = job_list.begin(); i<=job_list.end(); i++)
				{
					if(waitpid((*i).GetPid(),WNOHANG)) // This job ended.
					{
						free((*i).GetName());
						job_list.remove(i);
					}
					else if((double)(start_time[counter_delete] - time(NULL))>5) // 5 Seconds past
					{
						if(kill((*i).GetPid(),SIGKILL)==-1) //Kill it
						{
							printf("Error with sending signal\n");
						}
						else //Remove from list
						{
							free((*i).GetName());
							job_list.remove(i);
						}
					}
					counter_delete++;
				}
				
				if(job_list.empty()) //true if list is empty
				{
					exit(0);
				}
			}
		}
   		
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, job_list);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, list<job>& job_list)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					printf("Error\n");
					exit(1);
					/* 
					your code
					*/
        	case 0 :
                	// Child Process
               		setpgrp();
					execvp(args[0],args);
					printf("Error\n");
					exit(1);
			
			default:
					waitpid(pID,NULL,WUNTRACED);
					break;
			
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
	char* new_command[MAX_ARG];
	new_command[0] = "csh";
	new_command[1] = "-f";
	new_command[2] = "-c";
	new_command[3] = linesize;
	new_command[4] = NULL;
	
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					printf("Error\n");
					exit(1);
					/* 
					your code
					*/
        	case 0 :
                	// Child Process
               		setpgrp();
					execvp(new_command[0],new_command);
					printf("Error\n");
					exit(1);
			
			default:
					waitpid(pID,NULL,WUNTRACED);
					break;
			
	}
		
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char *linesize, list<job>& job_list)
{
	char* Command;
	char* delimeters = " \t\n";
	char* args[MAX_ARG];
	if(linesize[strlen(linesize)-2] == "&")
	{
		linesize[strlen(linesize)-2] = "\0";
		/*if (Command == NULL) //Building the args
			return 0; */
		for (i = 1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}
		args[0] = Command;

			int pID;
    	switch(pID = fork()) 
			{
					case -1: 
							// Add your code here (error)
							printf("Error\n");
							exit(1);
							/* 
							your code
							*/
					case 0 :
							// Child Process
							setpgrp();
							int my_pID = (int)getpid();
							job new_job = new job(my_pID, args[0]);
							job_list.push_back(new_job);
							execvp(args[0],args);
							printf("Error\n");
							exit(1);
					
					default:
							break;
					
			}
	}
	return -1;
}

