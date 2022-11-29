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
#include "users.h"

using namespace std;

#define PORT 3000

sqlite3* myDatabase;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void createTable(char *sqlStatement, char *tableName){
    int run;
    char *ErrMsg = 0;

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
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


    sqlite3_close(myDatabase);
}

int main(){
    struct sockaddr_in server;
    struct sockaddr_in from;
    char msg[100];
    char rasp[100]= " ";
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

    while(1){
        bzero(msg, 100);

        if(read(client, msg, 100) <= 0){
            perror("[server]Eroare la read() de la client.\n");
        }

        printf("[client]--> %s", msg);

        char username[20] = "";
        char password[20] = "";
        if(strncmp(msg, "register", 8) == 0){
            int i = 9, j = 0;
            while(msg[i] != ' ')
                username[j++] = msg[i++];
            username[j] = '\0';
            i++, j = 0;
            while(msg[i] != ' ' && msg[i] != '\0')
                password[j++] = msg[i++];
            password[j] = '\0';
            printf("user: %s\npass: %s\n", username, password);
            
            User u;
            u.registerUser(username, password);

        }

        bzero(msg, 100);
        read(0, msg, 100);

        if(write(client, msg, strlen(msg)) <= 0){
            perror("[server]Eroare la write() catre client.\n");
        }
        if(strncmp("stop", msg, 4) == 0)
            break;
    }
    close(client);
    close(sd);
    return 0;
}