#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "queue.h"
#define PWD_PATHNAME_SIZE 4096

// built in commands (no io redirection)
int cd (char* d_name);
int pwd ();
void exit (int e_status);

// other commands
int general_command(char* command, Queue* args, Queue* io); 

#endif
