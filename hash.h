#include <stdio.h>
#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif
#include <string.h>
#ifndef LINKED_LIST
#define LINKED_LIST
#include "linkedList.h"
#endif
#define HASHTABLE_MAX_IDX 28

typedef struct 
{
    linkedList *buckets[HASHTABLE_MAX_IDX];
}
hashtable;

hashtable* createEmptyHashTable()
{
    hashtable *new = malloc(sizeof(hashtable));
    if (new == NULL)
        return NULL;
    int error_check = 0;
    int i;
    for (i = 0; i < HASHTABLE_MAX_IDX; i++)
    {
        new->buckets[i] = createEmptyList();
        if (new->buckets[i] == NULL)
        {
            error_check++;
            break;
        }
    }
    //cleanup in case of failure
    if (error_check > 0)
    {
        while (i >= 0)
        {
            free(new->buckets[i]);
            i--;
        }
        return NULL;
    }
    return new;
}

void deleteHashTable(hashtable **h)
{
    for (int i = 0; i < HASHTABLE_MAX_IDX; i++)
    {
        deleteList((*h)->buckets[i]);
        free((*h)->buckets[i]);
        (*h)->buckets[i] = NULL;
    }
    //*h = NULL;
}

void print2dhashtable(hashtable h)
{
    for (int i = 0; i < HASHTABLE_MAX_IDX; i++)
    {
        if ( i == 26)
            printf("| 0-9 | -> ");
        else if (i == 27)
            printf("| @_$ | -> ");
        else
            printf("| %c | -> ", 'A' + i);
        node *trav = h.buckets[i]->head;   
        int j = 0; 
        while (trav != NULL)
        {
            if (j == 5)
            {
                printf("\n         ");
                j = -1;
            }
            printf("|%s| -> ", trav->ac_name);
            trav = trav->next;
            j++;
        }
        printf(" NULL\n\n\n");
    }
}


int hashFunction(char *str)   //very naive and rudimentary hash function
{
    if (*str >= 'a' && *str <= 'z')
        return *str - 'a';
    else if (*str >= 'A' && *str <= 'Z')
        return *str - 'A';
    else if (*str >= '0' && *str <= '9')
        return  26;
    else
        return 27;
}
