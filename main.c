#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//These are defined as per the spec.
#define MAX_LINE 2048   //Longest possible input
#define MAX_ARGS 512    //Maximum number of arguments


int main(void)
{
    char input[MAX_LINE];
    char * args[MAX_ARGS] = {NULL};
    char * token;
    pid_t * children;
    pid_t parent_pid = getpid();
    int nav = 1;



    while(nav == 1)
    {
        printf(":");                        //Prompt user for input
        fgets(input, MAX_LINE, stdin);      //Get user input
        input[strcspn(input, "\n")] = 0;    //Strip the trailing newline
        token = strtok(input, " ");         //Get the command. 

        for(int i = 0; token; i++) {
            args[i] = strdup(token);    //We need to strdup() this b/c args[] is an array of pointers
            token = strtok(NULL, " ");
        }


        //Since the first token is always the command, we check for the 
        //pre-programmed commands first and execute them.

        if(!args[0] || args[0] == "#") {         
            continue;
        }
        else if(strcmp(args[0], "exit") == 0) {
            kill(1, SIGKILL);
            nav = 0;
        }
        else if(strcmp(args[0], "cd") == 0) {

        }
        else if(strcmp(args[0], "ls") == 0) {
            
        }
        else {

        }




    }

    return EXIT_SUCCESS;
}


void SIGINT_handler() {
    char* err_message = "SIGINT detected\n";
    write(STDOUT_FILENO, err_message, 16);
    sleep(10);
}