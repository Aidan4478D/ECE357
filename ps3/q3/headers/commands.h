#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "queue.h"

// map names to function pointers to make this cool
typedef struct {
    char* name;
    void (*func)(void *);
} func_map;

// built in commands (no io redirection)
int cd (char* d_name);
int pwd ();
//int exit (int e_status);

// other commands
int general_command(char* command, Queue* args, Queue* io); 

#endif
