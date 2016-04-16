//		commands.c
//********************************************
#include "commands.hpp"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************


using std::list;


int ExeCmd(void* jobs, char* lineSize, char* cmdString, list<Var>& var_list, list<job>& job_list)
{
	char* cmd; 
	char* args[MAX_ARG];
	char* pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	// ADDED BY US
	char* fwd[MAX_LINE_SIZE]; //Former Working Directory
	getcwd(fwd,MAX_LINE_SIZE); //At the start they are the same
	getcwd(pwd,MAX_LINE_SIZE);
	// ADDED BY US
	bool illegal_cmd = FALSE; // illegal command
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
		int counter = 1;
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
	else if (!strcmp(cmd, "fg")) 
	{
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		
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
					
					/* 
					your code
					*/
        	case 0 :
                	// Child Process
               		setpgrp();
					if(ExeComp(cmdString) == -1) //Complicated command
					{
						
					}
					elseif(BgCmd(args,job_list) == -1) //Do regular in the background
					{
						execvp(args[0],args_passing); //Do regular in the foreground
					}
					exit(-1);
			
			default:
					
			
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize, char* args[MAX_ARG])
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
		
		bool bg_condition = false;
		for(int i=1; i<MAX_ARGS, i++)
		{
			if(!strcmp(args[i],"&")){bg_condition = true;}
		}
		if (bg_condition=true) // Run complicated in bg
		{
			//command_line[strlen(command_line)-2] = '\0';
			int my_pid = (int)getpid();
			job new_job = new job(my_pid,args[0]);
			char* new_command[MAX_ARG];
			command[0] = "csh";
			command[1] = "-f";
			command[2] = "-c";
			command[3] = lineSize;
			command[4] = NULL; 
			execvp(command[0],command);
			return 1;
			exit(1);
		}
		else //run in fg
		{
			//command_line[strlen(command_line)-2] = '\0';
			char* new_command[MAX_ARG];
			command[0] = "csh";
			command[1] = "-f";
			command[2] = "-c";
			command[3] = lineSize;
			command[4] = NULL; 
			execvp(command[0],command);
			return 1;
			exit(1);
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
int BgCmd(char *args[MAX_ARG], list<job>& job_list)
{
	bool bg_condition = false;
	for(int i=1; i<MAX_ARGS, i++)
	{
		if(!strcmp(args[i],"&")){bg_condition = true;}
	}
	if (bg_condition=true)
	{
		//command_line[strlen(command_line)-2] = '\0';
		int my_pid = (int)getpid();
		job new_job = new job(my_pid,args[0]);
		execvp(args[0],args);
		return 1;
		exit(1);
	}
	return -1;
}

