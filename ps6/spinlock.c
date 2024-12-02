#include <sched.h>

#include "headers/spinlock.h"
#include "headers/tas.h"


// acquire the spinlock
void spin_lock(spinlock_t *spinlock) {

    while(tas(&spinlock->lock)) {
        // lock is locked
        sched_yield();
    }
}

// release spinlock
void spin_unlock(spinlock_t *spinlock) {

    spinlock->lock = 0;
}
