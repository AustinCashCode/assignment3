/*
    Austin J. Cash
    CS 344

    This file contains the implementation
    of cusom signal handelers for both 
    SIGINT and SIGSTP.

    SIGINT(2): Terminates the foreground child process,
            assuming it exists. There is no custom handler for
            this interupt, but the documentation for it is here
            for the sake of readability.

    SIGSTP: Toggles between foregound-only mode and
            background-permissable mode. Any process
            created in foreground only mode will only
            be ran in the foreground, regardless of the 
            usage of '&'.

    All other signals use default behavior.

    LAST EDIT: 2/27/2023
*/



void handle_SIGTSTP(int signal)
{
    char * enter_message = "Entering foreground-only mode (& is now ignored)\n";
    char* exit_message = "Exiting foreground-only mode\n";
    int * foreground_flag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if(*foreground_flag == 0) {
        write(STDOUT_FILENO, enter_message, 49);
        *foreground_flag = 1;
    }
    else {
        write(STDOUT_FILENO, exit_message, 29);
        *foreground_flag = 0;
    }
    munmap(foreground_flag, sizeof(int));
}