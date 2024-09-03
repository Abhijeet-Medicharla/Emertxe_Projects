#include "main.h"

/* get_command, to convert input_string into array of strings(commands) */
char **get_command(char *input, char delim)
{
    char *token;
    char **tokens = NULL;
    int count = 0;

    token = strtok(input, &delim);
    while(token != NULL)
    {
        tokens = realloc(tokens, (count + 1)*sizeof(char *));

        tokens[count] = malloc(strlen(token) + 1);
        strcpy(tokens[count++], token);
        token = strtok(NULL, &delim);
    }
    tokens = realloc(tokens, (count + 1) * sizeof(char *));
    tokens[count] = NULL;

    return tokens;
}