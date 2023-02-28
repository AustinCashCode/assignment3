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
    char to_set;
    FILE* fd = fopen(".foreground_setting", "r+");
    char foreground_status = fgetc(fd);

    if(foreground_status == '0') {
        write(STDOUT_FILENO, enter_message, 49);
        to_set = '1';
    }
    else {
        write(STDOUT_FILENO, exit_message, 29);
        to_set = '0';
    }
    fseek(fd, 0, SEEK_SET);
    fputc(to_set, fd);
    fclose(fd);
}