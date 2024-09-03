#include "main.h"

extern int exit_status, stopped_process;
extern struct sigaction act;
unsigned char bg_active;

/* insert_job, adds the stopped_process to linkedlist */
void insert_jobs(Plist **head, Plist **top, struct process *job, int num)
{
    Plist *new = (Plist *)malloc(sizeof(Plist));

    if (new == NULL)
    {
        /* New job not added/created */
        fprintf(stderr, "Error: adding jobs\n");
        return;
    }

    /* Updating new member fields */
    new->count = num;
    new->link = NULL;
    strcpy(new->status, job->status);
    strcpy(new->procname, job->process);
    new->pid = job->pid;

    /* Establishing link */
    if (*head == NULL)
    {
        *head = new;
        *top = new;
    }
    else
    {
        (*top)->link = new;
        (*top) = new;
    }
}

/* display_jobs display the list of stopped processes */
void display_jobs(Plist *head)
{
    Plist *temp = head;

    while (temp != NULL)
    {
        printf("[%d]   %s\t\t\t%s\n", temp->count, temp->status, temp->procname);
        temp = temp->link;
    }
}

/* fg command implementation, continues the last stopped jobs execution in foreground */
void fg(Plist **head, Plist **top)
{
    // Check if job present or not
    if (*head == NULL)
    {
        /* No process there to stop */
        fprintf(stderr, "fg : current: no such jobs\n");
    }
    else
    {
        tcsetpgrp(STDIN_FILENO, (*top)->pid);
        puts((*top)->procname);
        kill(-(*top)->pid, SIGCONT);
        

        int stat;
        //waitpid((*top)->pid, &stat, WUNTRACED);
        wait(&stat);

        tcsetpgrp(STDIN_FILENO, (*top)->pid);
        if (WIFEXITED(stat) || WIFSIGNALED(stat))
        {
            remove_job(head, top);
            stopped_process--;
            exit_status = WEXITSTATUS(stat);
        }
    }
}

/* bg command implementation, continues the last stopped job's execution in background */
void bg(Plist **head, Plist **top)
{
    if (*head == NULL)
    {
        /* No process there to stop */
        fprintf(stderr, "bg : current: no such jobs\n");
    }
    else
    {
        bg_active = 1;
        // Update status and name of the job
        strcpy((*top)->status, "Running");
        strcat((*top)->procname, " &");
        kill(-(*top)->pid, SIGCONT);
        if (sigaction(SIGCHLD, &act, NULL) == -1)
        {
            perror("sigaction");
            exit_status = EXIT_FAILURE;
        }
    }
}

/* Remove job after the completion of execution from list of jobs */
void remove_job(Plist **head, Plist **top)
{
    Plist *temp = *head;

    if (*head == *top)
    {
        free(*head);
        *top = *head = NULL;
    }
    else
    {
        while (temp->link != *top)
            temp = temp->link;

        temp->link = NULL;
        free(*top);
        *top = temp;
    }
}