#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "headers/sem.h"

void sem_init(sem_t *s, int count) {
    s->count = count; // act as a mutex lock
    spin_unlock(&s->lock);
    memset(s->sleep_pids, 0, sizeof(s->sleep_pids));
    memset(s->sleep_count, 0, sizeof(s->sleep_count));
    memset(s->woken_count, 0, sizeof(s->woken_count));
    memset(s->handled_count, 0, sizeof(s->handled_count));
}

// attempt to perform P operation
int sem_try(sem_t *s) {

    // initially assume that it would block
    int success = 0;
    spin_lock(&s->lock);

    if(s->count > 0) {
        s->count--;
        success = 1;
    }

    spin_unlock(&s->lock);
    return success;
}

// perform P operation, blocking until successful
void sem_wait(sem_t *s, int procnum) {

    sigset_t newmask, oldmask;

    // block SIGUSR1 to avoid race condition while adding self to list
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    spin_lock(&s->lock);

    // unconditionally decrement counter 
    // as a negative value can be used to track how many are sleeping
    s->count--;

    // block caller if semphore is not available
    if(s->count < 0) {

        // add self to sleeping list
        s->sleep_pids[procnum] = getpid();
        s->sleep_count[procnum]++;        
        spin_unlock(&s->lock);

        // wait for SIGUSR1
        sigset_t sus_mask;
        sigsuspend(&sus_mask);

        // restore signal mask after waking up
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
        s->handled_count[procnum]++;
    }

    // semaphore is available
    else {
        spin_unlock(&s->lock);
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
        s->handled_count[procnum]++;
    }
}


void sem_inc(sem_t *s) {

    spin_lock(&s->lock);
    s->count++;

    // if there are sleeping processes to wake up
    if(s->count <= 0) {
        for(int i = 0; i < NUM_PROCESSES; i++) {
            if(s->sleep_pids[i] != 0) {
                kill(s->sleep_pids[i], SIGUSR1);
                s->woken_count[i]++;
                s->sleep_pids[i] = 0;
            }
        }
    }

    spin_unlock(&s->lock);
}
