#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "queue.h"

#define MAX_INPUT_SIZE 4096

int read_input() {
    
    char buf[MAX_INPUT_SIZE];
    char* ret = NULL;

    while((ret = fgets(buf, MAX_INPUT_SIZE, stdin))) {
        
        // use a queue to store input tokens
        Queue* queue = create_queue();

        // skip if comment
        if(buf[0] == '#') continue; 
        
        // tokenize sentence
        char* token = strtok(buf, " ");
        while(token) {
            enqueue(queue, token);
            token = strtok(NULL, " ");
        }

        // get first word from input (command)
        char* command = dequeue(queue);
        printf("string is: %s\n", token);

    }

    printf("ret is: %s and eof is clicked!\n", ret);

    return 0;
}

int main() {

    read_input();
    return 0;
}
