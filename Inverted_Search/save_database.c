#include "inverted_search.h"

void save_database(Wlist *head[])
{
    // prompt the user for new file to save the database
    char file_name[FNAME_SIZE];

    printf("Enter the filename to save the database\n");
    scanf("%s", file_name);

    // open file in write mode

    FILE *fptr = fopen(file_name, "w");
    fprintf(fptr,"[index]    [word]    [file_count] file/s:          File      word_count\n");
    int i;
    for (i = 0; i < 27; i++)
    {
        if (head[i] != NULL)
        {
            write_databasefile(head[i], fptr);
        }
    }
}

/* Writing Database into a file */
void write_databasefile(Wlist *head, FILE *databasefile)
{
    while (head != NULL)
    {
        fprintf(databasefile, " [%2d]   [%-*s]     [%d]      file/s: ", hash_function(head->word), WORD_SIZE, head->word, head->file_count);
        Ltable *Thead = head->Tlink;

        // Traverse through Ltable
        while (Thead)
        {
            fprintf(databasefile, "File : %-*s %d,      ", FNAME_SIZE, Thead->file_name, Thead->word_count);
            Thead = Thead->table_link;
        }
        fprintf(databasefile, "\n");
        head = head->link;
    }
}
