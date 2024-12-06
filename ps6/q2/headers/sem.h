#ifndef _SEM_H
#define _SEM_H

#include "spinlock.h"
#include <sys/types.h>

#define NUM_PROCESSES 64

typedef struct {
    int count;
    spinlock_t lock;
    pid_t sleep_pids[NUM_PROCESSES];
    int sleep_count[NUM_PROCESSES];
    int woken_count[NUM_PROCESSES];
    int handled_count[NUM_PROCESSES];
} sem_t;

void sem_init(sem_t *sem, int count); 
int sem_try(sem_t *sem);
void sem_wait(sem_t *sem, int proc_num);
void sem_inc(sem_t *sem); 

#endif //_SEM_H
