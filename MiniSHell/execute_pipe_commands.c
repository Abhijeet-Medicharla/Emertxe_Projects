#include "main.h"

extern int exit_status;

/* execute_pipe_commands, to execute input with '|' */
void execute_pipe_commands(char *input)
{
    // Count the number of pipes in input
    int pipes = 0;
    for (int i = 0; input[i]; i++)
    {
        if (input[i] == '|')
        {
            pipes++;
        }
    }

    // Convert input_string to array of commands
    char **cmds = get_command(input, ' ');

    int cmd[pipes + 1], pipepos = 0;
    cmd[pipepos++] = 0;

    // Replace '|' with NULL and take position of the next command
    for (int i = 0; cmds[i] != NULL; i++)
    {
        if (!strcmp(cmds[i], "|"))
        {
            cmd[pipepos++] = i + 1;
            cmds[i] = NULL;
        }
    }

    int pipefd[pipes][2];
    pid_t pid[pipes + 1];

    for (int i = 0; i < pipes + 1; i++)
    {
        if (i < pipes)
        {
            if (pipe(pipefd[i]) < 0)
            {
                perror("pipe");
                exit(exit_status = EXIT_FAILURE);
            }
        }

        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("fork");
            exit(exit_status = EXIT_FAILURE);
        }
        else if (pid[i] == 0) // Child process
        {
            // If not the first command, get input from the previous pipe
            if (i != 0)
            {
                if (dup2(pipefd[i - 1][0], 0) < 0)
                {
                    perror("dup2 input");
                    exit(exit_status = EXIT_FAILURE);
                }
            }

            // If not the last command, output to the next pipe
            if (i < pipes)
            {
                if (dup2(pipefd[i][1], 1) < 0)
                {
                    perror("dup2 output");
                    exit(exit_status = EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors in the child process
            for (int j = 0; j < pipes; j++)
            {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }

            execvp(cmds[cmd[i]], cmds + cmd[i]);
            perror("execvp");
            exit(exit_status = EXIT_FAILURE);
        }
    }

    // Parent process closes all pipe file descriptors
    for (int i = 0; i < pipes; i++)
    {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    int status;
    /* Wait for child processes to complete execution */
    for (int i = 0; i < pipes + 1; i++)
    {
        // wait till child completed is terminated.
        waitpid(pid[i], &status, WUNTRACED);

        // Check whether child terminated normally or abnormally
        if (WIFEXITED(status)) // Normal termination
        {
            printf("The child %d terminated normally with code %d\n", pid[i], (exit_status = WEXITSTATUS(status)));
        }
        else if (WIFSIGNALED(status))
        {
            exit_status = WTERMSIG(status); // Return the signal no. which caused abnormal termination
            printf("The child %d terminated abnormally by receiving the signal %d\n", pid[i], exit_status);
            exit_status += 128;
        }
        else if (WIFSTOPPED(status)) // True if the process stopped
        {
            exit_status = WSTOPSIG(status);
            printf("The child %d stopped by receiving the signal %d\n", pid[i], WSTOPSIG(status));
            exit_status += 128;
        }
    }

    /* Free dynamically allocated memory */
    for (int i = 0; cmds[i] != NULL; i++)
    {
        free(cmds[i]);
    }
    free(cmds);
}