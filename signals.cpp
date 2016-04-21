// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.hpp"

using namespace std;
using std::list;




//********************************************
// function name: sighandler
// Description:  function that sets the signal handlers
// Parameters: int signum - signal
//			   SIG_HANDLER_PTR func_ptr - pointer to function that handles our signal
// Returns: void
//**************************************************************************************


void sighandler(int signum, SIG_HANDLER_PTR func_ptr) {
	struct sigaction act;
	act.sa_handler = func_ptr;
	if (sigaction(signum, &act, NULL)) //set out signal handler function to a given signal
	{
		perror("Error: ");
		exit(1);
	}
}



//********************************************
// function name: ctrl_c
// Description:  SIGINT handler function
// Parameters: int signum - signal
// Returns: void
//********************************************

void ctrl_c(int signum){

	if (GPid == -1) //no process in foreground
		return;

	if (kill(GPid, SIGINT))
	{
		perror("Error: ");
		printf("Error failed to send SIGINT to %d\n", GPid);
		return;
	}

	printf("signal SIGINT was sent to pid %d\n", GPid);
	GPid = -1;
	return;

}





//********************************************
// function name: ctrl_z
// Description: SIGSTP handler function
// Parameters: int signum - signal
// Returns: void
//********************************************

void ctrl_z(int signum){

	if (GPid == -1) //no process in foreground
		return;

	if (kill(GPid,SIGTSTP))
	{
		perror("Error: ");
		printf("Error failed to send SIGTSTP to %d\n",GPid);
		return ;
	}

	/********put process to a job list*******/
    char* name=(char*)malloc(sizeof(L_Fg_Cmd));
    strcpy(name, L_Fg_Cmd);
    //cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;
	job newjob(GPid, suspended, name);
	job_list->push_back(newjob);

	printf("signal SIGTSTP was sent to pid %d\n", GPid);
	GPid = -1;
	return;

}


void sigchld_handler(int signum) //cathing sigchild when child dies
{
    int pid;
    int   status;
 /*   if(GPid_2 != -1)
    {
        cout << "We get here"<<endl;
        job thisJob = job_list->back();
        free(thisJob.GetName());
        job_list->pop_back();
        GPid_2 = -1;
    }*/
       /* mypid = getpid();
         for(std::list<job>::iterator i = (job_list)->begin(); i!=(job_list)->end(); i++){
            if(){
            cout<<"Ivan is right"<<endl;
                free((*i).GetName());
                job_list->erase(i);
                break;*/

        while ((pid = waitpid(-1,NULL,WNOHANG))>0){
        remove_by_pid(pid);

    }



