#include <stdio.h>
#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif
#include <string.h>

typedef struct nodet
{
    char *ac_name;
    char *ac_pas;
    struct nodet *next;
} node;

typedef struct
{
    node *head;
    node *rear;
    int l_size;
}
linkedList;

linkedList* createEmptyList(void);
void traverseList(linkedList l);
void insertAtBeg(linkedList *l, char *account, char *password);
void insertAtEnd(linkedList *l, char *val);
node* Search(linkedList *l, char *val);
int searchList(linkedList *l, char *val, node *arr[]);
char* deletefromBeg(linkedList *l);
char* deletefromEnd(linkedList *l);
void* deleteList(linkedList *l);

linkedList* createEmptyList(void)
{
    linkedList *l = (linkedList*) malloc(sizeof(linkedList));
    if (l ==  NULL)
        return NULL;
    l->head = l->rear = NULL;
    l->l_size = 0;
    return l;
}

void traverseList(linkedList l)
{
    if (l.head == NULL)
    {
        printf("Empty List\n");
        return;
    }
    while (l.head != NULL)
    {
        printf("%s\n", l.head->ac_name);
        l.head = l.head->next;
    }
    printf("\n");
}

void insertAtBeg(linkedList *l, char *account, char *password)
{
    node *temp = malloc(sizeof(node));
    if (temp == NULL)
    {
        printf("Out of memory\n");
        return;
    }
    temp->ac_name = malloc(sizeof(char) * (strlen(account) + 1));
    temp->ac_pas = malloc(sizeof(char) * (strlen(password) + 1));
    strcpy(temp->ac_name, account);
    strcpy(temp->ac_pas, password);
    temp->next = l->head;
    if (l->head == NULL)
        l->rear = temp;
    l->head = temp;
    l->l_size++;
}

int searchList(linkedList *l, char *val, node *arr[])  //arr len must be atleast equal to the length of linkedlist
{
    int i = 0; //for arr[] index count
    node *trav = l->head;
    while(trav != NULL)
    {
        if (strncasecmp(trav->ac_name, val, strlen(val)) == 0)
        {
            arr[i] = trav;
            i++;
        }
        trav = trav->next;
    }
    if (i == 0)
        return 0;  //not found
    return i;  //how many found
}

void deleteAtLoc(linkedList *l, node *loc)
{
    if (l->head == loc && l->rear == loc)  //if there is only one node in list
    {
        free(l->head->ac_name);
        free(l->head->ac_pas);
        free(l->head);
        l->head = l->rear = NULL;
    }
    else if (l->head == loc)   //if the node to be deleted is the first node
    {
        free(l->head->ac_name);
        free(l->head->ac_pas);
        node *next = l->head->next;
        free(l->head);
        l->head = next;
    }
    else if (l->rear == loc)  //if the node to be deleted is the last node
    {
        node *trav = l->head;
        while (trav->next->next != NULL)  //gets us to the second last node
            trav = trav->next;
        free(l->rear->ac_name);
        free(l->rear->ac_pas);
        free(l->rear);
        trav->next = NULL;
        l->rear = trav;
    }
    else   //for the inner node
    {
        node *trav = l->head;
        while (trav->next != loc)  //gets us to the node before the loc node
            trav = trav->next;
        free(loc->ac_name);
        free(loc->ac_pas);
        trav->next = loc->next;
        free(loc);
    }
    l->l_size--;
}

void* deleteList(linkedList *l)
{
    while(l->head != NULL)
    {
        node *next = l->head->next;
        free(l->head->ac_name);
        free(l->head->ac_pas);
        free(l->head);
        l->head = next; 
    }
    l->rear = NULL;
    l->l_size = 0;
    return NULL;
}