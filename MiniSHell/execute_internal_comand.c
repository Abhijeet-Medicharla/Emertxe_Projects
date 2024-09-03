#include "main.h"

extern int exit_status, stopped_process;
extern Plist *head;
extern Plist *top;
pid_t pidr;

/* execute_internal_command, executes all the internal commands */
void execute_internal_commands(char *input)
{
    char **cmds = get_command(input, ' ');
    short int len = strlen(cmds[0]);

    char *buf = (char *) malloc(150);
    if(!strncmp(cmds[0], "cd", len))        /* cd */
    {
        if(chdir(input + (len + 1)) != 0)
        {
            fprintf(stderr, "cd : %s: No such file or directory\n", input + len + 1);
        }
    }
    else if(!strncmp(cmds[0], "pwd", len))  /* pwd */
    {
        if(getcwd(buf, 150 * sizeof(char)) != NULL)
        {
            printf("%s\n",buf);
        }
        else
        {
            perror("getwd");
        }
    }
    else if(!strncmp(cmds[0], "jobs", len)) /* jobs */
    {
        display_jobs(head);
    }
    else if(!strncmp(cmds[0], "fg", len))   /* fg */
    {
        pidr = top->pid;
        fg(&head, &top);
    }
    else if(!strncmp(cmds[0], "bg", len))   /* bg */
    {
        pidr = top->pid;
        bg(&head, &top);
    }
    free(buf);
}