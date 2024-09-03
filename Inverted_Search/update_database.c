#include "inverted_search.h"

void update_database(Wlist *head[], Flist **f_head)
{
    // prompt the user

    char file_name[FNAME_SIZE];
    printf("Enter the file for updating the database\n");
    scanf("%s", file_name);

    /*  Validating file */
    int empty;
    empty = isFileEmpty(file_name);
    if (empty == FILE_NOTAVAILABLE)
    {
        printf("INFO :: File : %s is not available\n", file_name);
        printf("STATUS :: Not adding the file into file linked list\n");
    }
    else if (empty == FILE_EMPTY)
    {
        printf("INFO :: File : %s is not having contents in it\n", file_name);
        printf("STATUS :: Not adding the file into file linked list\n");
    }
    else
    {
        int ret_val = to_create_list_of_files(f_head, file_name);
        if (ret_val == SUCCESS)
        {
            printf("SUCCESS :: Inserting the file: %s into file linked list\n", file_name);
            printf("SUCCESS :: Database Updated !!!\n");
        }
        else if (ret_val == REPEATATION)
        {
            printf("INFO :: The file: %s is repeated.\n", file_name);
            printf("STATUS :: Not adding the file into file linked list\n");
        }
        else
        {
            printf("FAILURE :: Adding the file into file linked list\n");
        }
    }
    //

    // create the database
    while (*f_head != NULL)
    {
        if (!strcmp((*f_head)->file_name, file_name))
        {
            create_database(f_head, head);
        }
        else
        {
            *f_head = ((*f_head)->link);
        }
    }
}
