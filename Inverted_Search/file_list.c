#include "inverted_search.h"

void file_validation_n_file_list(Flist **f_head, char *argv[])
{
	int i = 1, empty;
	while (argv[i] != NULL)
	{
		empty = isFileEmpty(argv[i]);
		if (empty == FILE_NOTAVAILABLE)
		{
			printf("INFO :: File : %s is not available\n", argv[i]);
			printf("STATUS :: Not adding the file into file linked list\n");
			i++;
			continue;
		}
		else if (empty == FILE_EMPTY)
		{
			printf("INFO :: File : %s is not having contents in it\n", argv[i]);
			printf("STATUS :: Not adding the file into file linked list\n");
			i++;
			continue;
		}
		else
		{
			int ret_val = to_create_list_of_files(f_head, argv[i]);
			if (ret_val == SUCCESS)
			{
				printf("SUCCESS :: Inserting the file: %s into file linked list\n", argv[i]);
			}
			else if (ret_val == REPEATATION)
			{
				printf("INFO :: The file: %s is repeated.\n", argv[i]);
				printf("STATUS :: Not adding the file into file linked list\n");
			}
			else
			{
				printf("FAILURE :: Inserting the file into file linked list\n");
			}
			i++;
		}
	}
}

// fun() is used to check file availability and file contents
int isFileEmpty(char *filename)
{
	FILE *fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		if (errno == ENOENT)
		{
			return FILE_NOTAVAILABLE;
		}
	}

	fseek(fptr, 0, SEEK_END);
	if (ftell(fptr) == 0)
	{
		return FILE_EMPTY;
	}
}

// fun() to create list of files
int to_create_list_of_files(Flist **f_head, char *name)
{
	if (*f_head == NULL)
	{
		/* Creating new node */
		Flist *new_file = (Flist *)malloc(sizeof(Flist));

		if (new_file == NULL)
		{
			return FAILURE;
		}

		/* Updating fields */
		strcpy(new_file->file_name, name);
		new_file->link = NULL;

		/* Establishing link */
		*f_head = new_file;
	}
	else
	{
		Flist *temp = *f_head, *prev = NULL;

		while (temp != NULL)
		{
			if (strcmp(temp->file_name, name) == 0)
			{
				return REPEATATION;
			}
			else
			{
				prev = temp;
				temp = temp->link;
			}
		}

		/* Create new node */
		Flist *new_file = (Flist *)malloc(sizeof(Flist));

		if (new_file == NULL)
		{
			return FAILURE;
		}

		strcpy(new_file->file_name, name);
		new_file->link = NULL;

		prev->link = new_file;
	}
	return SUCCESS;
}