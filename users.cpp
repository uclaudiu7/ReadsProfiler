#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void User::registerUser(char username[20], char password[20]){
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    char sqlChar[100];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    
    strcpy(sqlChar, "INSERT INTO USERS VALUES (");
    strcat(sqlChar, "'");
    strcat(sqlChar, username);
    strcat(sqlChar, "', '");
    strcat(sqlChar, password);
    strcat(sqlChar, "');");

    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Couldn't register user: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{
        printf("User '%s' registered successfully!\n", username);
    }
}