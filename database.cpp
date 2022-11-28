#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database.h"

using namespace std;

sqlite3* myDatabase;

UserDatabase::UserDatabase(){userNumber = 0;}

void UserDatabase::openUserDatabase(){
    int runCommand;
    char *sqlStatement;
    char *ErrMsg = 0;

    runCommand = sqlite3_open("databasedb.db", &myDatabase);
    if(runCommand){
        fprintf(stderr, "Couldn't open database!\n");
    }
    else{
        fprintf(stderr,"Database has been opened!\n");
    }


    sqlStatement = "CREATE TABLE USERS(" \
                 "ID INT PRIMARY KEY NOT NULL," \
                 "USERNAME TEXT NOT NULL," \
                 "PASSWORD TEXT NOT NULL);";

    runCommand = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    
    if(runCommand != SQLITE_OK){
        fprintf(stderr, "SQL Error!\n");
        sqlite3_free(ErrMsg);
    }
    else{
        fprintf(stdout, "User table created!");
    }
}

void UserDatabase::closeUserDatabase(){
    sqlite3_close(myDatabase);
}

int UserDatabase::callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

bool UserDatabase::exists(char *username){

}

bool UserDatabase::login(char *username, char *password){

}

bool UserDatabase::insertUser(char *username, char *password){
    int runCommand;
    char *sqlStatement;
    char *ErrMsg = 0;

    sqlStatement = "INSERT INTO USERS VALUES(";
    if(userNumber == 1)
        strcat(sqlStatement,", 1,");
    if(userNumber == 2)
        strcat(sqlStatement,", 2,");
    if(userNumber == 3)
        strcat(sqlStatement,", 3,");
    if(userNumber == 4)
        strcat(sqlStatement,", 4,");

    strcat(sqlStatement, username);
    strcat(sqlStatement,", ");
    strcat(sqlStatement, password);
    strcat(sqlStatement,");");
    sqlStatement[strlen(sqlStatement)] = '\0';

    runCommand = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);

    if(runCommand != SQLITE_OK){
        fprintf(stderr,"SQL Insert Error.\n");
        sqlite3_free(ErrMsg);
    }
    else{
        fprintf(stdout, "User %s registered!", username);
    }
}
