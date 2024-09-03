#include "main.h"

extern int exit_status;
extern struct sigaction act;
extern int cmd_stat;
pid_t cpid = 0;

/* Execute external commands, executes all external commands by creating a child process */
void execute_external_commands(char *input)
{
    if(input[0] == '\0')
    {
        return;
    }
    char **cmds = get_command(input,' ');
    
    int pid = fork();
    if(pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0)   /* CHild process */
    {
        if(execvp(cmds[0], cmds) == -1)
        {
            printf("%s : Command not found\n", cmds[0]);
            cmd_stat = 0;
            exit_status = EXIT_FAILURE;
            exit(EXIT_FAILURE);
        }
    }
    else                /* Parent process */
    {
        cpid = pid;
        int status;
        init_sighandler();
        waitpid(pid, &status, WUNTRACED);
        
        // Check whether child terminated normally or abnormally
        if(WIFEXITED(status)) // Normal termination
        {
            printf("The child %d terminated normally with code %d\n", pid, (exit_status = WEXITSTATUS(status)));
        }
        else if(WIFSIGNALED(status))
        {
            exit_status = WTERMSIG(status); // Return the signal no. which caused abnormal termination
            printf("The child %d terminated abnormally by receiving the signal %d\n", pid, exit_status);
            exit_status += 128;
        }
        else if(WIFSTOPPED(status)) // True if the process stopped
        {
            exit_status = WSTOPSIG(status);
            printf("The child %d stopped by receiving the signal %d\n", pid, WSTOPSIG(status));
            exit_status += 128;
        }
    }

    /* Free dynamically allocated memory */
    for(int i = 0; cmds[i] != NULL; i++)
    {
        free(cmds[i]);
    }
    free(cmds);
}