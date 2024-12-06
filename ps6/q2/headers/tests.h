#ifndef _TESTS_H
#define _TESTS_H

#define NUM_CORES 12 // oh yeah this guy is a beast
#define NUM_ITERATIONS 3000000

#include "headers/spinlock.h"

typedef struct {
    int counter;
    spinlock_t lock;
} shared_t;

int spinlock_test();

#endif // _TESTS_H
