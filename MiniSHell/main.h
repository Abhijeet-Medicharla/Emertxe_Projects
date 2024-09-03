#ifndef MAIN_H
#define MAIN_H

/* Header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 
/* MACROS */
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3
#define PIPE        4

#define SUCCESS         10
#define FAILURE         11
#define NOJOBS          12

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* Structure to store information of a process */
struct process
{
    char status[20];
    char process[100];
    pid_t pid;
};

/* Structure to create linkedlists for list of jobs */
typedef struct jobs
{
    unsigned short int count;
    pid_t pid;
    char status[20];
    char procname[100];
    struct jobs *link;
}Plist;


/* Function definitions related to implementation of minishell */
int check_command_type(char *command);
void echo(char *input_string, int status);
void execute_external_commands(char *input);
void execute_internal_commands(char *input);
void execute_pipe_commands(char *input);
char **get_command(char *input_string, char delim);

/* Function definitions related to job(stopped process) handling */
void insert_jobs(Plist **head, Plist **top, struct process *job, int num);
void display_jobs(Plist *head);
void fg(Plist **head, Plist **top);
void bg(Plist **head, Plist **top);
void remove_job(Plist **head, Plist **top);

/* Function definition related to signal handling */
void sighandler(int signum, siginfo_t *info, void *dummy);
void init_sighandler(void);


#endif