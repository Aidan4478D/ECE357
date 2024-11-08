#include <stdio.h>
#include <signal.h>

int main() {

    int i;
    for(i = i; i < 31; i++)
        signal(i, SIG_IGN);
    
    *(int *)0 = 1;
}
