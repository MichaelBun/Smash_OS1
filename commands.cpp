//		commands.c
//********************************************
#include "commands.hpp"
#include <sys/stat.h>
#include <string>
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************

using namespace std;
using std::list;
using std::string;

void remove_elem_from_jobs(int );


int ExeCmd(char* lineSize, list<Var*>& var_list, list<job>& job_list, char* fwd, char* pwd)
{
	char* cmd;
	char* args[MAX_ARG];
	string delim_ = " \t\n";
	char* delimiters=(char*)malloc(sizeof(char*)*delim_.length());
	strcpy(delimiters,delim_.c_str());
	int i = 0, num_arg = 0;

	getcwd(pwd,MAX_LINE_SIZE);


	bool illegal_cmd = false; // illegal command

	cmd = strtok(lineSize, delimiters); //command
	if (cmd == NULL) {
        perror("Can't read the command");
		return 0;
    }
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
            //char* fwd_ptr;
            //fwd_ptr=&fwd[1];

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
				return -1;
			}
			printf("smash error: > “%s” – cannot create directory\n", args[1]);
		}
	}
	/*************************************************/

	else if (!strcmp(cmd, "set"))
	{
        if(num_arg!=2){
            cout<<"Num of parameters is wrong"<<endl;
            return -1;
        }
        for(std::list<Var*>::iterator i = var_list.begin(); i!=var_list.end(); i++) //check if var alreardy in list
            {
                if(!strcmp((*i)->name, args[1])){
                    free((*i)->value);
                    (*i)->value = (char*)malloc(sizeof(args[2]));
                    strcpy((*i)->value, args[2]);
                    return 0;
                }
            }
		Var* new_var = (Var*)malloc(sizeof(Var));
		new_var->name = (char*)malloc(sizeof(args[1]));
		new_var->value = (char*)malloc(sizeof(args[2]));
		strcpy(new_var->name, args[1]);
		strcpy(new_var->value, args[2]);
		var_list.push_back(new_var);

	}
	/*************************************************/

	else if (!strcmp(cmd, "unset"))
	{
        if(num_arg!=1){
            cout<<"Num of parameters is wrong"<<endl;
            return 0;
        }
		int flag=-1;
 		for(std::list<Var*>::iterator i = var_list.begin(); i!=var_list.end(); i++)
		{
			if(!strcmp((*i)->name,args[1]))
			{
				flag=0; //var found
				free((*i)->name);
				free((*i)->value);
				free((*i));
				var_list.erase(i);
				return 0;
			}
		}
 		if(flag!=0)
		printf("smash error: > “%s” - variable not found\n",args[1]);
	}
	/*************************************************/

	else if (!strcmp(cmd, "show"))
	{
        if(var_list.empty()) cout<<"No parameters to show"<<endl;
		if(num_arg==0) //Print all
		{
			for(std::list<Var*>::iterator i = var_list.begin(); i!=var_list.end(); i++)
			{
				printf("%s := %s\n",(*i)->name,(*i)->value);
			}
		}
		else //Print only one
		{
			for(std::list<Var*>::iterator i = var_list.begin(); i!=var_list.end(); i++)
			{
				if(!strcmp((*i)->name,args[1]))
				{
					printf("%s := %s\n",(*i)->name,(*i)->value);
					return 0;
				}
				cout<<"Parameter not found"<<endl;
			}

		}

		//TODO what if varname not found??

	}
	/*************************************************/


	else if (!strcmp(cmd, "jobs"))
	{
		int counter=1;
 		for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++)
		{
			int pid = i->GetPid();
			int time = i->GetTime();
			char* name = i->GetName();
			printf("[%d] %s : %d %d secs",counter,name,pid,time);
			if((i->GetStatus())==suspended) cout<<" (Stopped)"<<endl;
			else cout<<endl;
			counter++;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		int my_pid = (int)getpid();
		printf("smash pid is %d\n", my_pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
        if(num_arg!=2){
            cout<<"Num of parameters is wrong"<<endl;
            return -1;
        }
		int job_num = atoi(args[2]);

   		if(job_num>job_list.size() || job_list.empty() == true) // job does not exist
		{
			printf("smash error: > kill %s – job does not exist\n",args[2]);
		}
		else
		{
			int system_call;
			//TODO what if signum is not int
			system_call = (-1)*atoi(args[1]);
			int counter = 1;
			for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++)
			{
				if(counter == job_num ) // This is the one we need
				{
					if(kill((*i).GetPid(),system_call) == -1) // we failed in sending the sig
					{
						printf("smash error: > kill %s – cannot send signal\n",args[2]);
					}
					else
					{
						if(system_call == SIGSTOP || system_call == SIGTSTP) //We want to suspend
						{
							 //This process is now suspended
							(*i).ChangeStatus(suspended);
						}
						else if(system_call == SIGCONT)
						{
							(*i).ChangeStatus(working);
						}
						else if(system_call == SIGKILL || system_call == SIGQUIT) // We want to delete the item from the job list
						{
							delete((*i).GetName());
							job_list.erase(i);
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
		for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++)
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
							GPid = job_pid ;
                            strcpy(L_Fg_Cmd, i->GetName()); //copy name of last process that sent to fg
							free(i->GetName());
							job_list.erase(i);
							waitpid(job_pid,NULL,WUNTRACED); // wait untill we done
							GPid = -1;
						}
					}
					else //it was running in the background
					{
						GPid=job_pid ;
						strcpy(L_Fg_Cmd, i->GetName()); //copy name of last process that sent to fg
						free((*i).GetName());
						job_list.erase(i);
						waitpid(job_pid,NULL,WUNTRACED); // wait untill we done
						GPid = -1;
					}
					break;
				}
				counter++;
			}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		int job_num = atoi(args[1]);
		int counter = 1;
		for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++)
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
		//time_t start_time[job_list.size()];
		if(num_arg == 0) //regular quit
		{
            for(int i=1; i<job_list.size()+1; i++){//free memory
                remove_elem_from_jobs(i);
                free(L_Fg_Cmd);
            }
			exit(0);
		}
		if(strcmp(args[1], "kill")==0 && num_arg==1) //quit kill
		{
            if(job_list.empty()) exit(0);//true if list is empty

            int counter=1;
            bool flag=false; //if sigterm was sent flag=true

            for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++) //TODO check if we delet the last element
			{
                if(kill((*i).GetPid(),SIGTERM)==-1)
				{
					printf("Error with sending signal\n");
				}
				else
				{
                    int start_time = (int)time(NULL);

                    printf("[%d] %s - ",counter,(*i).GetName());
                    cout<<"Sending SIGTERM... ";

                    while((int)(time(NULL)-start_time) < 5){

                        if(waitpid((*i).GetPid(),NULL,WNOHANG)){

                            flag=true;
                            free((*i).GetName());
							job_list.erase(i);
                            cout<<"Done."<<endl;
                            break;
                        }

                    }
                    if(flag!=true){

                        cout<<"(5 sec passed) Sending SIGKILL... ";
                        if(kill((*i).GetPid(),SIGKILL)){
                            perror("Error: ");
							printf("Error failed to send SIGKILL");
							exit(-1);
                        }
                        cout<<"Done."<<endl;
                        continue; //for. get next parameter from list
                    }


				}
            }//for

			exit(0);
		}//if(args[1] == "kill")
	}

	/*************************************************/
	else // external command
	{
 		ExeExternal(args, job_list);
	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", lineSize);
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
void ExeExternal(char *args[MAX_ARG], list<job>& job_list)
{
	int pID;
    	switch(pID = fork())
	{
    		case -1:
					// Add your code here (error)
					perror("Error\n");
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
					GPid = pID;
                    strcpy(L_Fg_Cmd, args[0]); //copy name of last process that sent to fg
					waitpid(pID,NULL,WUNTRACED);
					GPid = -1;
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
	//char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
	char* new_command[MAX_ARG];
	string _new_command_0 = "csh"; //So C++ wont be mad about using char*
	string _new_command_1 = "-f";
	string _new_command_2 = "-c";

    new_command[0]=(char*)malloc(sizeof(char)*_new_command_0.length());
    new_command[1]=(char*)malloc(sizeof(char)*_new_command_1.length());
    new_command[2]=(char*)malloc(sizeof(char)*_new_command_2.length());

	strcpy(new_command[0],_new_command_0.c_str());
	strcpy(new_command[1],_new_command_1.c_str());
	strcpy(new_command[2],_new_command_2.c_str());
	new_command[3] = lineSize;
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
					GPid = pID;
					waitpid(pID,NULL,WUNTRACED);
					GPid = -1;
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
	char* cmd;
	char* delimiters = strdup(" \t\n");
	char* args[MAX_ARG];
	char* ampercent = strdup("&");
	char* empty = strdup("\n");
	int num_arg;
	if(linesize[strlen(linesize)-2] == *ampercent)
	{
		linesize[strlen(linesize)-2] = *empty; //remove &

        cmd = strtok(linesize, delimiters); //command
        if (cmd == NULL) {
            perror("Can't read the command");
            return 0;
        }
        args[0] = cmd;

        for (int i=1; i<MAX_ARG; i++)
        {
            args[i] = strtok(NULL, delimiters);
            if (args[i] != NULL)
                num_arg++;

        }
            

            int pID;
            switch(pID = fork())
                {
                        case -1:
                            // Add your code here (error)
                                perror ("Error BgCmd fork\n");
                                break;

                        case 0 :
						{		// Child Process
                                setpgrp();
                                if(execvp(args[0],args)<0){
									exit(1);
                                }
						}

                       default:
								jobStatus status = working;
                                char* procc_name = (char*)malloc(sizeof(char)*strlen(args[0]));
                                strcpy(procc_name,args[0]);
								int my_pID = (int)getpid();
                                job new_job(my_pID, status, procc_name);
                                job_list.push_back(new_job);
								if(kill(pID,0) ==0){}
								else if (errno == ESRCH) //NO SUCH PROCESS
								{
									printf("Unknown command: %s\n", cmd);
									job Free_this = job_list.back();
									free(Free_this.GetName());
									job_list.pop_back();
								}
								else
								{
									printf("Unknown command: %s\n", cmd);
									job Free_this = job_list.back();
									free(Free_this.GetName());
									job_list.pop_back();
								}
							return(0);
                }
	}
	return -1;
}

void remove_elem_from_jobs(int elem_num){
    int counter = 1;
    for(std::list<job>::iterator i = job_list.begin(); i!=job_list.end(); i++)
    {
        if(counter == elem_num){
            free((*i).GetName());
            job_list.erase(i);
         //   free(L_Fg_Cmd);
        }
    }
}

