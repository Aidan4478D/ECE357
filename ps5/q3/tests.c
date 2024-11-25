#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/mman.h>

#include "tests.h"

int test1() {
    fprintf(stderr, "Executing Test #1...\n"); 

    char* map;
    int fd;

    if((fd = open("/dev/zero", O_RDONLY)) < 0) {
        fprintf(stderr, "Error creating anonymous file for test 1: %s", strerror(errno));
    }

    // addr = NULL so kernel chooses addres
    // length = 4096 -> page aligned
    // offset = 0
    if((map = mmap(NULL, PAGE_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "mmap failed in test 1: %s\n", strerror(errno));
        return errno; 
    }
    close(fd);

    map[3] = 'A';
    fprintf(stderr, "map[3] == '%c'\n", map[3]);

    // change protection to read-only
    if (mprotect(map, PAGE_LEN, PROT_READ) != 0) {
        fprintf(stderr, "Changing permissions in test 1 using mprotect failed: %s\n", strerror(errno));
        munmap(map, PAGE_LEN);
        return errno;
    }

    fprintf(stderr, "writing a 'B'\n");
    map[3] = 'B';

    if(map[3] == 'B') {
        fprintf(stderr, "Write succedded in test 1\n");
        munmap(map, PAGE_LEN); 
        return 0;
    }
    else {
        fprintf(stderr, "Write failed in test 1: %s\n", strerror(errno));
        munmap(map, PAGE_LEN); 
        return errno;
    }
}

int test2() {
    fprintf(stderr, "test is 2\n"); 
}

int test3() {
    fprintf(stderr, "test is 3\n"); 
}

int test4() {
    fprintf(stderr, "test is 4\n"); 
}

