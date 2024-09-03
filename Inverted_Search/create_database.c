#include "inverted_search.h"

/* Global variable to store filename */
char *fname;

/* Create database */
void create_database(Flist **f_head, Wlist *head[])
{
	// traverse through the file linked list
	while(*f_head != NULL)
	{
		read_datafile(*f_head, head, (*f_head)->file_name);
		(*f_head) = (*f_head)->link;
	}
}

/*  Read the contents of the file */
Wlist *read_datafile(Flist *file, Wlist *head[], data_t *filename)
{
	fname = filename;
	// open file in read mode
	FILE *fptr = fopen(filename, "r");

	// declare an array to store words
	char word[WORD_SIZE];
	int index, flag;

	while (fscanf(fptr, "%s", word) != EOF)
	{
		flag = 1;

		index = hash_function(word);

		/* Check if head[index] is not empty */
		if (head[index] != NULL)
		{
			// check words are repeated or not
			Wlist *temp = head[index];

			// compare the words at each node with new word
			while (temp)
			{
				if (!strcmp(temp->word, word))
				{
					update_word_count(&temp);
					flag = 0;
					break;
				}
				temp = temp->link;
			}
			if (flag == 1)
			{
				// if words are not repeated
				insert_at_last(&head[index], word, fname);
			}
		}
		/* head[index] is empty */
		else
		{
			insert_at_last(&head[index], word, fname);
		}
	}
}

/* Update_word_count 
	=> If files are same, Increment word_count
	=> If files are not same, Increment file_count
*/
int update_word_count(Wlist **head)
{
	Ltable *temp = (*head)->Tlink, *prev = NULL;

	while(temp != NULL)
	{
		if (!strcmp(temp->file_name, fname))
		{
			/* Incrementing word_count for same files */
			(temp->word_count)++;
			return SUCCESS;
		}
		else
		{
			prev = temp;
			temp = temp->table_link;
		}
	}

	/* Incrementing file count for different files */
	((*head)->file_count)++;

	Ltable *new = (Ltable *)malloc(sizeof(Ltable));

	if (new == NULL)
	{
		return FAILURE;
	}

	strcpy(new->file_name, fname);
	new->word_count = 1;
	new->table_link = NULL;

	prev->table_link = new;
	return SUCCESS;
}
