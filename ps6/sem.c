#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "headers/sem.h"

void sem_init(sem_t *s, int count) {
    s->count++;
    spin_unlock(&s->lock);
    s->wait_count = 0;
    memset(s->waiting_pids, 0, sizeof(s->waiting_pids));
}

// attempt to perform P operation
int sem_try(sem_t *s) {

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
void sem_wait(sem_t *s) {

    sigset_t newmask, oldmask;

    // block SIGUSR1 to avoid race condition while adding self to list
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    spin_lock(&s->lock);

    s->count--;
    if(s->count < 0) {

        // add self to waiting list
        pid_t pid = getpid();
        s->waiting_pids[s->wait_count++] = pid;

        spin_unlock(&s->lock);

        // wait for SIGUSR1 from sem_inc
        sigemptyset(&newmask);
        sigsuspend(&oldmask);

        // restore signal mask after waking up
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
    }
    else {
        spin_unlock(&s->lock);
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
    }
}


void sem_inc(sem_t *s) {

    spin_lock(&s->lock);

    s->count++;

    // wake up all waiting processes
    if(s->count <= 0 && s->wait_count > 0) {
        for(int i = 0; i < s->wait_count; i++) {

            kill(s->waiting_pids[i], SIGUSR1);
        }
        s->wait_count = 0;
    }

    spin_unlock(&s->lock);
}
