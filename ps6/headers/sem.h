#ifndef _SEM_H
#define _SEM_H

#include "spinlock.h"

typedef struct {
    int count;
    spinlock_t lock;
    int waiting_pids[10];   // pids of waiting processes
    int wait_count;         // num of processes waiting
} sem_t;

void sem_init(sem_t *sem, int count); 
int sem_try(sem_t *sem);
void sem_wait(sem_t *sem);
void sem_inc(sem_t *sem); 

#endif //_SEM_H
