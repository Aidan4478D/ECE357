#ifndef _SPINLOCK_H
#define _SPINLOCK_H

typedef struct {
    volatile char lock;
} spinlock_t;

void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);

#endif //_SPINLOCK_H
