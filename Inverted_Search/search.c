#include "inverted_search.h"

void search(Wlist **head, char *word)
{
	// check empty or not

	if (*head == NULL)
	{
		printf("INFO :: Search Operation is not possible, List is empty \n");
		return; // 1;
	}

	// traverse the list

	while (*head != NULL)
	{
		// compare the search word with each node words available
		if (!strcmp((*head)->word, word))
		{
			printf("Word %s is present in %d file/s\n", word, (*head)->file_count);
			Ltable *Thead = (*head)->Tlink;

			printf("%s file_name\n",Thead->file_name);
			while (Thead != NULL)
			{
				printf("In file : %s  %d  time/s \n", Thead->file_name, Thead->word_count);
				Thead = Thead->table_link;
			}
			printf("\n");
			return; // 0;
		}
		{
			(*head) = (*head)->link;
		}
	}
	printf("INFO :: Search word not found in list\n");
}


/* Hash function */
int hash_function(const char *word)
{
	int index = tolower(word[0]) % 97;
	if(!(index >= 0 && index <= 25))
		index = 26;
	
	return index;
}
