#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void f1() {

    static int i;
    printf("%d\n", i);
    i++;
}

int main() {

    int ws;
    f1(;

    if(fork() == 0)
        f1();
    
    f1();

    if(wait(&ws) < 0) {
        printf("%d is ws and program done and errno is %d: %s\n", ws, errno, strerror(errno));
        return errno;
    }
    printf("%d is ws pre math\n", ws);

    ws = ws >> 8;
    printf("%d is ws>>8\n", ws);

    ws = ws & 255;
    printf("%d is ws&255\n", ws);

    /*ws = (ws >> 8) & 255;*/
    printf("%d is ws\n", ws);

    return ws;
}
