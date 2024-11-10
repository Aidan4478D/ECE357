#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include <sys/types.h>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {

    //allocate memory for the buffer

    char* buf = malloc(BUF_SIZE);
    if(buf == NULL) fprintf(stderr, "Error allocating memory for buf!");

    if(argc < 3) fprintf(stderr, "Please enter more than one argument! syntax: catgrepmore pattern infile1 [...infile2...]\n");
    
    //pattern in first argument
    char* pattern = argv[1];
    int g_ret = 0;
    int m_ret = 0;
    int total_read = 0;

    // for all infiles
    for(int i = 2; i < argc; i++) {

        int g_pipe[2]; // file descriptors for grep pipe
        int m_pipe[2]; // file descriptors for more pipe
        
        int infile_fd = 0;
                      
        // open input file and set it to write end of first pipe
        if((infile_fd = open(argv[i], O_RDONLY)) < 0) {
            fprintf(stderr, "Could not open file %s: %s\n", argv[i], strerror(errno)); 
            continue;
        }

        // create pipes
        if(pipe(g_pipe) < 0) {
            fprintf(stderr, "Error making grep pipe: %s\n", strerror(errno)); 
            return -1;
        }
        if(pipe(m_pipe) < 0) {
            fprintf(stderr, "Error making more pipe: %s\n", strerror(errno)); 
            return -1;
        }


        // create child processes
        pid_t g_pid = 0;
        pid_t m_pid = 0; 
        
        if((g_pid = fork()) < 0) {
            fprintf(stderr, "Fork for grep pipe failed: %s\n", strerror(errno));
            return -1;
        }
        // grep child
        if(g_pid == 0) {
            if(close(g_pipe[1]) < 0) {
                fprintf(stderr, "Error closing write side of grep pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(m_pipe[0]) < 0) {
                fprintf(stderr, "Error closing read side of grep pipe: %s\n", strerror(errno));
                return -1;
            }

            // set read side of grep pipe to stdin
            if(dup2(g_pipe[0], STDIN_FILENO) < 0) {
                fprintf(stderr, "Error duping read side of grep pipe: %s\n", strerror(errno));
                return -1;
            }

            // set write side of grep pipe to stdout
            if(dup2(m_pipe[1], STDOUT_FILENO) < 0) {
                fprintf(stderr, "Error duping write side of more pipe: %s\n", strerror(errno));
                return -1;
            }

            execlp("grep", "grep", pattern, NULL);
            fprintf(stderr, "Error executing more: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        
        if((m_pid = fork()) < 0) {
            fprintf(stderr, "Fork for grep pipe failed: %s\n", strerror(errno));
            return -1;
        }
        // more child
        if(m_pid == 0) {
            // do some error checking with pipes and then execlp more with no args
            if(close(g_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of grep pipe: %s\n", strerror(errno));
                return -1;
            }
            // set read side of grep pipe to stdin
            if(dup2(m_pipe[0], 0) < 0) {
                fprintf(stderr, "Error duping read side of more pipe: %s\n", strerror(errno));
                return -1;
            }

            // set write side of grep pipe to stdout
            // standard output is not modified?
            //
            /*if(dup2(m_pipe[1], 1) < 0) {*/
                /*fprintf(stderr, "Error duping write side of grep pipe: %s\n", strerror(errno));*/
                /*return -1;*/
            /*}*/

            execlp("more", "more", NULL);
        }

        // in parent process
        if(m_pid > 0) {
            
            // while there are characters to read in the file put them in the buffer
            ssize_t n_read = 0, n_write = 0;

            // read from input file
            // maybe add something with signal handling with EINTR or SA_RESTART
            while ((total_read += n_read = read(infile_fd, buf, BUF_SIZE)) > 0) {

                // write to write end of grep pipe
                n_write = write(g_pipe[1], buf, n_read);
                if (n_write != n_read) {
                    fprintf(stderr, "Error writing to grep pipe from infile %s: %s\n", argv[i], strerror(errno));
                    return -1;
                }
            }

            if(close(infile_fd) < 0) {
                fprintf(stderr, "Error closing input file descriptor: %s\n", strerror(errno));
                return -1;
            }
            // close write side of pipe
            if(close(g_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(g_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of pipe: %s\n", strerror(errno));
                return -1;
            }


            // wait for child processes to die
            if(waitpid(g_pid, &g_ret, 0) < 0) {
                fprintf(stderr, "Error waiting for grep child process: %s\n", strerror(errno)); 
                return errno;
            }
            if(waitpid(m_pid, &m_ret, 0) < 0) {
                fprintf(stderr, "error waiting for more child process: %s\n", strerror(errno)); 
                return errno;
            }
        }
    }

    free(buf);

    return 0;
}
