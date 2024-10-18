#ifndef _COMMANDS_H
#define _COMMANDS_H

// map names to function pointers to make this cool
typedef struct {
    char* name;
    void (*func)(void *);
} func_map;

// built in commands
int cd (char* d_name);
int pwd ();
//int exit (int e_status);

// other commands
int ls (char* d_name);

#endif
