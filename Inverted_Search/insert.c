#include "inverted_search.h"

int insert_at_last(Wlist **head, data_t *data, data_t *filename)
{
    // create a node

    Wlist *new = malloc(sizeof(Wlist));

    if (new == NULL)
    {
        return FAILURE;
    }

    // update
    new->file_count = 1;
    strcpy(new->word, data);
    new->Tlink = NULL;
    new->link = NULL;

    // call fun to update link table

    update_link_table(&new, filename);

    // checl Wlist is empty or not
    if (*head == NULL)
    {
        *head = new;
        return SUCCESS;
    }

    // not empty
    Wlist *temp = *head;
    // traverse
    while (temp->link)
    {
        temp = temp->link;
    }

    temp->link = new;
    return SUCCESS;
}

int update_link_table(Wlist **head, data_t *data)
{
    // create
    Ltable *new = malloc(sizeof(Ltable));

    if (new == NULL)
    {
        return FAILURE;
    }

    new->word_count = 1;

    strcpy(new->file_name, data);
    new->table_link = NULL;

    (*head)->Tlink = new;
    return SUCCESS;
}
