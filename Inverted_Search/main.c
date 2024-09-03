#include "inverted_search.h"

int main(int argc, char *argv[])
{
    system("clear");
    Wlist *head[27] = {NULL};
    
    /* Command Line Args Validation */
    if (argc <= 1)
    {
        printf("INFO :: file/s not passed through Command Line.\n");
        printf("USAGE :: ./proj.exe file1.txt file2.txt...\n");
        printf("STATUS :: Program Terminated\n");
        return 0;
    }

    // create list of filenames
    // declare head pointer

    Flist *f_head = NULL;

    /*  Validate the files */
    file_validation_n_file_list(&f_head, argv);

    if (f_head == NULL)
    {
        printf("INFO :: No files are available in the file linked list\n");
        printf("STATUS :: Program Terminated !!! \n");
        return 1;
    }

    int option;
    char choice;

    do
    {
        printf("Select your choice among following options:\n1. Create DATABASE\n2. Display Database\n3. Update DATABASE\n4. Search\n5. Save Database\n6. Exit\nEnter your choice: ");
        scanf("%d", &option);
        switch (option)
        {
        case 1: /* Create Database */
            create_database(&f_head, head);
            printf("SUCCESS :: Database Creation !!!\n");
            break;

        case 2: /* Display Database */
            display_database(head);
            break;

        case 3: /* Update Database */
            update_database(head, &f_head);
            break;

        case 4: /* Search Database */
            char word[WORD_SIZE];
            printf("Enter the word to search in database\n");
            scanf("%s", word);
            search(&head[hash_function(word)], word);
            break;

        case 5: /* Save Database */
            save_database(head);
            printf("SUCCESS :: Database Saved !!!\n");
            break;

        case 6: /* Exit Project */
            printf("INFO :: Exiting Project !!!\n");
            exit(1);
            break;

        default:
            puts("Please, Enter valid argument");
            break;
        }
        printf("Do you want to continue? (y/n): \n");
        __fpurge(stdin);
        scanf("%c", &choice);

    } while (choice != 'n');
}
