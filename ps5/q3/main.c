#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#include "tests.h"

void handler(int s) {
    fprintf(stderr, "\nSignal #%d recieved\n", s); 
    exit(s);
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        fprintf(stderr, "Please enter a test to take!\n");
        return -1;
    }
    
    // create a 1 char buf to store test number
    int buf = atoi(argv[1]);
    if(buf == 0 || buf > 4) {
        fprintf(stderr, "Please enter in a valid test number! (1-4)\n"); 
        return -1;
    }
    

    // trap/handle all possible signals
    struct sigaction sa;
    sa.sa_handler = handler; 
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    for(int i = 1; i < 31; i++) {
        if (i == SIGKILL || i == SIGSTOP) continue; // cant set handlers for these guys
        if(sigaction(i, &sa, NULL) < 0) {
            fprintf(stderr, "Error creating sigaction for signal #%d\n", i);
            return errno;
        }
    }

    if(buf == 1) return test1();
    else if(buf == 2) return test2();
    else if(buf == 3) return test3();
    else if(buf == 4) return test4();
    else {
        fprintf(stderr, "Invalid test number! Please re enter a digit from (1-4)"); 
        return -1; 
    }

    return 0;
}
