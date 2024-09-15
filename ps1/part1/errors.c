#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    
    int errno;
    /*int cl = close(-1);*/
    int fd = open("penis.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd, NULL, 5);

    printf("error is: %d\n", errno);
    printf("error: %s\n", strerror(errno));

    return 0;
}
