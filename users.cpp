#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

User::User(){ this->log_status = false; }

char *User::getName(){ return this->name; }

bool User::isLogged(){ return this->log_status; }

char *User::registerUser(char username[20], char password[20]){
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
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
        strcpy(responseChar, "User '");
        strcat(responseChar, username);
        strcat(responseChar, "' already exists. Try a different username!\n");
        
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        printf("User '%s' already exists. Try a different username!\n", username);
        sqlite3_free(ErrMsg);
    }
    else{
        strcpy(responseChar, "User '");
        strcat(responseChar, username);
        strcat(responseChar, "' registered successfully!\n");
        printf("User '%s' registered successfully!\n", username);
        this->log_status = true;
        strcpy(this->name, username);
    }
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    printf("AICI: '%s'\n", response);
    return response;
}

char *User::deleteUser(){
    if(log_status == false)
        return "You must be logged in first!\n";
    
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    strcpy(sqlChar, "DELETE FROM USERS WHERE USERNAME='");
    strcat(sqlChar, this->name);
    strcat(sqlChar, "';");

    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);    

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        return "SQL Error!\n";
        sqlite3_free(ErrMsg);
    }
    else{
        strcpy(responseChar, "User '");
        strcat(responseChar, this->name);
        strcat(responseChar, "' deleted from database!\n");
        this->log_status = false;
        //printf("User '%s' deleted from database!\n", username);
    }
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    return response;
}

char *User::loginUser(char *username, char *password){
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    printf("In comanda login am: '%s' si '%s'.\n", username, password);
    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    strcpy(sqlChar, "SELECT USERNAME FROM USERS WHERE USERNAME='");
    strcat(sqlChar, username);
    strcat(sqlChar, "' AND PASSWORD='");
    strcat(sqlChar, password);
    strcat(sqlChar, "';");
    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);    

    run = sqlite3_prepare_v2(myDatabase, sqlStatement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    run = sqlite3_step(s);
    if(run == SQLITE_ROW){
        //printf("Successfully connected as '%s'!\n", username);
        strcpy(responseChar, "Successfully connected as '");
        strcat(responseChar, username);
        strcat(responseChar, "'!\n");
        log_status = true;
        strcpy(this->name, username);
    }
    else{
        //printf("User '%s' not found. Please try loggin in again!\n", username);
        strcpy(responseChar, "Wrong credentials. Please try again!\n");  
    }
    sqlite3_finalize(s);
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    return response;
}   

char *User::logoutUser(){
    this->log_status = false;
}
