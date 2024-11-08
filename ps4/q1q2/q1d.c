#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {

    int fds[2];
    struct stat st;

    pipe(fds);
    fstat(fds[0], &st);
    printf("%o\n", st.st_mode);
}
