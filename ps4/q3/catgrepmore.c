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

    // for all infiles
    for(int i = 2; i < argc; i++) {

        int g_pipe[2]; // file descriptors for read and write ends of grep pipe
        int m_pipe[2]; // file descriptors for read and write ends of more pipe
                      
        // open input file and set it to write end of first pipe
        if((g_pipe[0] = open(argv[i], O_RDONLY)) < 0) {
            fprintf(stderr, "Could not open file %s: %s", argv[i], strerror(errno)); 
            continue;
        }

        // create pipes
        if(pipe(g_pipe) < 0) {
            fprintf(stderr, "Error making grep pipe: %s", strerror(errno)); 
            return -1;
        }
        if(pipe(m_pipe) < 0) {
            fprintf(stderr, "Error making more pipe: %s", strerror(errno)); 
            return -1;
        }


        // create child processes
        pid_t g_pid = 0;
        pid_t m_pid = 0; 
        
        if((g_pid = fork()) < 0) {
            fprintf(stderr, "Fork for grep pipe failed: %s", strerror(errno));
            return -1;
        }
        // more child
        if(g_pid == 0) {
            // do some error checking with pipes and then execlp grep with pattern
            // close ends of pipe
            // then dup2 fds to pipe ends
        }

        
        if((m_pid = fork()) < 0) {
            fprintf(stderr, "Fork for grep pipe failed: %s", strerror(errno));
            return -1;
        }
        // more child
        if(m_pid == 0) {
            // do some error checking with pipes and then execlp more with no args
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

        // while there are characters to read in the file put them in the buffer
        ssize_t n_read = 0, n_write = 0;
        while ((n_read = read(g_pipe[0], buf, BUF_SIZE)) > 0) {

            // write to one end of the pipe but do this after dups
            /*n_write = write(of_fd, buf, n_read);*/
            if (n_write != n_read) {
                /*fprintf(stderr, "Error writing to output file %s: %s\n", of_name, strerror(errno));*/
                return -1;
            }
        }


        // check if there were errors reading from file
        if (n_read < 0) {
            fprintf(stderr, "Error reading from %s: %s\n", strcmp(argv[i], "-") == 0 ? "stdin" : argv[i], strerror(errno));
            return -1;
        }
    }

    free(buf);

    return 0;
}
