#ifndef INVERTED_SEARCH_H
#define INVERTED_SEARCH_H

#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

//defining macros
#define FAILURE   -1
#define SUCCESS    0
#define FNAME_SIZE 15
#define WORD_SIZE 10
#define FILE_EMPTY -2
#define FILE_NOTAVAILABLE -3
#define REPEATATION -4

//Structure for file list
typedef char data_t;
typedef struct file_node
{
	data_t file_name[FNAME_SIZE];
	struct file_node *link;

}Flist;

//Structure for link table
typedef struct linkTable_node
{
	int word_count;
	data_t file_name[FNAME_SIZE];
	struct linkTable_node *table_link;
}Ltable;

//structure to store word count
typedef struct word_node
{
	int file_count;
	data_t word[WORD_SIZE];
	Ltable *Tlink;
	struct word_node *link;

}Wlist;

/* Create list of files */
int to_create_list_of_files(Flist **f_head, data_t *name);

/* Create Database */
void create_database(Flist **f_head, Wlist *head[]);

/* Reads contents of a file */
Wlist * read_datafile(Flist *file, Wlist *head[], data_t *filename);

/* Insert at last */
int insert_at_last(Wlist **head, data_t *data, data_t *filename);

/* Update link table */
int update_link_table(Wlist **head, data_t *file_node);

/* Update word count */
int update_word_count(Wlist **head);

/* Print word count */
int print_word_count(Wlist *head);

/* Search Operation in Database */
void search( Wlist **head, data_t *word);
 
/* Display Database */
void display_database( Wlist *head[] );

/* Save Database */
void save_database( Wlist *head[]);

/* Write Database into file */
void write_databasefile(Wlist *head, FILE* databasefile);

/* Update Database */
void update_database( Wlist *head[], Flist **f_head);

/* Check if File is Empty */
int isFileEmpty(data_t *filename);

/* File Validation and File_list */
void file_validation_n_file_list(Flist **f_head, data_t *argv[]);

/* Hash function for words */
int hash_function(const char *word);

#endif
