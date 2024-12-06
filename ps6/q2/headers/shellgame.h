#ifndef _SHELLGAME_H
#define _SHELLGAME_H

#define NUM_VCPUS 6

// shared memory region struct
typedef struct {
    sem_t A, B, C;
} shared_t;

#endif
