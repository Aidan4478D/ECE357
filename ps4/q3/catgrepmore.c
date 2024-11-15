#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

#include <sys/types.h>

#define BUF_SIZE 4096

int total_read = 0;
int total_files = 0;

void sig1_handler(int s) {
    if(s == SIGUSR1) fprintf(stderr, "SIGUSR1 Recieved: %d files recieved and %d bytes processed so far", total_files, total_read);
}

// i think we use a jump here
void sig2_handler(int s) {
    if(s == SIGUSR2) fprintf(stderr, "SIGUSR2 Recieved - moving to the next file");
}


int main(int argc, char* argv[]) {

    // too few arguments
    if(argc < 3) fprintf(stderr, "Please enter more than one argument! syntax: catgrepmore pattern infile1 [...infile2...]\n");

    // allocate memory for the buffer
    char* buf = malloc(BUF_SIZE);
    if(buf == NULL) fprintf(stderr, "Error allocating memory for buf!");

    struct sigaction sa_usr1;
    sa_usr1.sa_handler = sig1_handler;
    sa_usr1.sa_flags = SA_RESTART;
    sigemptyset(&sa_usr1.sa_mask);
    if(sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        fprintf(stderr, "Error creating sigaction for SIGUSR1: %s", strerror(errno)); 
        return -1;
    }

    struct sigaction sa_usr2;
    sa_usr2.sa_handler = sig2_handler;
    sa_usr2.sa_flags = 0;
    sigemptyset(&sa_usr2.sa_mask);
    if(sigaction(SIGUSR2, &sa_usr2, NULL) < 0) {
        fprintf(stderr, "Error creating sigaction for SIGUSR2: %s", strerror(errno)); 
        return -1;
    }
    
    // pattern in first argument
    char* pattern = argv[1];

    // for all infiles
    for(int i = 2; i < argc; i++) {

        total_files++;

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

            // close read FD of grep pipe and write FD of more pipe
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

            // close duplicated descriptors
            if(close(g_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of grep pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(m_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of grep pipe: %s\n", strerror(errno));
                return -1;
            }


            if(execlp("grep", "grep", pattern, NULL) < 0) {
                fprintf(stderr, "Error executing grep: %s\n", strerror(errno));
                exit(127);
            }
        }

        
        if((m_pid = fork()) < 0) {
            fprintf(stderr, "Fork for grep pipe failed: %s\n", strerror(errno));
            return -1;
        }
        // more child
        if(m_pid == 0) {
            // close unused file descriptors
            if(close(g_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of grep pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(g_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of grep pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(m_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of more pipe: %s\n", strerror(errno));
                return -1;
            }

            // set read side of grep pipe to stdin
            if(dup2(m_pipe[0], STDIN_FILENO) < 0) {
                fprintf(stderr, "Error duping write side of more pipe: %s\n", strerror(errno));
                return -1;
            }

            // close duplicated file descriptor
            if(close(m_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of more pipe: %s\n", strerror(errno));
                return -1;
            }

            if(execlp("more", "more", NULL) < 0) {
                fprintf(stderr, "Error executing more: %s\n", strerror(errno));
                exit(127);
            }
        }

        // in parent process
        if(m_pid > 0) {
            
            // close ends of pipes we're not using to ensure EOF is properly sent to more process
            // when the parent closes
            if(close(g_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(m_pipe[0]) < 0) {
                fprintf(stderr, "Error closing write side of more pipe: %s\n", strerror(errno));
                return -1;
            }
            if(close(m_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of more pipe: %s\n", strerror(errno));
                return -1;
            }


            ssize_t n_read = 0, n_write = 0;

            // read from input and write to output wihile having proper read/write handling
            while(1) {
                total_read += n_read = read(infile_fd, buf, BUF_SIZE);
                if(n_read == 0) break; // hit EOF
                else if(n_read < 0) {
                    // if interrupted while reading try again
                    if(errno == EINTR) continue;
                    else {
                        fprintf(stderr, "Error reading from infile %s: %s\n", argv[i], strerror(errno));
                        break; // just move onto next file
                    }
                }
                
                // keep track of total bytes written to grep pipe so we can save our position
                // in case an error occurs
                int total_written = 0;
                while(total_written < n_read) {
                    total_written += n_write = write(g_pipe[1], buf + total_written, n_read - total_written);
                    if (n_write < 0) {
                        // if interrupted try again
                        if(errno == EINTR) continue;
                        else {
                            fprintf(stderr, "Error writing to grep pipe from infile %s: %s\n", argv[i], strerror(errno));
                            return -1;
                        }
                    }
                }
           }
            
            

            if(close(infile_fd) < 0) {
                fprintf(stderr, "Error closing input file descriptor: %s\n", strerror(errno));
                return -1;
            }

            if(close(g_pipe[1]) < 0) {
                fprintf(stderr, "Error closing read side of pipe: %s\n", strerror(errno));
                return -1;
            }
            
            int g_ret = 0;
            int m_ret = 0;

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


    return 0;
}
