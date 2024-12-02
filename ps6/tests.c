#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "headers/tests.h"

int spinlock_test() {

    shared_t *map;
    if((map = mmap(NULL, sizeof(shared_t), PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error mmapping shared data region: %s\n", strerror(errno));
        return -1;
    }

    // initialize counter to be zero and lock to be unlocked
    map->counter = 0;
    spin_unlock(&map->lock);

    pid_t pids[NUM_CORES];
    for(int i = 0; i < NUM_CORES; i++) {
        if((pids[i] = fork()) == 0) {
            for(int j = 0; j < NUM_ITERATIONS; j++) {
                spin_lock(&map->lock);      //acquire spinlock
                map->counter++;             //now in critical region and increment counter
                spin_unlock(&map->lock);    //release spinlock
                fprintf(stderr, "count is at %d\n", map->counter);
            }
            exit(0);
        }
    }

    // wait for children to finish
    for(int i = 0; i < NUM_CORES; i++) {
        waitpid(pids[i], NULL, 0);
    }

    int expected = NUM_CORES * NUM_ITERATIONS;
    if (map->counter == expected) {
        printf("Test passed!! - expected %d and got %d\n", expected, map->counter);
    }
    else {
        printf("Test failed :( expected %d and got %d\n", expected, map->counter);
    }

    return 0;
}
