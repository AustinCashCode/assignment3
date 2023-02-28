/*
    Austin J. Cash
    CS 344

    This file contains the bulk of the code for the 
    smallsh program, including the main event loop, 
    process creation, and I/O management.

    LAST EDIT: 2/27/2023

    TODO:   
            Fix signal handaling to properly display
            termination signal.
            Add foreground-only mode
*/



#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "pid_list.c"
#include "signal_handlers.c"

//These are defined as per the spec.
#define MAX_LINE 2048   //Longest possible input
#define MAX_ARGS 512    //Maximum number of arguments
#define no_op           //A macro that doesn't insert any code.



//Returns the total number of arguments in a given ragged array
int count_args(char ** args)
{
    int total_args = 0;
    for(int i = 0; args[i]; i++) {
        ++total_args;
    }
    return total_args;
}



//Alters the passed-in string to expand any instance of '$$' to the PID.
void variable_expansion(char * str) 
{
    char pid_str[8]; //Linux PID's only go up to 2^22, which has 7 digits.
    pid_t pid = getpid();
    sprintf(pid_str, "%d", pid); //Convert PID to a str so we can insert it later
    char * pid_in_str = strstr(str, "$$"); //Find the first '$$' in the string
    size_t pid_len = strlen(pid_str);
    size_t token_len = strlen("$$");

    //This loop finds the portion of the string to the left of '$$'
    //and to the right of '$$', then copies the data into a new string
    //that is appropriately sized in the correct order.
    while (pid_in_str != NULL) 
    {
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



//Checks the args vector for '<' or '>' and redirects
//stdin and stdout appropriately. This function will also
//remove the found tokens, since they should not be passed in
//to the command.
void io_redirect(char ** args)
{
    int argc = count_args(args);
    for(int i = 0; i < argc; ++i) {
        if(*args[i] == '<') {
            int FD1 = open(args[i + 1], O_RDONLY);

            //Need to check for errors. We can exit here
            //because this function is only called in the child.
            if(FD1 == -1) {
                perror("ERROR: Could not open file.");
                exit(1);    
            }

            dup2(FD1, 0);
            free(args[i]);
            free(args[i + 1]);

            for(int j = i; j < argc - 2; ++j) {
                args[j] = args[j + 2];
            }

            args[argc - 1] = NULL;
            args[argc - 2] = NULL;
            i--; //Since we moved the array back, we need to re-check this index
            argc = argc - 2; //We also removed two arguments, so argc needs correction
        }
        if(*args[i] == '>') {
            int FD2 = open(args[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0777);

            if(FD2 == -1) {
                perror("ERROR: Could not open file.");
                exit(1);    
            }

            dup2(FD2, 1);
            free(args[i]);
            free(args[i + 1]);
            for(int j = i; j < argc - 2; ++j) {
                args[j] = args[j + 2];
            }
            args[argc - 1] = NULL;
            args[argc - 2] = NULL;
            i--;
            argc = argc - 2;
        }
    }
}



//Takes a ragged array representing the command and it's arguments
//Creates a process which executes the command in args[0]
//Terminates the child once finished (unless it is a background process)
//Returns the exit status of the child
int command_execution(char ** args, list_of_children ** children, struct sigaction SIGINT_action, char foreground_flag)
{
    int erval;
    int exit_val = 0;
    pid_t child_pid = -5;
    int argc = count_args(args);
    int background_flag = 0;

    if(*args[argc - 1] == '&') {
        background_flag = 1;
        args[argc-1] = NULL;
        --argc;
    }


    child_pid = fork();
    switch(child_pid) {
        case -1:
            //error
            perror("fork() failed\n");
            exit(1);
            break;

        case 0:
            //child
            SIGINT_action.sa_handler = SIG_DFL;
            io_redirect(args);
            execvp(args[0], args);
            perror("File not found");
            exit(1);
            break;

        default:
            //parent

            if(background_flag == 1 && foreground_flag == '0') {
                //background processes
                *children = push(child_pid, *children);
                printf("Background PID is %d\n", child_pid);
            } 
            else {
                //foreground processes
                child_pid = waitpid(child_pid, &erval, 0);
                if(WIFSIGNALED(erval)) {
                    exit_val = WTERMSIG(erval); //We need to hold on to the previous exit value for the status command
                    printf("Child %d encountered an error: %d\n", child_pid, exit_val);
                }
                else {
                    exit_val = WEXITSTATUS(erval);
                    if(exit_val) {
                        printf("terminated by signal %d\n", exit_val);
                    }
                }
            }
            break;
    }

    return exit_val;
}



int main(void)
{
    char input[MAX_LINE];
    char foreground_only_status;
    char * token;
    char * args[MAX_ARGS] = {NULL};
    int my_argc = 0;
    int child_status;
    int nav = 0;
    int wait_status = 0;
    list_of_children * children = NULL;

    struct sigaction SIGINT_action = {0};
    struct sigaction SIGTSTP_action = {0};


    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);



    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGINT_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    FILE* fd = fopen(".foreground_setting", "r+");
    if(fd == NULL) {
        fd = fopen(".foreground_setting", "w");
        fputc('0', fd);
    }


    while(nav == 0)
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

        if(!args[0] || args[0][0] == '#') {
            //This is supposed to guard the rest of the event loop.
            //I was using continue, but continue is incorrect b/c
            //we still need to delete args[] and check child processes,
            //And I didn't want to duplicate them needlessly.
            no_op;
        }
        else if(strcmp(args[0], "exit") == 0) {
            kill(1, SIGKILL);
            nav = 1;
        }
        else if(strcmp(args[0], "cd") == 0) {
            if(args[1]) {
                chdir(args[1]);
            }
            else {
                char * home_dir = getenv("HOME");
                chdir(home_dir);
            }
        }
        else if(strcmp(args[0], "status") == 0) {
            printf("EXIT STATUS: %d\n", wait_status);
        }
        else {
            foreground_only_status = '0';
            wait_status = command_execution(args, &children, SIGINT_action, foreground_only_status);
        }

        //We need to remove all the old args, or else they could
        //interfere with future commands.
        my_argc = count_args(args);
        for(int i = 0; i < my_argc; i++) { 
            free(args[i]); 
            args[i] = NULL;
        }

        check_background_processes(&children);
    }
    fclose(fd);
    return EXIT_SUCCESS;
}