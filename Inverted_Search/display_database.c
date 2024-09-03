#include "inverted_search.h"

void display_database(Wlist *head[])
{
	printf("INFO :: Displaying Database \n");
	printf("[index]    [word]      file_count   file/s    File : File_name  word_count\n");

	for (int i = 0; i < 27; i++)
	{
		if (head[i] != NULL)
		{
			print_word_count(head[i]);
		}
	}
}
int print_word_count(Wlist *head)
{
	// traverse through Wlist
	while (head != NULL)
	{
		printf(" [%2d]    [%-*s]      [%d]      file/s:   ", hash_function(head->word), WORD_SIZE, head->word, head->file_count);
		Ltable *Thead = head->Tlink;

		// traverse through Ltable
		while (Thead)
		{
			printf("File : %-*s  %d, ", FNAME_SIZE, Thead->file_name, Thead->word_count);
			Thead = Thead->table_link;
		}
		printf("\n");
		head = head->link;
	}
}
