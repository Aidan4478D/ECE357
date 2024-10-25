#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "headers/commands.h"

#define MAX_INPUT_SIZE 4096

// trim trailing white spaces at the end of input
void trim_trailing_space(char* str) {
    int len = strlen(str);
    if (len > 0) {
        while (len > 0 && isspace(str[len - 1])) {
            str[--len] = '\0';
        }
    }
}

int read_input() {
    
    char buf[MAX_INPUT_SIZE];
    int ret = 0;

    while((fgets(buf, MAX_INPUT_SIZE, stdin))) {

        // remove trailing whitespace by replacing it with null terminator
        trim_trailing_space(buf); 

        // the queues are a little extra here but I made them and didn't want
        // them to go to waste
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

        // check for exit and if so exit with most recent return value
        else if(strcmp(command, "exit") == 0) {
        
            if(!is_empty(args_queue)) {
                int temp_ret = 0;

                if(get_size(args_queue) > 1) {
                    fprintf(stderr, "%s: Please enter only one argument. Exiting with %d\n", command, ret);
                }
                if((temp_ret = atoi(dequeue(args_queue))) == 0) {
                    fprintf(stderr, "%s: Invalid return code. Exiting with %d\n", command, ret);
                }
                else ret = temp_ret;
            }

            exit(ret);
        }

        // otherwise we have a "general" command which can do i/o redirection
        // needs all the forking and execing and all that
        else ret = general_command(command, args_queue, io_queue); 
    }

    fprintf(stderr, "ret is: %d and eof is clicked!\n", ret);

    return ret;
}

int main() {

    int ret = read_input();
    return ret;
}
