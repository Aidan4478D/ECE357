#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "headers/commands.h"


int cd(char* d_name) {
    
    int ret = 0;
    if((ret = chdir(d_name)) < 0) {
        fprintf(stderr, "error changing directories to %s: %s", d_name, strerror(errno));
        return errno;
    }

    return ret;
}

int pwd() {

    char* path_name;

    if((path_name = getcwd(NULL, 0)) == NULL) {
        fprintf(stderr, "error getting the current working directory path: %s", strerror(errno));
        return errno;
    }

    // print current working directory
    printf("%s\n", path_name);
    free(path_name);

    return 0;
}

/*int exit(int e_status) {*/

    /*printf("exit was run!!!");*/
/*}*/

int general_command(char* command, Queue* args, Queue* io) {

    /*fprintf(stderr, "command is: %s\n", command);*/
    /*printf("size of args is: %d\n", get_size(args));*/
    /*printf("size of io is: %d\n", get_size(io));*/
    
    int ret = 0;
    pid_t pid = 0;
    
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork failed: %s", strerror(errno)); 
        return errno; 
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

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


    // Calculate real time in milliseconds
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double real_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    // Get CPU times using getrusage
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);

    // user time 
    double user_time_ms = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000.0;
    double system_time_ms = usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000.0;



    if (WIFEXITED(ret)) printf("child exited with status %d\n", WEXITSTATUS(ret));
    else if(WIFSIGNALED(ret)) printf("child killed by signal %d\n", WTERMSIG(ret));
    else printf("child did not exit normally\n");

    printf("Real time elapsed: %.3f ms\n", real_time_ms);
    printf("User CPU time: %.3f ms\n", user_time_ms);
    printf("System CPU time: %.3f ms\n", system_time_ms);

    return ret;

}
