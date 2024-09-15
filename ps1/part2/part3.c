#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int process_argument(char* inf, char* buf, int buf_size, int total_bytes) {
    
    int fd, n = 0;

    //if input is '-' read from stdin
    if(strcmp(inf, "-") == 0) {
        n = read(0, buf, buf_size - total_bytes);
        if(n < 0) { 
            fprintf(stderr, "Error reading from stdin: %s\n", strerror(errno));
            return 0;
        }
    }

    //otherwise open file
    else {

        //if the file name exists read from it and save contents to buffer
        if((fd = open(inf, O_RDONLY)) >= 0) {
            n = read(fd, buf, buf_size - total_bytes);
            close(fd);
            
            if(n < 0) { 
                fprintf(stderr, "Error reading from %s: %s\n", inf, strerror(errno));
                return 0;
            }
        }

        //otherwise report an error
        else {
            fprintf(stderr, "Error opening file %s for reading: %s\n", inf, strerror(errno));
        }
    }
    
    //return number of bytes read
    return n;
}

int main(int argc, char *argv[]) {

    int total_bytes = 0;

    int buf_size = 4096;
    char* buf = malloc(buf_size);

    char* of_name = NULL;
    int of_fd = 1; //default to stdout

    if(argc < 2) printf("Please enter a valid file or '-' for manual input!\n");
    
    //process all of the arguments and update total btyes read
    for(int i = 1; i < argc; i++) {

        if(strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                of_name = argv[++i];
            } 
            else {
                fprintf(stderr, "The -o flag requires a filename argument!\n");
                return -1;
            }
        }
        else if (strcmp(argv[i], "-b") == 0) {
            if (i + 1 < argc) {
                if ((buf_size = atoi(argv[++i])) < 0) {
                    fprintf(stderr, "Please enter a valid buffer size (> 0)!\n");
                    return -1;
                }
                else {
                    free(buf);
                    buf = malloc(buf_size);
                }
            }
            else {
                fprintf(stderr, "The -b flag requires a file size argument!\n");
                return -1;
            }
        }
        else total_bytes += process_argument(argv[i], buf + total_bytes, buf_size, total_bytes);
    }
    
    if (of_name != NULL) {
        of_fd = open(of_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (of_fd < 0) {
            fprintf(stderr, "Error opening output file %s: %s\n", of_name, strerror(errno));
            return -1;
        }
    }

    printf("%d total bytes\n", total_bytes);
    if (total_bytes > 0) write(of_fd, buf, total_bytes);
    //write error checks here

    if (of_fd != 1) close(of_fd);
    free(buf);

    return 0;
}
