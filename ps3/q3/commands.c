#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "headers/commands.h"

int cd(char* d_name) {

    printf("cd was run!!!");
    return 0;
}

int pwd() {

    printf("pwd was run!!!");
    return 0;
}

/*int exit(int e_status) {*/

    /*printf("exit was run!!!");*/
/*}*/

int general_command(char* command, Queue* args, Queue* io) {

    fprintf(stderr, "command is: %s\n", command);
    printf("size of args is: %d\n", get_size(args));
    printf("size of io is: %d\n", get_size(io));
    
    int ret = 0;
    pid_t pid = 0;
    
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed: %s", strerror(errno)); 
        return errno; 
    }

    // in child process
    if(pid == 0) {

        // handle redirection 
        while(!is_empty(io)) {

            char* re = dequeue(io);
            int fd = 0;
            int std_re = 0;
            
            // open, create, append, redirect stderr
            if(strncmp(re, "2>>", 3) == 0) {
                re += 3;
                fd = open(re, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                std_re = 2; //stderr num
            }   

            // open, create, truncate, redirect stderr
            else if(strncmp(re, "2>", 2) == 0) {
                re += 2;
                fd = open(re, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                std_re = 2; 
            }

            // open, create, append, redirect stdout
            else if(strncmp(re, ">>", 2) == 0) {
                re += 2;
                fd = open(re, O_WRONLY | O_CREAT | O_APPEND, 0666);
                std_re = 1;
            }

            // open, create, truncate, redirect stdout
            else if(strncmp(re, ">", 1) == 0) {
                re += 1;
                fd = open(re, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                std_re = 1;

                /*printf("opened fd %d with one >\n", fd);*/
            }
            
            // input redirection
            // open and redirect stdin
            else if(strncmp(re, "<", 1) == 0) {
                re += 1;
                fd = open(re, O_RDONLY);
                std_re = 0;
            }

            else {
                fprintf(stderr, "Cannot interpret redirect path: %s. Skipping redirection\n", re); 
                exit(1); // error for invalid redirectoin
            }

            // allocate new fd for process and point it to same struct file as std_re
            /*printf("duplicated fd: %d with stdre: %d on redirect path: %s\n", fd, std_re, re);*/
            dup2(fd, std_re);
            close(fd); 
        }

        /* EXECUTE PROCESS */

        // I know this is kinda dumb to be using a queue in the first place
        // but I made it and don't want it to go to waste :(
        char* arg_list[256];
        int i = 0;
        arg_list[i++] = command;
        
        while (!is_empty(args))
            arg_list[i++] = dequeue(args);
        
        arg_list[i] = NULL; // null terminate list

        if (execvp(command, arg_list) < 0) {
            fprintf(stderr, "error executing command %s: %s\n", command, strerror(errno));
            exit(127);
        }
    }

    // waits for child process to complete 
    if(wait(&ret) < 0) {
        fprintf(stderr, "error waiting for child process: %s", strerror(errno)); 
        return errno;
    }
    
    if (WIFEXITED(ret)) printf("child exited with status %d\n", WEXITSTATUS(ret));
    else printf("child did not exit normally\n");

    return ret;

}
