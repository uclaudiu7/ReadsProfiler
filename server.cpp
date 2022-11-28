#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"

using namespace std;

#define PORT 2024

int main(){
    struct sockaddr_in server;
    struct sockaddr_in from;
    char msg[100];
    char rasp[100]= " ";
    int sd;

    UserDatabase db;

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

    db.openUserDatabase();
    while(1){
        bzero(msg, 100);

        if(read(client, msg, 100) <= 0){
            perror("[server]Eroare la read() de la client.\n");
        }

        printf("[client]--> %s", msg);

        char username[10] = "";
        char password[10] = "";
        if(strncmp(msg, "register", 8) == 0){
            int i = 9, j = 0;
            while(msg[i] != ' ')
                username[j++] = msg[i++];
            username[j] = '\0';
            i++, j = 0;
            while(msg[i] != ' ' && msg[i] != '\0')
                password[j++] = msg[i++];
            printf("user: %s\npass: %s\n", username, password);
            db.insertUser(username, password);
        }
        db.closeUserDatabase();

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