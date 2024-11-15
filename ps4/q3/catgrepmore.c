#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/types.h>

#define BUF_SIZE 8

int total_read = 0;
int total_files = 0;
sigjmp_buf jump_buffer;

void sig1_handler(int s) {
    if(s == SIGUSR1) fprintf(stderr, "\nSIGUSR1 Recieved: %d files recieved and %d bytes processed so far\n", total_files, total_read);
}

// i think we use a jump here
void sig2_handler(int s) {
    if(s == SIGUSR2) {
        fprintf(stderr, "\nSIGUSR2 Recieved - moving to the next file\n");
        siglongjmp(jump_buffer, 1);
    }
}


int main(int argc, char* argv[]) {

    // too few arguments
    if(argc < 3) {
        fprintf(stderr, "Please enter more than one argument! syntax: catgrepmore pattern infile1 [...infile2...]\n");
        return -1;
    }

    // allocate memory for the buffer
    char* buf = malloc(BUF_SIZE);
    if(buf == NULL) {
        fprintf(stderr, "Error allocating memory for buf!\n");
        return -1;
    }

    struct sigaction sa_usr1;
    sa_usr1.sa_handler = sig1_handler;
    sa_usr1.sa_flags = SA_RESTART;
    sigemptyset(&sa_usr1.sa_mask);
    if(sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        fprintf(stderr, "Error creating sigaction for SIGUSR1: %s\n", strerror(errno)); 
        return errno;
    }

    struct sigaction sa_usr2;
    sa_usr2.sa_handler = sig2_handler;
    sa_usr2.sa_flags = 0;
    sigemptyset(&sa_usr2.sa_mask);
    if(sigaction(SIGUSR2, &sa_usr2, NULL) < 0) {
        fprintf(stderr, "Error creating sigaction for SIGUSR2: %s\n", strerror(errno)); 
        return errno;
    }
    
    // pattern in first argument
    char* pattern = argv[1];

    // for all infiles
    for(int i = 2; i < argc; i++) {

        // set long jump point
        if(sigsetjmp(jump_buffer, 1) == 0) {

            total_files++;

            // note to self: pipe[0] = read, pipe[1] = write
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
                return errno;
            }
            if(pipe(m_pipe) < 0) {
                fprintf(stderr, "Error making more pipe: %s\n", strerror(errno)); 
                return errno;
            }


            // create child processes
            pid_t g_pid = 0;
            pid_t m_pid = 0; 
            
            if((g_pid = fork()) < 0) {
                fprintf(stderr, "Fork for grep pipe failed: %s\n", strerror(errno));
                return errno;
            }
            // grep child
            if(g_pid == 0) {

                // close unused file descriptors  
                if(close(g_pipe[1]) < 0) {
                    fprintf(stderr, "Error while grepping - closing write side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(m_pipe[0]) < 0) {
                    fprintf(stderr, "Error while grepping - closing read side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // set read side of grep pipe to stdin
                if(dup2(g_pipe[0], STDIN_FILENO) < 0) {
                    fprintf(stderr, "Error while grepping - duping read side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                // set write side of grep pipe to stdout
                if(dup2(m_pipe[1], STDOUT_FILENO) < 0) {
                    fprintf(stderr, "Error while grepping - duping write side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // close duplicated descriptors
                if(close(g_pipe[0]) < 0) {
                    fprintf(stderr, "Error while grepping - closing old read side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(m_pipe[1]) < 0) {
                    fprintf(stderr, "Error while grepping - closing old write side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // run grep which will read from STDIN since no file specified i think (i asked and u said yes)
                if(execlp("grep", "grep", pattern, NULL) < 0) {
                    fprintf(stderr, "Error executing grep: %s\n", strerror(errno));
                    return errno;
                }
            }

            
            if((m_pid = fork()) < 0) {
                fprintf(stderr, "Fork for more pipe failed: %s\n", strerror(errno));
                return errno;
            }
            // more child
            if(m_pid == 0) {
                
                // close unused file descriptors
                if(close(g_pipe[0]) < 0) {
                    fprintf(stderr, "Error while moring - closing read side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(g_pipe[1]) < 0) {
                    fprintf(stderr, "Error while moring - closing write side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(m_pipe[1]) < 0) {
                    fprintf(stderr, "Error while moring - closing write side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // set read side of grep pipe to stdin
                if(dup2(m_pipe[0], STDIN_FILENO) < 0) {
                    fprintf(stderr, "Error while moring - duping read side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // close duplicated file descriptor
                if(close(m_pipe[0]) < 0) {
                    fprintf(stderr, "Error while moring - closing old read side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                if(execlp("more", "more", NULL) < 0) {
                    fprintf(stderr, "Error executing more: %s\n", strerror(errno));
                    return errno;
                }
            }

            // in parent process
            if(m_pid > 0) {
                
                // close ends of pipes we're not using to ensure EOF is properly sent to more process
                // when the parent closes
                if(close(g_pipe[0]) < 0) {
                    fprintf(stderr, "Error in parent - closing read side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(m_pipe[0]) < 0) {
                    fprintf(stderr, "Error in parent - closing read side of more pipe: %s\n", strerror(errno));
                    return errno;
                }
                if(close(m_pipe[1]) < 0) {
                    fprintf(stderr, "Error in parent - closing write side of more pipe: %s\n", strerror(errno));
                    return errno;
                }

                // read from input and write to output wihile having proper read/write handling
                ssize_t n_read = 0, n_write = 0;

                while(1) {
                    total_read += n_read = read(infile_fd, buf, BUF_SIZE);
                    if(n_read == 0) break; // hit EOF
                    else if(n_read < 0) {
                        if(errno == EINTR) continue; // if interrupted try read again
                        else if(errno == EPIPE) break; // is this what u mean by the grep program dies on a broken pipe #2
                        else {
                            fprintf(stderr, "Error reading from infile %s: %s\n", argv[i], strerror(errno));
                            break; // just move onto next file if we can't resolve
                        }
                    }
                    
                    // keep track of total bytes written to grep pipe so we can save our position
                    // in case an error occurs 
                    int total_written = 0;
                    while(total_written < n_read) {
                        total_written += n_write = write(g_pipe[1], buf + total_written, n_read - total_written);
                        if (n_write < 0) {
                            if(errno == EINTR) continue; // if interrupted try write again but write from where left off
                            else {
                                fprintf(stderr, "Error writing to grep pipe from infile %s: %s\n", argv[i], strerror(errno));
                                return errno;
                            }
                        }
                    }
                }
                
                // close input file and grep read end
                if(close(infile_fd) < 0) {
                    fprintf(stderr, "Error in parent - closing input file descriptor: %s\n", strerror(errno));
                    return errno;
                }

                if(close(g_pipe[1]) < 0) {
                    fprintf(stderr, "Error in parent - closing read write side of grep pipe: %s\n", strerror(errno));
                    return errno;
                }
                
                // variables for return code even tho we don't need 
                // to stop the buildup of zombie processes
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
    }

    // all done!
    return 0;
}
