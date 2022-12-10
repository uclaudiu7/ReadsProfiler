#include <iostream>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "users.h"
#include "books.h"

using namespace std;

#define PORT 3002
#define MAX_THREADS 100

pthread_t threads[MAX_THREADS];
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
    const char *sqlStatement;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9780747532743', " \
                    "'Harry Potter and the Philosopher`s Stone', " \
                    "'J.K.Rowling', " \
                    "'1997', 'Magic', 'Fiction', '10');";
    printf("---->> *%s*\n", sqlStatement);
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9780747538493', " \
                    "'Harry Potter and the Chamber of Secrets', " \
                    "'J.K.Rowling', " \
                    "'1998', 'Magic', 'Fiction', '9');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9781408703991', " \
                    "'The Cuckoo`s Calling', " \
                    "'J.K.Rowling', " \
                    "'2013', 'Crime', 'Fiction', '7');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9780679745587', " \
                    "'In Cold Blood', " \
                    "'TrumanCapote', " \
                    "'1995', 'Crime', 'Nonfiction', '8');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9781542005227', " \
                    "'If You Tell', " \
                    "'GreggOlsen', " \
                    "'2019', 'Nonfiction', 'Biography', '6');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    

    sqlite3_close(myDatabase);
}

void createTable(const char *sqlStatement, const char *tableName){
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
    const char *sqlStatement;
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
    createTable(sqlStatement, "BOOKS");
    addDefaultBooks();
}

const char *handleRegister(char command[100], User &u){
    /*###### check if command is register <user> <pass> <pass> ######*/
    /*######        check if pasword matches in database       ######*/

    char username[20] = "";
    char password[20] = "";
    char password2[20] = "";

    

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

const char *handleDelete(User &u){ return u.deleteUser(); }

const char *handleLogin(char command[100], User &u){
    if(u.isLogged())
        return "You are already logged in!\n";
    char username[100] = "";
    char password[100] = "";
    
    int fields = sscanf(command, "login %s %s", username, password);
    if(fields = 2)
        return u.loginUser(username, password);
    else
        return "Please login using: login <username> <password> !\n";
}

const char *handleLogout(User &u){
    return u.logoutUser();
}

const char *handleStatus(User &u){
    if(u.isLogged() == true)
        return "You are logged in!\n";
    else
        return "You are not logged in!\n";
}

const char *handleHelp(){
    return "Available commands: help, register, delete account, login, logout, status, stop.\n";
}

const char *handleSearch(char command[100], User &u, int &client){
    char author[50], title[100], year[6];
    write(client, "Please enter details about the book, or press ENTER:\nauthor: ", 61);
    read(client, author, 50);
    write(client, "title: ", 7);
    read(client, title, 100);

    char *response = new char[500];
    char *r;
    strcpy(r, "Ai ales autorul ");
    strcat(r, author);
    strcat(r, " si titlul ");
    strcat(r, title);

    strcpy(response, r);
    return response;
}

const char *handleRecommend(User &u){
    return u.recommend();
}

const char *handleCommand(char command[100], User &u){
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
    if(strncmp(command, "search", 6) == 0){
        return "Success\n";
    }

    if(strncmp(command, "recommend", 9) == 0)
        return handleRecommend(u);

    return "Invalid command. Type 'help' to display available commands!\n";
}

void *handleThread(void *arg){
    int client = *(int *)arg;
    char command[400];
    char response[400];
    User u;

    while(1){
        bzero(command, 400);

        if(read(client, command, 400) <= 0){
            perror("[server]Eroare la read() de la client.\n");
        }

        printf("[client]--> %s\n", command);

        if(strncmp("stop", command, 4) == 0){
            write(client, "Server stopped!\n", 16);
        }

        strcpy(response, handleCommand(command, u));

        write(client, response, strlen(response));
        bzero(response, 400);
    }

    close(client);
    return NULL;
}

int main(){
    struct sockaddr_in server;
    struct sockaddr_in from;
    char command[400];
    char response[400];
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

    createDatabase();

    User u;
    while(1){
        bzero(command, 400);
        int client = accept(sd, NULL, NULL);
        int threadIndex = 0;

        while(threads[threadIndex] != 0 && threadIndex < MAX_THREADS)
            threadIndex++;

        if(threadIndex < MAX_THREADS)
            pthread_create(&threads[threadIndex], NULL, handleThread, &client);
    }


    /*###### Closing the server socket and joining all of the threads we created ######*/
    close(sd);
    for(int i = 0; i < 100; i++)
        if(threads[i] != 0)
            pthread_join(threads[i], NULL);

    return 0;
}