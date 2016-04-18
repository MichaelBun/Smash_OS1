#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.hpp"

typedef void(*SIG_HANDLER_PTR)(int signum);

void ctrl_c(int sig_num);
void ctrl_z(int sig_num);
void sighandler(int , SIG_HANDLER_PTR );




#endif

