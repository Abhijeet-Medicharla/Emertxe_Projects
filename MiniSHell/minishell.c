#define _GNU_SOURCE
#include "main.h"

/* Global declarations */
struct process proc;
Plist *top = NULL, *head = NULL;

char input_string[100], cmd[100];
int exit_status, stopped_process, cmd_stat = 1;

extern pid_t cpid;
extern unsigned char sigtstp, sigchld, bg_active;

/* Function to implement special variables, ?, $, SHELL */
int special_variable(char *input)
{
    if (!(strncmp(input, "echo ", 5)))
    {
        if (!strcmp(input + 5, "$?"))
        {
            printf("%d\n", exit_status);
        }
        else if (!strcmp(input + 5, "$$"))
        {
            printf("%d\n", getpid());
        }
        else if (strcmp(input + 5, "$SHELL") == 0)
        {
            printf("%s\n", getenv("SHELL"));
        }
        else
        {
            printf("%s", input + 5);
        }
        return SUCCESS;
    }
    return FAILURE;
}

int main()
{
    char prompt[50] = "minishell$", buf[20], c;
    int ret;

    init_sighandler();

    while (1)
    {
        printf("%s", prompt); // Displaying the promptz

        if (fgets(input_string, 100, stdin) == NULL)
        {
            // printf("input = %s\n", input_string);
            if (input_string == NULL)
            {
                continue;
            }
            // break;
        }

        input_string[strlen(input_string) - 1] = '\0';
        // Duplicating input string into cmd
        strcpy(cmd, input_string);

        /* Handling input command */
        if (!(strcmp(input_string, "exit"))) /* exit */
        {
            exit(0);
        }
        else if (!strncmp(input_string, "PS1=", 4)) /* PS1= */
        {
            strcpy(prompt, input_string + 4);
        }
        else if (special_variable(input_string) == SUCCESS) /* Special varible */
        {
            continue;
        }
        else if ((ret = check_command_type(input_string)) == EXTERNAL) /* External commands */
        {
            execute_external_commands(input_string);
        }
        else if (ret == BUILTIN) /* Builtin commands */
        {
            execute_internal_commands(input_string);
        }
        else if (ret == PIPE) /* N pipes */
        {
            execute_pipe_commands(input_string);
        }

        if (sigtstp)
        {
            if (strlen(cmd) > 1 && cmd_stat != 0)
            {
                strcpy(proc.status, "Stopped");
                strcpy(proc.process, cmd);
                proc.pid = cpid;
                stopped_process++;
                insert_jobs(&head, &top, &proc, stopped_process);
                printf("[%d]   %s\t\t\t%s\n", stopped_process, proc.status, proc.process);
            }
            sigtstp = 0;
        }

        if (sigchld)
        {
            printf("Sigchld received\n");
            if (bg_active)
            {
                int child_status;
                printf("Waiting for child to exit\n");
                pid_t child_pid = waitpid(-1, &child_status, 0);
                if (WIFEXITED(child_status))
                {
                    exit_status = WEXITSTATUS(child_status);
                    remove_job(&head, &top);
                    if (stopped_process > 0)
                        --stopped_process;

                    bg_active = 0;
                }
            }
            sigchld = 0;
        }

        memset(input_string, 0, sizeof(input_string));
        memset(cmd, 0, sizeof(cmd));
        cmd_stat = 1;
    }
}