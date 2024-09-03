#include "main.h"

/* List of internals_cmds */
char *internal_cmds[] = {"cd", "pwd", "fg","bg", "jobs", NULL};

/* Check command type, checks the type of command and executes accordingly */
int check_command_type(char *input)
{
    int i = 0;

    for(i = 0; input[i] != '\0'; i++)
    {
        if(input[i] == '|')
        {
            return PIPE;
        }
    }

    char cmd[strlen(input) + 1];
    for(i = 0; input[i] != '\0' && input[i] != ' '; i++)
    {
        cmd[i] = input[i];
    }
    cmd[i] = '\0';

    for(i = 0; internal_cmds[i] != NULL; i++)
    {
        if(strcmp(cmd, internal_cmds[i]) == 0)
        {
            return BUILTIN;
        }
    }

    return EXTERNAL;
}