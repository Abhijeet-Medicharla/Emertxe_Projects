#include "main.h"

extern int exit_status;
struct sigaction act;
unsigned char sigtstp, sigchld;

/* sighandler, handles the signals SIGINT, SIGTSTP and SIGCHLD */
void sighandler(int signum, siginfo_t *info, void *dummy)
{
    switch(signum)
    {
        case SIGINT:            /* Handling SIGINT */
            printf("\n");
            break;
        case SIGTSTP:           /* Handling SIGTSTP, adding stopped process to list of jobs */
            printf("\n");
            sigtstp = 1;
            break;
        case SIGCHLD:           /* Handling SIGCHLD, calling remove_job after the child process has finished its execution*/
            sigchld = 1;
            printf("Sigchld received\n");
            break;
    } 
    exit_status = info->si_status;
}

/* Initialising sigaction system call for handling signals : SIGINT, SIGTSTP, SIGCHLD */
void init_sighandler(void)
{
    act.sa_sigaction = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        exit_status = EXIT_FAILURE;
    }
    if(sigaction(SIGTSTP, &act, NULL) == -1)
    {
        perror("sigaction");
        exit_status = EXIT_FAILURE;
    }
}