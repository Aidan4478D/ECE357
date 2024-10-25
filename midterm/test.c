#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int ret = 0;

    if(fork() == 0) {
        return 139;
    }

    if(wait(&ret) < 0) {
        fprintf(stderr, "an error has occured: %s\n", strerror(errno)); 
        return errno;
    }

    fprintf(stderr, "ret is %d\n", ret); 

    return 0;
}
