#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 20

int process_argument(char* filename, char* buf, int total_bytes) {
    
    int fd, n = 0;

    //if input is '-' read from stdin
    if(strcmp(filename, "-") == 0) {
        n = read(0, buf, BUF_SIZE - total_bytes);
    }

    //otherwise open file
    else {

        //if the file name exists read from it and save contents to buffer
        if((fd = open(filename, O_RDONLY)) >= 0) {
            n = read(fd, buf, BUF_SIZE - total_bytes);
            close(fd);
        }

        //otherwise report an error
        else {
            fprintf(stderr, "Error opening file %s for reading: %s\n", filename, strerror(errno));
        }
    }
    
    //return number of bytes read
    return n;
}

int main(int argc, char *argv[]) {

    char buf[BUF_SIZE];
    int total_bytes = 0;

    if(argc < 2) printf("Please enter a valid file or '-' for manual input!\n");
    
    //process all of the arguments and update total btyes read
    for(int i = 1; i < argc; i++) {
        total_bytes += process_argument(argv[i], buf + total_bytes, total_bytes);
    }


    printf("%d total bytes\n", total_bytes);
    if(total_bytes > 0) write(1, buf, total_bytes);

    return 0;
}
