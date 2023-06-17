//header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifndef LINKED_LIST
#define LINKED_LIST
#include "linkedList.h"
#endif
#include "hash.h"
#include <time.h>
#include <conio.h>
#include <sys/types.h> //  --for opendir()
#include <dirent.h>    //  --for opendir()

//macros
#define default_pass "0000"  //default password of the password manager
#define MAX_ATTEMPTS 5  //maximum no. of wrong attempts accepted before the program quits
#define Program_Folder_Path "C:\\PassManager"
#define RECORDS_FILE "C:\\PassManager\\MyPasswords.bin"
#define LOGIN_PASSWORD_FILE "C:\\PassManager\\LOGIN_PASS.txt"
#define DATABASE_CPY_FILE "MyRecords.txt"
#define TEST_DATA_NAMES_FILE "C:\\PassManager\\testdata.txt"
#define TEST_DATA_PASS_FILE "C:\\PassManager\\testdata2.txt"
#define TEST_DATA_RECORDS 203

//globals
int ENCRYPTION_KEY;
char *LOGIN_PASS; 


//function declarations
void reset_program(void);
void printBanner(void);
void printMenu(void);
void gotoxy(int x,int y);
int searchRecords(void);
int editRecords(void);
int loadDatabaseWritingMode(void);
int loadDatabaseReadingMode(void);
int makeDatabaseCopy(void);
int print2DHashMap(void);
void generateRandomString(void);
int fillWithTestData(void);
void encrypt(char str[]);
void decrypt(char str[], int key);
char* getstr(void);
int changeLOGIN_PASS(void);
char* getstrMasked(void);

//typedefs
typedef struct
{
    char *account_name;
    char *account_password;
} 
record;


int main(void)
{
    //set crypto key
    //srand(time(NULL));
    ENCRYPTION_KEY = 1; //rand() % 25;
    //CRYPTO_KEY++;

    //make PassManager directory if it doesn't exists
    //checking if PassManager exist or not
    {
        char cmd[100];
        strcpy(cmd, "mkdir ");
        strcat(cmd, Program_Folder_Path);
        DIR *ptr = opendir(Program_Folder_Path);
        if (ptr == NULL)  //dir doesn't exist
        {
            system(cmd);
        }
        closedir(ptr);
    }


    /*this section checks the LOGIN_PASSWORD_FILE. If the file exists, it gets the password from there and 
     *puts it in the variable LOGIN_PASS. If the file doesn't exist, then we go along with our default_pass only                                  */
    //______________________________________________________________________
    FILE *pasw = fopen(LOGIN_PASSWORD_FILE, "r");
    if (pasw != NULL)
    {
        char buffer[100];
        fscanf(pasw, "LOGIN_PASS: %[^\n]s", buffer);
        fclose(pasw);
        decrypt(buffer, ENCRYPTION_KEY);
        //printf("buffer: %s\n", buffer);
        char *tmp = malloc(sizeof(char) * (strlen(buffer) + 1));
        if (tmp != NULL)
        {
            strcpy(tmp, buffer);
            LOGIN_PASS = tmp;
        }
    }
    else
    {
        LOGIN_PASS = malloc(sizeof(char) * (strlen(default_pass) + 1)); //I assigned value dynamically here beacause LOGIN_PASS is freed later in the code, so it removes that edge case
        strcpy(LOGIN_PASS, default_pass);
    }

    //PROGRAM ----- TITLE
    printBanner();

    char *user_input = NULL; 
    int attempt_count = 1;  //we count the no. of attempts user has made to guess the password
    printf("enter password to continue: ");
    do
    {
        if (attempt_count > MAX_ATTEMPTS)  //we exit after max attempts
        {
            printf("\nToo many incorrect attempts.\n");
            //char temp;
            printf("\npress |CTRL| + |R| to reset password, any other key to exit: ");
            //scanf(" %c", &temp);
            fseek(stdin, 0, SEEK_END);
            int inp = getch();
            if (inp == 18) {  // Ctrl + r gives ascii value 18
                printf("\n\nAre you sure? All data will be deleted(Press 'y' for yes): ");
                fseek(stdin, 0, SEEK_END);
                inp = getchar();
                if (inp == 'y' || inp == 'Y')
                    reset_program();
            }
            return 0;
        }        
        if (attempt_count > 1 && attempt_count != MAX_ATTEMPTS)
            printf("\nWrong password!(%d attempts left) Try again: ", MAX_ATTEMPTS - attempt_count + 1);

        if (attempt_count == MAX_ATTEMPTS)
            printf("\nWrong password!(Last attempt left) Try again: ");

        do
            user_input = getstrMasked();
        while (user_input == NULL);
        
        attempt_count++;
    }
    while (strcmp(user_input, LOGIN_PASS) != 0);
    free(user_input);

    //program loop
    int i = 0;
    for (;;)
    {
        //gotoxy(40, 13);
        system("cls");
        printBanner();
        if (i == 0)
        {
            printf("UNLOCKED!\n\n");
            i++;
        }
        int opt = 1234;  //this very large no. makes sure, there is no auto-selection in switch
        //for eg. suppose at the moment I entered an invalid character (like 'z') instead of an int. But last time I 
        //selected option 4, thus opt currently has 4 in it. Thus, the switch statement automatically goes to 
        //case 4 because opt has a previous value in it, thus I reinitialize opt every cycle of loop
        //fseek(stdin, 0, SEEK_END);
        printMenu();
        fseek(stdin, 0, SEEK_END);
        scanf("%d", &opt);
        //system("cls");
        //printBanner();
        
        switch (opt)
        {
            case 0: {   //exit program
                
                char c;
                printf("Are you sure you want to exit(Press 'y' for yes)? ");
                fseek(stdin, 0 , SEEK_END);
                scanf("%c", &c);
                if (c == 'y' || c == 'Y')
                {
                    free(LOGIN_PASS);
                    //printf("LOGIN_PASS: %p\n", LOGIN_PASS);
                    return 0;
                }
                break;
            }
            case 1: {   //searching records
                system("cls");
                printBanner();
                searchRecords();
                break;
            }
            case 2: {   //editing records
                system("cls");
                printBanner();
                editRecords();
                break;
            }
            case 4: {     //See records
                system("cls");
                printBanner();
                loadDatabaseReadingMode();
                break;
            }
            case 3: {  //Add new record
                system("cls");
                printBanner();
                loadDatabaseWritingMode();
                //printf("record added. press to co\n");
                //scanf("%d", (int *)NULL);
                break;
            }
            case 5: {  //generate random password
                system("cls");
                printBanner();
                generateRandomString();
                break;
            }
            case 6: {
                //change LOGIN_PASS
                char resp = 'n';
                printf("Are you sure you want to change the login password(Press 'y' for yes)? ");
                scanf(" %c", &resp);
                if (resp == 'y' || resp == 'Y')
                {  
                    system("cls"); 
                    printBanner();
                    changeLOGIN_PASS();
                }
                break;                
            }
            case 7: {
                //delete entire database
                int inp;
                printf("\nAre you sure? All records will be deleted(Press 'y' for yes): ");
                fseek(stdin, 0, SEEK_END);
                inp = getchar();
                if (inp == 'y' || inp == 'Y')
                {
                    printf("\nEnter the login password to delete database: ");
                    char *pass_confirm = getstrMasked();
                    if (strcmp(pass_confirm, LOGIN_PASS) != 0)
                    {
                        free(pass_confirm);
                        printf("\nIncorrect password!\n");
                        fseek(stdin, 0, SEEK_END);
                        printf("\nPress any key to continue: ");
                        getchar();
                        break;
                    }
                    free(pass_confirm);
                    FILE *fptr = fopen(RECORDS_FILE, "r");
                    int ch;
                    if (fptr == NULL)
                        printf("\nDatabase was already empty\n\n");
                    else if (fptr != NULL && (ch = fgetc(fptr)) == EOF)
                    {
                        fseek(fptr, -1, SEEK_CUR);
                        fclose(fptr);
                        printf("\nDatabase was already empty\n\n");
                    }
                    else if (fptr != NULL && (ch = fgetc(fptr)) != EOF)
                    {
                        fclose(fptr);
                        fptr = fopen(RECORDS_FILE, "w");
                        printf("\nDatabase deleted successfully!\n\n");
                    }
                    //else    
                    //    printf("\n\nNo database found! \n");
                    fseek(stdin, 0, SEEK_END);
                    printf("Press any key to continue: ");
                    getchar();
                }
                break;
            }
            case 8: {    //Make a copy file of records
                system("cls");
                printBanner();
                makeDatabaseCopy();
                break;
            }
            case 9: {
                system("cls");
                printBanner();
                print2DHashMap();
                break;
            }
            case 10: {
                //system("cls");
                //printBanner();
                fillWithTestData();
                break;
            }
            default: {
                system("cls");
                printBanner();
                printf("Invalid option\n");
                fseek(stdin, 0, SEEK_END);
                printf("press any key to continue: ");
                getchar();
                break;
            }
        }

    }
    
    return 0;
}

void reset_program(void)
{
    //we open the records file in write mode to delete any previous data
    FILE *fp = fopen(RECORDS_FILE, "w");
    if (fp == NULL)
    {
        printf("Couldn't reset program at the moment. Try again later.\n");
        fseek(stdin, 0, SEEK_END);
        getch();
        return;
    }
    fclose(fp);

    //we remove LOGIN_PASS file which essentially resets LOGIN_PASS to default_pass
    if (remove(LOGIN_PASSWORD_FILE) != 0)
    {
        printf("\nCouldn't reset password at the moment. It is possible that the password is already set to default. Try logging in again with the default password.\n");
        printf("-> ");
        fseek(stdin, 0, SEEK_END);
        getch();
        return;
    }
    printf("\nProgram has been successfully reset. Use '0000' to login next time\n");
    printf("\npress any key to exit-> ");
    fseek(stdin, 0, SEEK_END);
    getch();
}

void printBanner(void)
{
    printf("\n\n\n\n");
    printf("\t\t\t\t  ____               __  __\n");                                   
    printf("\t\t\t\t |  __ \\            |  \\/  |\n");                                  
    printf("\t\t\t\t | |__) __ _ ___ ___| \\  / | __ _ _ __   __ _  __  _  ___ _ __\n");
    printf("\t\t\t\t |  ___/ _` / __/ __| |\\/| |/ _` | '_ \\ / _` |/ _`  |/ _ | '__|\n");
    printf("\t\t\t\t | |  | (_| \\__ \\__ | |  | | (_| | | | | (_| | (_|  |  __| |\n");   
    printf("\t\t\t\t |_|   \\__,_|___|___|_|  |_|\\__,_|_| |_|\\__,_|\\__,  |\\___|_|\n"); 
    printf("\t\t\t\t                                               __/  |      Ver 0.00\n");
    printf("\t\t\t\t                                              |____/\n\n\n\n\n\n\n\n\n\n\n\n"); 
}

void printMenu(void)
{
    printf("Select option to continue:\n\n");
    printf("[1].Search records\n");
    printf("[2].Edit records\n");
    printf("[3].Add a new record\n");
    printf("[4].See entire database\n");
    printf("[5].Generate random password\n");
    printf("[6].Reset login password\n");
    printf("[7].Delete entire database\n");
    printf("[8].Make a backup copy of the database\n");
    printf("[9].Show 2-D HASH representation of records\n");
    printf("[10].Dump database with test data\n");
    printf("[0].Exit PassManager\n");
    printf("________________________________________________________\n");
    printf("-> ");
}


int searchRecords(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "rb");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    } 
    record newrec;
    int account_name_len, account_password_len;
    //make a hash table to work on records
    hashtable *records_hash_table = createEmptyHashTable();
    if (records_hash_table == NULL)
    {
        printf("Error in loading the records to memory.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;
    }
    int i = 0;
    //get record one by one from file
    while (1)
    { 
        if (fgetc(t_ptr) == EOF)
            break;
        fseek(t_ptr, -1, SEEK_CUR);
        fread(&account_name_len, sizeof(int), 1, t_ptr);
        newrec.account_name = malloc(sizeof(char) * account_name_len);
        fread(newrec.account_name, account_name_len, 1, t_ptr);
        fread(&account_password_len, sizeof(int), 1, t_ptr);
        newrec.account_password = malloc(sizeof(char) * account_password_len);
        fread(newrec.account_password, account_password_len, 1, t_ptr);
        decrypt(newrec.account_name, ENCRYPTION_KEY);
        decrypt(newrec.account_password, ENCRYPTION_KEY);
        int hash_value = hashFunction(newrec.account_name);
        insertAtBeg(records_hash_table->buckets[hash_value], newrec.account_name, newrec.account_password);
        free(newrec.account_name);
        free(newrec.account_password);
        i++;
    }
    fclose(t_ptr);
    if (i == 0)
    {
        printf("\nNo records found :(\n");
        printf("\npress any key to go back: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        goto cleanup;
    }
    SEARCH : {
        char *searchKey = NULL;
        printf(" __________________\n");
        printf("|no.of records: %3d|\n", i);
        printf(" ------------------\n\n\n\n");
        do {
            printf("SEARCH: ");
            searchKey = getstr();
        } while (searchKey == NULL);
        int search_num = hashFunction(searchKey);
        //array of node pointers to store the locations of nodes in which matching string was found
        node *catcharray[records_hash_table->buckets[search_num]->l_size];
        int len = searchList(records_hash_table->buckets[search_num], searchKey, catcharray);
        free(searchKey);
        int choice;
        if (len == 0)
            printf("\n\nNo match found.\n");
        else
        {
            printf("\n\nFollowing options were found: \n\n");
            for (int i = 0; i < len; i++) {
                printf("%0d. %s\n\n", i+1, catcharray[i]->ac_name);
            }
            printf("__________________________________________\n");
            do  {
                fseek(stdin, 0, SEEK_END);
                printf("\nSelect option: ");
                scanf("%d", &choice);
            } while (choice <= 0 || choice > len);
            choice--;
            printf("\nDetails:\n\nAccount Name: %s\nPassword: %s\n\n", catcharray[choice]->ac_name, catcharray[choice]->ac_pas);
        }
    }
    int ch;
    printf("\npress 'r' to search more records, any other key to continue: ");
    fseek(stdin, 0, SEEK_END);
    ch = getchar();
    if (ch == 'r' || ch == 'R')
        goto SEARCH;
    
    cleanup:
    deleteHashTable(&records_hash_table);
    free(records_hash_table);  
    return 0;
}

int editRecords(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "rb");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    } 
    record newrec;
    int account_name_len, account_password_len;
    //make a hash table to work on records
    hashtable *records_hash_table = createEmptyHashTable();
    if (records_hash_table == NULL)
    {
        printf("Error in loading the records to memory.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;
    }
    int i = 0;
    //get record one by one from file
    while (1)
    { 
        if (fgetc(t_ptr) == EOF)
            break;
        fseek(t_ptr, -1, SEEK_CUR);
        fread(&account_name_len, sizeof(int), 1, t_ptr);
        newrec.account_name = malloc(sizeof(char) * account_name_len);
        fread(newrec.account_name, account_name_len, 1, t_ptr);
        fread(&account_password_len, sizeof(int), 1, t_ptr);
        newrec.account_password = malloc(sizeof(char) * account_password_len);
        fread(newrec.account_password, account_password_len, 1, t_ptr);
        //printf("ftell: %ld\n", ftell(t_ptr));
        decrypt(newrec.account_name, ENCRYPTION_KEY);
        decrypt(newrec.account_password, ENCRYPTION_KEY);
        //load the record into the hash table at a designated linked list according to its hash_value
        int hash_value = hashFunction(newrec.account_name);
        insertAtBeg(records_hash_table->buckets[hash_value], newrec.account_name, newrec.account_password);
        free(newrec.account_name);
        free(newrec.account_password);
        i++;
    }
    fclose(t_ptr);
    search: 
    if (i == 0)
    {
        deleteHashTable(&records_hash_table);
        free(records_hash_table);
        printf("\nNo records found :(\n");
        printf("\npress any key to go back: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
  
        return 0;
    }
    //after this loop all our records will be loaded into the hash table 
    int flag_for_delete = 0;
    char *searchKey = NULL;

        printf(" __________________\n");
        printf("|no.of records: %3d|\n", i);
        printf(" ------------------\n\n\n\n");
        do {
            printf("SEARCH: ");
            searchKey = getstr();
        } while (searchKey == NULL);
        int search_num = hashFunction(searchKey);
        //array of node pointers to store the locations of nodes in which matching string was found
        node *catcharray[records_hash_table->buckets[search_num]->l_size];
        int len = searchList(records_hash_table->buckets[search_num], searchKey, catcharray);
        if (len == 0) {
            printf("\n\nNo match found.\n");
            printf("\npress 'r' to try again, any other key to exit: ");
            fseek(stdin, 0, SEEK_END);
            int get = getchar();
            if (get == 'r')
                goto search;
            goto cleanup;
        }
        else
        {
            printf("\n\nFollowing options were found: \n\n");
            for (int i = 0; i < len; i++) {
                //printf("add: %p\n", catcharr[i]);
                printf("%02d. %s\n\n", i+1, catcharray[i]->ac_name);
            }
            int choice;
            printf("__________________________________________\n");
            do  {
                fseek(stdin, 0, SEEK_END);
                printf("\nSelect option: ");
                scanf("%d", &choice);
            } while (choice <= 0 || choice > len);
            choice--;
            select: 
            
                printf("\nDetails:\n\nAccount Name: %s\nPassword: %s\n\n", catcharray[choice]->ac_name, catcharray[choice]->ac_pas);
                int ch;
                printf("\n1. Edit account name\n");
                printf("2. Edit account password\n");
                printf("3. Delete record\n");
                do  {
                    fseek(stdin, 0, SEEK_END);
                    printf("\nSelect option: ");
                    scanf("%d", &ch);
                } while (ch != 1 && ch != 2 && ch != 3);
                switch(ch)
                {
                    case 1: {   //change account name
                        char cur_name[100];
                        strcpy(cur_name, catcharray[choice]->ac_name);
                        free(catcharray[choice]->ac_name);
                        do {
                            printf("\n\nEnter new account name: ");
                            catcharray[choice]->ac_name = getstr();
                            if (catcharray[choice]->ac_name != NULL && (strcmp(catcharray[choice]->ac_name, cur_name)) == 0)
                                printf("\nNote: New account name is same as the previous account name\n\n");
                        } while (catcharray[choice]->ac_name == NULL);            
                        break;
                    }
                    case 2:    //change account password
                    {
                        char *password_confirm = NULL;
                        char cur_pass[100];
                        strcpy(cur_pass, catcharray[choice]->ac_pas);
                        do {
                            free(catcharray[choice]->ac_pas);
                            do {
                                printf("\n\nEnter new password: ");
                                catcharray[choice]->ac_pas = getstrMasked();
                                if (catcharray[choice]->ac_pas != NULL && (strcmp(catcharray[choice]->ac_pas, cur_pass)) == 0)
                                    printf("\nNew password is same as the previous password\n\n");
                            } while (catcharray[choice]->ac_pas == NULL);

                            do {
                                printf("Confirm password: ");
                                password_confirm = getstrMasked();
                            } while(password_confirm == NULL);

                            if (strcmp(catcharray[choice]->ac_pas, password_confirm))
                            {
                                printf("\nPasswords don't match. Try again.\n\n");
                                free(password_confirm);
                            }

                        }   while (strcmp(catcharray[choice]->ac_pas, password_confirm) != 0);
                        free(password_confirm);
                        break;
                    }
                    case 3: {    //delete the record
                        int val = hashFunction(catcharray[choice]->ac_name);
                        deleteAtLoc(records_hash_table->buckets[val], catcharray[choice]);
                        flag_for_delete++;
                        i--;
                    }
                }   
        }
    t_ptr = fopen(RECORDS_FILE, "wb");
    for (int i = 0; i < HASHTABLE_MAX_IDX; i++)
    {
        node *trav = records_hash_table->buckets[i]->head;
        while (trav != NULL)
        {
            encrypt(trav->ac_name);
            encrypt(trav->ac_pas);
            //serialization of the struct -> record's data
            int account_name_len = strlen(trav->ac_name) + 1;
            int account_password_len = strlen(trav->ac_pas) + 1;
            fwrite(&account_name_len, sizeof(int), 1, t_ptr);
            fwrite(trav->ac_name, account_name_len, 1, t_ptr);
            fwrite(&account_password_len, sizeof(int), 1, t_ptr);
            fwrite(trav->ac_pas, account_password_len, 1, t_ptr);
            decrypt(trav->ac_name, ENCRYPTION_KEY);
            decrypt(trav->ac_pas, ENCRYPTION_KEY);
            trav = trav->next;
        }
    }
    int pnp;
    printf("\nChanges made successfully\n");
    if (flag_for_delete != 0)
    {
        printf("\n\nPress 'r' to edit another record.\nPress any other key to return: ");
        fseek(stdin, 0, SEEK_END);
        pnp = getchar();
        if (pnp == 'r' || pnp == 'R')
            goto search;
        goto cleanup;
    }
    int inp;
    printf("\n\nPress 'r' to edit the same record again.\nPress 'b' to make changes to any other record.\nPress any other key to return: ");
    fseek(stdin, 0, SEEK_END);
    inp = getchar();
    if (inp == 'r' || inp == 'R')
        goto select;
    else if (inp == 'b' || inp == 'B')
           goto search;

    cleanup:
    fclose(t_ptr);   
    free(searchKey);
    deleteHashTable(&records_hash_table);
    free(records_hash_table);  
    return 0;
}

int makeDatabaseCopy(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "rb");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    } 
    record newrec;
    int account_name_len, account_password_len;
    //make a hash table to work on records
    hashtable *records_hash_table = createEmptyHashTable();
    if (records_hash_table == NULL)
    {
        printf("Error in loading the records to memory.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;
    }
    int i = 0;
    //get record one by one from file
    while (1)
    { 
        if (fgetc(t_ptr) == EOF)
            break;
        fseek(t_ptr, -1, SEEK_CUR);
        fread(&account_name_len, sizeof(int), 1, t_ptr);
        newrec.account_name = malloc(sizeof(char) * account_name_len);
        fread(newrec.account_name, account_name_len, 1, t_ptr);
        fread(&account_password_len, sizeof(int), 1, t_ptr);
        newrec.account_password = malloc(sizeof(char) * account_password_len);
        fread(newrec.account_password, account_password_len, 1, t_ptr);
        //printf("ftell: %ld\n", ftell(t_ptr));
        decrypt(newrec.account_name, ENCRYPTION_KEY);
        decrypt(newrec.account_password, ENCRYPTION_KEY);
        //load the record into the hash table at a designated linked list according to its hash_value
        int hash_value = hashFunction(newrec.account_name);
        insertAtBeg(records_hash_table->buckets[hash_value], newrec.account_name, newrec.account_password);
        free(newrec.account_name);
        free(newrec.account_password);
        i++;
    }
    fclose(t_ptr);
    if (i == 0)
    {
        printf("\nUnable to make backup. No records found :(\n");
        printf("\npress any key to go back: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        goto CLEANUP;
    }
    char FILE_PATH[200];
    strcpy(FILE_PATH, getenv("ONEDRIVE"));
    strcat(FILE_PATH, "\\Documents\\");
    strcat(FILE_PATH, DATABASE_CPY_FILE);
    t_ptr = fopen(FILE_PATH, "w");
    if (t_ptr == NULL)
    {
        deleteHashTable(&records_hash_table);
        free(records_hash_table);
        printf("Error. Couldn't make a copy of the database.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;
    }
    int ct = 1;
    fprintf(t_ptr, "RECORDS:\n");
    fprintf(t_ptr, "_________________________________________________________________________________________________\n");
    fprintf(t_ptr, "\n\n\n\n\n");
    for (int i = 0; i < 26; i++)
    {
        node *trav = records_hash_table->buckets[i]->head;
        while (trav != NULL)
        {
            fprintf(t_ptr, "%02d. Account Name: %s\n    Password: %s\n\n", ct, trav->ac_name, trav->ac_pas);
            trav = trav->next;
            ct++;
        }
    }
    fclose(t_ptr);
    printf("Copy of database made successfully!. Records will be found at %s\n", FILE_PATH);
    printf("\npress any key to continue: ");
    fseek(stdin, 0, SEEK_END);
    getchar();
    CLEANUP:
    deleteHashTable(&records_hash_table);
    free(records_hash_table);  
    return 0;
}

int print2DHashMap(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "rb");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    } 
    record newrec;
    int account_name_len, account_password_len;
    //make a hash table to work on records
    hashtable *records_hash_table = createEmptyHashTable();
    if (records_hash_table == NULL)
    {
        printf("Error in loading the records to memory.\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;
    }
    int i = 0;
    //get record one by one from file
    while (1)
    { 
        if (fgetc(t_ptr) == EOF)
            break;
        fseek(t_ptr, -1, SEEK_CUR);
        fread(&account_name_len, sizeof(int), 1, t_ptr);
        newrec.account_name = malloc(sizeof(char) * account_name_len);
        fread(newrec.account_name, account_name_len, 1, t_ptr);
        fread(&account_password_len, sizeof(int), 1, t_ptr);
        newrec.account_password = malloc(sizeof(char) * account_password_len);
        fread(newrec.account_password, account_password_len, 1, t_ptr);
        //printf("ftell: %ld\n", ftell(t_ptr));
        decrypt(newrec.account_name, ENCRYPTION_KEY);
        decrypt(newrec.account_password, ENCRYPTION_KEY);
        //load the record into the hash table at a designated linked list according to its hash_value
        int hash_value = hashFunction(newrec.account_name);
        insertAtBeg(records_hash_table->buckets[hash_value], newrec.account_name, newrec.account_password);
        free(newrec.account_name);
        free(newrec.account_password);
        i++;
    }
    fclose(t_ptr);
    if (i == 0)
    {
        printf("\nCan't make a HASHMAP. No records found :(\n");
        printf("\npress any key to go back: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        goto CLEANUP;
    }
    printf("_____________________________________________________________________________________________________________________________________\n\n");
    printf("                                                       __________________________\n");
    printf("                                                      |2-D HASHMAP REPRESENTATION|\n");
    printf("                                                       --------------------------\n\n\n\n\n\n\n\n");
    print2dhashtable(*records_hash_table);
    printf("\n\n\n\n\n\npress any key to continue: ");
    fseek(stdin, 0, SEEK_END);
    getchar();
    CLEANUP:
    deleteHashTable(&records_hash_table);
    free(records_hash_table);  
    return 0;
}

int loadDatabaseWritingMode(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "ab");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    }
    get_record: 
    {
        record newrec;
        do {
            printf("enter account name: ");
            //fseek(stdin, 0, SEEK_END);
            newrec.account_name = getstr();
        }
        while (newrec.account_name == NULL);

        char *password_confirm = NULL;
        do {
            do {
                printf("enter password: ");
                newrec.account_password = getstrMasked();
            } while(newrec.account_password == NULL);
            
            do {
                printf("enter password again to confirm: ");
                password_confirm = getstrMasked();
            } while(password_confirm == NULL);

            if (strcmp(newrec.account_password, password_confirm))
            {
                printf("\nPasswords don't match. Try again.\n\n");
                free(newrec.account_password);
                free(password_confirm);
            }
        }   while (strcmp(newrec.account_password, password_confirm) != 0);
        free(password_confirm);

        //encrypt the user-data
        encrypt(newrec.account_name);
        encrypt(newrec.account_password);
        //serialization of the struct -> record's data
        int account_name_len = strlen(newrec.account_name) + 1;
        int account_password_len = strlen(newrec.account_password) + 1;
        fwrite(&account_name_len, sizeof(int), 1, t_ptr);
        fwrite(newrec.account_name, account_name_len, 1, t_ptr);
        fwrite(&account_password_len, sizeof(int), 1, t_ptr);
        fwrite(newrec.account_password, account_password_len, 1, t_ptr);
        free(newrec.account_name);
        free(newrec.account_password);
    }

    int inp;
    printf("\nRecord added!\n");
    printf("\n\nPress 'r' to enter another record. Press any other key to return: ");
    fseek(stdin, 0, SEEK_END);
    inp = getchar();
    if (inp == 'r' || inp == 'R')
        goto get_record;
    else
        fclose(t_ptr);
    return 0;
}

int loadDatabaseReadingMode(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "rb");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database. Try entering new records to fix the problem\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    } 
    record newrec;
    int account_name_len, account_password_len;
    printf("Records:\n");
    //char records[200];
    int i = 1;
    while (1)
    { 
        if (fgetc(t_ptr) == EOF)
            break;
        fseek(t_ptr, -1, SEEK_CUR);
        fread(&account_name_len, sizeof(int), 1, t_ptr);
        newrec.account_name = malloc(sizeof(char) * account_name_len);
        fread(newrec.account_name, account_name_len, 1, t_ptr);
        fread(&account_password_len, sizeof(int), 1, t_ptr);
        newrec.account_password = malloc(sizeof(char) * account_password_len);
        fread(newrec.account_password, account_password_len, 1, t_ptr);
        decrypt(newrec.account_name, ENCRYPTION_KEY);
        decrypt(newrec.account_password, ENCRYPTION_KEY);
        printf("\n%02d. Account Name: %s\n    Password: %s\n", i, newrec.account_name, newrec.account_password);
        free(newrec.account_name);
        free(newrec.account_password);
        i++;
    }
    fclose(t_ptr);
    if (i == 1)
        printf(".....\n.....\n.....\n.....\n\nRecords empty\n");
    printf("\n______________________________________________________________________________________\n");
    printf("\nPress any key to go back -> ");
    fseek(stdin, 0, SEEK_END);
    getchar();
    return 0;
}

int fillWithTestData(void)
{
    FILE *t_ptr = fopen(RECORDS_FILE, "ab");
    if (t_ptr == NULL)
    {
        printf("Error in accessing the database\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    }
    FILE *ptr1 = fopen(TEST_DATA_NAMES_FILE, "r");
    if (ptr1 == NULL)
    {
        fclose(t_ptr);
        printf("Error in accessing the testfile\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    }
    FILE *ptr2 = fopen(TEST_DATA_PASS_FILE, "r");
    if (ptr2 == NULL)
    {
        fclose(t_ptr);
        fclose(ptr1);
        printf("Error in accessing the testfile\n\n");
        printf("press any key to continue: ");
        fseek(stdin, 0, SEEK_END);
        getchar();
        return 1;   
    }
    for (int i = 0; i < TEST_DATA_RECORDS; i++) 
    {
        record newrec;
        newrec.account_name = malloc(sizeof(char) * 100);
        newrec.account_password = malloc(sizeof(char) * 100);
        int i = 0;
        int ch = EOF;
        while (ch)
        {
            ch = fgetc(ptr1);
            if (ch == '\n' || ch == EOF)
                ch = '\0';
            newrec.account_name[i] = ch;
            i++;
        }
        ch = EOF;
        i = 0;
        while (ch)
        {
            ch = fgetc(ptr2);
            if (ch == '\n')
                ch = '\0';
            newrec.account_password[i] = ch;
            i++;
        }
        //encrypt the user-data
        encrypt(newrec.account_name);
        encrypt(newrec.account_password);
        //serialization of the struct -> record's data
        int account_name_len = strlen(newrec.account_name) + 1;
        int account_password_len = strlen(newrec.account_password) + 1;
        fwrite(&account_name_len, sizeof(int), 1, t_ptr);
        fwrite(newrec.account_name, account_name_len, 1, t_ptr);
        fwrite(&account_password_len, sizeof(int), 1, t_ptr);
        fwrite(newrec.account_password, account_password_len, 1, t_ptr);
        free(newrec.account_name);
        free(newrec.account_password);
    }
    fclose(t_ptr);
    fclose(ptr1);
    fclose(ptr2);
    printf("\n\nDone. Press any other key to return: ");
    fseek(stdin, 0, SEEK_END);
    getchar();
    return 0;
}

void encrypt(char str[])
{
    for (int i = 0; str[i] != '\0'; i++)
    {   
        if (str[i] != ',' && str[i] != ' ')
            str[i] += ENCRYPTION_KEY;
    }      
}

void decrypt(char str[], int key)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] != ',' && str[i] != ' ')   
            str[i] -= key; 
    }
}

void generateRandomString(void)
{
   
    printf("WELCOME TO RANDOM PASSWORD GENERATOR\n");
    printf("________________________________________________________\n");
    int len = 8;
    again:
    printf("\n\nenter length for your password: ");
    fseek(stdin, 0, SEEK_END);
    scanf("%d", &len);

    if (len < 6 || len > 12)
    {
        printf("\nChoose length between 6-12 characters only.\n");
        len = 8;
        goto again;
    }

    char *data = "abcdefghijklmnopqrstuvwxyz!@#_$&ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    int datal = strlen(data);
    srand(time(NULL));
    printf("\n\n\n");
    printf(" _______________\n");
    printf("|RANDOM PASSWORD|: "); 
    for (int i = 0; i < len; i++)
    {
        int j = rand() % datal;
        printf("%c", data[j]);
    }
    printf("\n");
    printf(" ---------------\n");
    printf("\n\n\n\n\n\n______________________________________________________________________________________\n");
    int ch;
    printf("\nPress 'r' to generate another password, any other key to go back -> ");
    fseek(stdin, 0, SEEK_END);
    ch = getchar();
    if (ch == 'r' || ch == 'R')
        goto again;
}

char* getstr(void)
{
    char *str = NULL, *tmp = NULL;
    int ch = EOF;
    int size = 0, idx = 0, i = 0;
    fseek(stdin, 0, SEEK_END);
    //printf("enter a string: ");
    while (ch)
    {
        ch = getchar();
        //printf("ch: %c\n", ch);
        if (ch == '\n' && i == 0)  //checks for unprintable characters. '\n' is allowed but should not be the first character of the entered string, which we check by variable 'i'            
            break;
        i++;
        if (ch == '\n' || ch == EOF)
            ch = 0;
        
        if (size <= idx)
        {
            size += sizeof(char);
            tmp = realloc(str, size);
            if (tmp == NULL)
            {
                free(str);
                str = NULL;
                break;
            }
            str = tmp;
        }
        str[idx++] = ch;
    }
    //printf("str: %s\n", str);
    //printf("strlen: %d\n", strlen(str));
    return str;
}

//funcn to get masked string input from the user eg. "abc d" would be taken as *****
//returns the address of the string it takes as input from the user, if no string entered then NULL is returned
//make sure to free this address later in the program
char* getstrMasked(void)  
{
    char *str = NULL, *tmp = NULL;
    int ch = EOF;         // EOF = -1 is used to just to give an initial non-zero value           
    int idx = 0;              
    long long size = 0;       // to keep track of string size
    fseek(stdin, 0, SEEK_END);  //to clear any input bufferr

    while (ch)     //while ch != 0
    {
        ch = getch();   //getch() is used to get characters from the terminal without echoing (showing) them

        // If |ENTER| (here '\r') is pressed as the very first character of the string, or after deleting all the previously entered characters, then we exit the function by returning NULL because in this case our string would be empty
        if (ch == 13 && size == 0)  // size helps us to know, if ch is the first character of the string or not
        {   
            printf("\n"); 
            return NULL;
        }

        // when you press |ENTER| on keyboard, carriage return '\r', with value 13 is read by getch(). If getchar() was used it would have read new line feed '\n' with value 10 
        if (ch == 13)  
            ch = 0;    // so when '\r' is encountered it means we have finished entering our string, thus we intialize 'ch' to 0 (or '\0' (NUL) to be more precise) to indicate the end of our string

        // ASCII value 8 is assigned to '\b' |BACKSPACE|. So this case is for when we want to remove the last entered character
        if (ch == 8)   
        {
            size = size - sizeof(char);  //we decrement our previous size by 1, since the last character is to be deleted

            //Size Cases
            if (size == 0)   // if the size of reduced string is to be 0 after deletion, we make tmp and size point to NULL
            {
                free(str);   
                tmp = str = NULL;
                idx--;  // before this, idx was +1, we decrement it to 0
                       // so that if we want to add another character(s), then it would be written from idx 0 onwards
                printf("\b \b");  //this printf is used to delete the '*' from the terminal
            }
            else if (size < 0)    // if the size of string is to be less than 0 (i.e the user entered backspace even if all the characters were deleted or before any character was entered)
            {
                free(str);        //NOTE: calling free oN NULL is valid 
                tmp = str = NULL;   // we do the same as we did for case (size == 0), but idx & i are not decremented, because the new characters entered (if any) must start from idx 0 only
                size = 0;   //we set size to 0, because if backspace is entered once again, then the size would go in negative otherwise
            }
            else //if the new size of reduced string is to be greater than 1
            {
                tmp = realloc(str, size);    // resize string using realloc(), it takes care of copying the values from idx 0 to size-1 in the case of shrinkage
                if (tmp == NULL)
                {
                    free(str);      //if realloc fails, set str to NULL and exit the while loop
                    str = NULL;
                    break;
                }
                idx--;  //decrement idx by 1 so that idx now points to the index that any new entered character must go to
                printf("\b \b");  //this printf is used to delete the '*' from the terminal
            }
            continue; //to jump to the next cycle of loop
        }

        // if any character other than |ENTER| or |BACKSPACE| is entered 
        size = size + sizeof(char);   //increment size by 1
        tmp = realloc(str, size);    // resize string using realloc(), it takes care of copying the previous values
        if (tmp == NULL)
        {
            free(str);            //if realloc fails, set str to NULL and exit the while loop
            str = NULL;
            break;
        }
        str = tmp;            //point str to same chunk of memory as tmp
        str[idx++]= ch;      //put the entered character ch at index idx, and post-increment idx
        if (ch != 0)        //dont print * when ch = 0 (i.e when |ENTER| is pressed)
            printf("*");   //this printf prints stars - **** - that are commonly used for masking passwords  
    }

    printf("\n");       //prints newline after the stars ******     
    return str;        //return the address of the string
}

int changeLOGIN_PASS()
{

    char *pre_entry = NULL;
    Label_1: {
    do {
        printf("\nEnter the curent password: ");
        pre_entry = getstrMasked();
    } while (pre_entry == NULL); }

    if (strcmp(pre_entry, LOGIN_PASS))
    {
        printf("\nIncorrect password!\n");
        printf("press 'r' to try again, any other key to go back: ");
        char inp;
        fseek(stdin, 0, SEEK_END);
        inp = getchar();
        if (inp == 'r' || inp == 'R')
            goto Label_1;
        return 1;
    }
    free(pre_entry);

    int opt;
    printf("\nCorrect!\n\n\nSelect:\n");
    printf("1.Set a new password\n");
    printf("2.Reset to default password\n");
    printf("______________________________________________________________________\n");
    printf("-> ");
    fseek(stdin, 0, SEEK_END);
    scanf("%d", &opt);

    switch (opt)
    {
        case 1: {           //take password from user
            FILE *pasws = fopen(LOGIN_PASSWORD_FILE, "w");
            if (pasws == NULL)
            {
                printf("Error__Couldn't change password at the moment. Please try again later.\n");
                printf("\n\n\npress any key to continue: ");
                fseek(stdin, 0, SEEK_END);
                getchar();
                return 1;
            }
            char *new_pass = NULL;
            char *new_pass_confirm = NULL;
            int cmp_val = 0;
            int len = 0;
            do {
                do {
                    printf("enter new password: ");
                    new_pass = getstrMasked();
                    if (new_pass != NULL && ((len = strlen(new_pass)) < 4 || (len = strlen(new_pass)) > 12)) {
                        printf("\nChoose length between 4-12 characters only.\n\n");
                        free(new_pass);
                    }
                    if (new_pass != NULL && (cmp_val = strcmp(new_pass, LOGIN_PASS)) == 0) {
                        printf("\nChoose a password different from the current password.\n\n");
                        free(new_pass);
                    }
                } while(new_pass == NULL || len < 4 || len > 12 || cmp_val == 0);
            
                do {
                    printf("enter password again to confirm: ");
                    new_pass_confirm = getstrMasked();
                } while(new_pass_confirm == NULL);

                if ((cmp_val = strcmp(new_pass, new_pass_confirm)) != 0) {
                    printf("\nPasswords don't match. Try again.\n\n");
                    free(new_pass);
                    free(new_pass_confirm);
                }

            }   while (cmp_val != 0);

            //change login pass here, so if the user may want to change password again without closing the program, 
            //they have to enter the new password and not the old one
            char *tmp = LOGIN_PASS;   //-------------------------------------------->try to write this section better later
            //strcpy(tmp, LOGIN_PASS);
            //free(LOGIN_PASS);
            LOGIN_PASS = malloc(sizeof(char) * (strlen(new_pass) + 1));
            if (LOGIN_PASS == NULL)
                LOGIN_PASS = tmp;
            else
            {
                strcpy(LOGIN_PASS, new_pass);
                free(tmp);
            }
            encrypt(new_pass);
            fprintf(pasws, "LOGIN_PASS: %s\n", new_pass);
            fclose(pasws);
            free(new_pass);
            free(new_pass_confirm);
            printf("\nPassword updated successfully! Use the new password for logging in next time.\n");
            printf("\n\nPress any key to go back -> ");
            fseek(stdin, 0, SEEK_END);
            getchar();
            return 0;
        }

        case 2: {                //set to default password
            FILE *ptr = fopen(LOGIN_PASSWORD_FILE, "r");
            if (ptr != NULL)  //if file exist then remove it
            {
                fclose(ptr);
                if (remove(LOGIN_PASSWORD_FILE) == 0)
                {
                    char *tmp = LOGIN_PASS; 
                    //free(LOGIN_PASS);
                    LOGIN_PASS = malloc(sizeof(char) * (strlen(default_pass) + 1));
                    if (LOGIN_PASS == NULL)
                        LOGIN_PASS = tmp;
                    else
                    {
                        strcpy(LOGIN_PASS, default_pass);
                        free(tmp);
                    }
                    //strcpy(LOGIN_PASS,default_pass);
                    printf("Login password set to default successfully - 0000\n");
                    printf("\nPress any key to go back -> ");
                    fseek(stdin, 0, SEEK_END);
                    getchar();
                    return 0;
                }
                else
                {
                    printf("Error in changing the password\n");
                    printf("\n\n\nPress any key to continue -> ");
                    fseek(stdin, 0, SEEK_END);
                    getchar();
                    return 1;
                }
            } 
            else
            {
                printf("Password is already set to default - 0000\n");
                printf("\n\n\nPress any key to continue -> ");
                fseek(stdin, 0, SEEK_END);
                getchar();
                return 1;
            }
        }

        default:
            printf("Invalid option\n");
            printf("\n\n\nPress any key to continue -> ");
            fseek(stdin, 0, SEEK_END);
            getchar();
            return 0;
    }

}