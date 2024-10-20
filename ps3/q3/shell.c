#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "headers/commands.h"

#define MAX_INPUT_SIZE 4096

int read_input() {
    
    char buf[MAX_INPUT_SIZE];
    char* ret = NULL;

    while((ret = fgets(buf, MAX_INPUT_SIZE, stdin))) {

        // remove trailing whitespace by replacing it with null terminator
        int c_len = strlen(ret);
        if (c_len > 0) {
            int i = c_len - 1;
            while (i >= 0 && isspace(ret[i])) {
                ret[i] = '\0';
                i--;
            }
        }


        // use a queue to store input tokens
        Queue* queue = create_queue();
        char command[4096]; 

        Queue* args_queue = create_queue();
        Queue* io_queue = create_queue();

        // skip if comment
        if(buf[0] == '#') continue; 
        
        // tokenize sentence
        char* token = strtok(buf, " ");
        int cnt = 0;
        while(token) {
            /*enqueue(queue, token);*/
            
            if(cnt == 0) strcpy(command, token);
            else if(strchr(token, '>') || strchr(token, '<')) enqueue(io_queue, token);
            else enqueue(args_queue, token);

            cnt++;
            token = strtok(NULL, " ");
        }

        /* BUILT IN COMMANDS (no I/O redirection) */

        // check for cd
        if(strcmp(command, "cd") == 0) {
            if(get_size(args_queue) > 1) {
                fprintf(stderr, "%s: too many arguments", command);
                continue; 
            }
            ret = cd(dequeue(args_queue));
        }
        
        // check for pwd
        else if(strcmp(command, "pwd") == 0) ret = pwd(); // doesn't take any args

        else if(strcmp(command, "exit") == 0) {
            // exit using last ret value
        }

        // otherwise we have a "general" command which can do i/o redirection
        // needs all the forking and execing and all that
        else {
            ret = general_command(command, args_queue, io_queue); 
        }

    }

    printf("ret is: %s and eof is clicked!\n", ret);

    return 0;
}

int main() {

    read_input();
    return 0;
}
