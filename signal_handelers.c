/*
    Austin J. Cash
    CS 344

    This file contains the implementation
    of cusom signal handelers for both 
    SIGINT and SIGSTP.

    SIGINT: Terminates the foreground child process,
            assuming it exists

    SIGSTP: Toggles between foregound-only mode and
            background-permissable mode. Any process
            created in foreground only mode will only
            be ran in the foreground, regardless of the 
            usage of '&'.

    LAST EDIT: 2/27/2023
*/


/*
void handle_SIGSTP(int signo)
{
    char * enter_message = "Entering foreground-only mode (& is now ignored)\n";
    char * exit_message = "Exiting foreground-only mode\n";

}*/