#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <sys/mman.h>
#include <sys/wait.h>

/*#include "headers/tests.h"*/
#include "headers/sem.h"
#include "headers/shellgame.h"

void h(int signo) {
    return;
}

// going to write shell game ehre
int main(int argc, char* argv[]) {

    /*return spinlock_test();*/

    if(argc != 3) {
        fprintf(stderr, "Please enter in exactly two arguments! Ex. ./shellgame [initial sem value] [num moves]\n");
        return -1;
    }

    int init_value = atoi(argv[1]);
    int moves = atoi(argv[2]);

    if(init_value <= 0 || moves <= 0) {
        fprintf(stderr, "Please enter only positive interger inputs!\n");
        return -1;
    }

    // set up handler for SIGUSR1
    struct sigaction sa;
    sa.sa_handler = h;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        fprintf(stderr, "Error setting SIGUSR1 handler: %s\n", strerror(errno));
        return -1;
    }

    // initialize shared memory region
    shared_t *map;
    if((map = mmap(NULL, sizeof(shared_t), PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error mmapping shared data region: %s\n", strerror(errno));
        return -1;
    }

    sem_init(&map->A, init_value);
    sem_init(&map->B, init_value);
    sem_init(&map->C, init_value);

    // Task 0: A->B
    // Task 1: A->C
    // Task 2: B->A
    // Task 3: B->C
    // Task 4: C->A
    // Task 5: C->B

    pid_t vcpus[NUM_VCPUS];
    fprintf(stderr, "Main process spawning children...\n");

    for(int i = 0; i < NUM_VCPUS; i++) {

        if((vcpus[i] = fork()) < 0) {
            fprintf(stderr, "Fork for VCPU %d failed: %s\n", i, strerror(errno));   
            return -1;
        }

        // in child
        if(vcpus[i] == 0) {

            sem_t *to, *from;
            switch (i) {
                case 0: from = &map->A; to = &map->B; break;
                case 1: from = &map->A; to = &map->C; break;
                case 2: from = &map->B; to = &map->A; break;
                case 3: from = &map->B; to = &map->C; break;
                case 4: from = &map->C; to = &map->A; break;
                case 5: from = &map->C; to = &map->B; break;
                default:
                    fprintf(stderr, "Invalid child index!\n");
                    return -1;
            }

            fprintf(stderr, "VCPU %d starting, pid %d\n", i, getpid());

            // perform num moves
            for (int m = 0; m < moves; m++) {
                sem_wait(from, i);
                sem_inc(to);
            }
            
            fprintf(stderr, "Child %d (pid %d) done, signal handler was invoked %d times\n", 
                    i, getpid(), map->A.handled_count[i] + map->B.handled_count[i] + map->C.handled_count[i]);
            fprintf(stderr, "VCPU %d done\n", i);

            return 0;
        }
    }

    // wait for each child to finish in parent
    for (int i = 0; i < NUM_VCPUS; i++) {
        int status;
        pid_t c;
        if((c = wait(&status)) < 0) {
            fprintf(stderr, "Error waiting for child: %s\n", strerror(errno));
            return -1;
        }
        fprintf(stderr, "Child pid %d exited w/ %d\n", c, WEXITSTATUS(status));
    }

    // final print, tried to replicate it as close to urs as possible
    for (int i = 0; i < 3; i++) {

        // map int to sem A, B, C
        sem_t *sem;
        char sem_name;

        switch (i) {
            case 0: sem = &map->A; sem_name = 'A'; break;
            case 1: sem = &map->B; sem_name = 'B'; break;
            case 2: sem = &map->C; sem_name = 'C'; break;
            default: continue;
        }

        fprintf(stderr, "\nsem %c - val %d\n", sem_name, sem->count);
        for (int j = 0; j < NUM_VCPUS; j++) {
            fprintf(stderr, "  VCPU %d: sleeps %d wakes %d\n", j, sem->sleep_count[j], sem->woken_count[j]);
        }
    }

    return 0;

}
