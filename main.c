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



//Alters the passed-in string to expand any instance of '$$' to the PID.
void variable_expansion(char * str) {
    char pid_str[8]; //Linux PID's only go up to 2^22, which has 7 digits.
    pid_t pid = getpid();
    sprintf(pid_str, "%d", pid); //Convert PID to a str so we can insert it later
    char * pid_in_str = strstr(str, "$$"); //Find the first '$$' in the string
    size_t pid_len = strlen(pid_str);
    size_t token_len = strlen("$$");

    //This loop finds the portion of the string to the left of '$$'
    //and to the right of '$$', then copies the data into a new string
    //that is appropriately sized in the correct order.
    while (pid_in_str != NULL) {
        size_t prefix_len = pid_in_str - str;
        size_t suffix_len = strlen(pid_in_str + token_len);
        size_t new_str_len = prefix_len + pid_len + suffix_len + 1;
        char * new_str = malloc(new_str_len);
        memcpy(new_str, str, prefix_len);
        memcpy(new_str + prefix_len, pid_str, pid_len);
        memcpy(new_str + prefix_len + pid_len, pid_in_str + token_len, suffix_len + 1);
        strcpy(str, new_str);
        free(new_str);
        pid_in_str = strstr(str, "$$"); // find next occurrence of "$$"
    }
}



int main(void)
{
    char input[MAX_LINE];
    char * args[MAX_ARGS] = {NULL};
    char * token;
    int nav = 1;

    while(nav == 1)
    {
        printf(":");                        //Prompt user for input
        fgets(input, MAX_LINE, stdin);      //Get user input
        input[strcspn(input, "\n")] = '\0'; //Replace the newline with null
        token = strtok(input, " ");         //Get the command. 


        for(int i = 0; token; i++) {
            args[i] = strdup(token);     //We need to strdup() this b/c args[] is an array of pointers
            variable_expansion(args[i]); //Replace '$$' with the PID
            token = strtok(NULL, " ");   //Get the next token
        }

        //Since the first token is always the command, we check for the 
        //pre-programmed commands first and execute them.

        if(!args[0] || strcmp(args[0], "#") == 0) {         
            continue;
        }
        else if(strcmp(args[0], "exit") == 0) {
            kill(1, SIGKILL);
            nav = 0;
        }
        else if(strcmp(args[0], "cd") == 0) {
            if(args[1]) {
                chdir(args[1]);
            }
            else {
                chdir(char * home_dir = getenv(HOME));
            }
        }
        else if(strcmp(args[0], "ls") == 0) {
        }
        else {
            //Execute a process
        }
        //We need to remove all the old args, or else they could
        //interfere with future commands.
        for(i = 0; i < MAX_ARGS; i++) { 
            free(args[i]); 
            args[i] = NULL;
        }
    }

    return EXIT_SUCCESS;
}


void SIGINT_handler() {
    char* err_message = "SIGINT detected\n";
    write(STDOUT_FILENO, err_message, 16);
    sleep(10);
}