#include <iostream>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "user.h"

using namespace std;

#define PORT 3002

sqlite3* myDatabase;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){ 
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void addDefaultBooks(){
    char *ErrMsg = 0;
    char sqlChar[100];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    
    strcpy(sqlChar, "INSERT INTO USERS VALUES (");
    
    sqlite3_close(myDatabase);
}

void createTable(char *sqlStatement, char *tableName){
    int run;
    char *ErrMSg = 0;

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMSg);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMSg);
        sqlite3_free(ErrMSg);
    }
    else{
        printf("Table %s created successfully!\n", tableName);
    }
}

void createDatabase(){
    FILE *exists = fopen("database.db", "r");
    if(exists){
        printf("Database already exists!\n");
        fclose(exists);
        return;
    }
    printf("Creating database...\n");
    char *sqlStatement;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    else{
        printf("Database opened successfully!\n");
    }    
    
    /*############## creating database tables ##############*/
    sqlStatement = "CREATE TABLE USERS(" \
                 "USERNAME TEXT PRIMARY KEY NOT NULL," \
                 "PASSWORD TEXT NOT NULL);";
    createTable(sqlStatement, "USERS");

    sqlStatement = "CREATE TABLE BOOKS(" \
                 "ISBN TEXT PRIMARY KEY NOT NULL," \
                 "TITLE TEXT NOT NULL, " \
                 "AUTHOR TEXT NOT NULL, " \
                 "YEAR TEXT NOT NULL, " \
                 "GENRE TEXT NOT NULL, " \
                 "SUBGENGRE TEXT NOT NULL, " \
                 "RATING TEXT NOT NULL);";
    addDefaultBooks();
    createTable(sqlStatement, "BOOKS");
}


char *handleRegister(char command[100], User &u){
    /*###### check if command is register <user> <pass> <pass> ######*/
    /*######        check if pasword matches in database       ######*/

    char username[20] = "";
    char password[20] = "";
    char password2[20] = "";

    printf("Command has len: %d\n", strlen(command));
    if(strlen(command) < 15)
        return "Please register using following command: register 'username' 'password' 'password'!\n";

    int i = 9, j = 0;
    while(command[i] != ' ')
        username[j++] = command[i++];
    username[j] = '\0';
    i++, j = 0;
    while(command[i] >= 33 && command[i] <= 126)
        password[j++] = command[i++];
    password[j] = '\0';
    i++, j = 0;
    while(command[i] >= 33 && command[i] <= 126)
        password2[j++] = command[i++];
    password2[j] = '\0';

    if(strlen(username) == 0 || strlen(password) == 0)
        return "Please register using following command: register 'username' 'password' 'password'!\n";

    if(strncmp(password2, password, strlen(password)) != 0){
        printf("Please register using following command: register 'username' 'password' 'password'!\n");
        return "Please register using following command: register 'username' 'password' 'password'!\n";
    }
    else{
        printf("user: '%s'\npass: '%s'\n", username, password);
        return u.registerUser(username, password);
    }
}

char *handleDelete(User &u){ return u.deleteUser(); }

char *handleLogin(char command[100], User &u){
    if(strlen(command) < 10)
        return "Please login using: login <username> <password> !\n";
    if(u.isLogged())
        return "You are already logged in!\n";
    char username[20] = "";
    char password[20] = "";
    int i = 6, j = 0;
    while(command[i] != ' ')
        username[j++] = command[i++];
    username[j] = '\0';
    i++, j = 0;
    while(command[i] >= 33 && command[i] <= 126)
        password[j++] = command[i++];

    if(strlen(username) == 0 || strlen(password) == 0)
        return "Please login using: login <username> <password> !\n";

    return u.loginUser(username, password);
}

char *handleLogout(User &u){
    u.logoutUser();
    return "Logged out!\n";
}

char *handleStatus(User &u){
    if(u.isLogged() == true)
        return "You are logged in!\n";
    else
        return "You are not logged in!\n";
}

char *handleHelp(){
    return "Available commands: help, register, delete account, login, logout, status, stop.\n";
}

char *handleCommand(char command[100], User &u){
    if(strncmp(command, "register", 8) == 0)
        return handleRegister(command, u);
    if(strncmp(command, "delete account", 14) == 0)
        return handleDelete(u);
    if(strncmp(command, "login", 5) == 0)
        return handleLogin(command, u);
    if(strncmp(command, "logout", 6) == 0)
        return handleLogout(u);
    if(strncmp(command, "status", 6) == 0)
        return handleStatus(u);
    if(strncmp(command, "stop", 4) == 0)
        return "stop";
    if(strncmp(command, "help", 4) == 0)
        return handleHelp();



    return "Invalid command. Type 'help' to display available commands!\n";
}

int main(){
    struct sockaddr_in server;
    struct sockaddr_in from;
    char command[100];
    char response[300];
    int sd;

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    bzero(&server, sizeof(server));   // preparing data structures
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if(bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1){
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    if(listen(sd, 5) == -1){
        perror("[server]Eroare la listen().\n");
        return errno;
    }

    int client;
    int length = sizeof(from);
    printf("[server]Asteptam la portul %d...\n", PORT);
    fflush(stdout);

    client = accept(sd, (struct sockaddr *) &from, (socklen_t *) &length);

    if(client < 0){
        perror("[server]Eroare la accept().\n");
    }

    createDatabase();

    User u;
    while(1){
        bzero(command, 100);

        if(read(client, command, 100) <= 0){
            perror("[server]Eroare la read() de la client.\n");
        }

        printf("[client]--> %s", command);

        if(strncmp("stop", command, 4) == 0){
            write(client, "Server stopped!\n", 16);
            break;
        }

        strcpy(response, handleCommand(command, u));
        write(client, response, strlen(response));

        bzero(response, 100);
    }
    close(client);
    close(sd);
    return 0;
}