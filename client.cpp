#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define PORT 3002

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server; // structura folosita pentru conectare
    char msg[400];             // mesajul trimis

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Eroare la socket().\n");
        return errno;
    }

    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
    server.sin_port = htons(PORT);

    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    while(1){
        bzero(msg, 400);
        read(0, msg, 400);
        if(strncmp("search", msg, 6) == 0){
            char author[100], title[200], genres[100], an[10], isbn[20], rating[5];
            printf("Please provide more details or press ENTER for all of them.\n");
            printf("Author: ");
            fgets(author, 100, stdin);
            printf("--------len: %d\n", strlen(author));
            printf("Title: ");
            fgets(title, 200, stdin);
            printf("--------len: %d\n", strlen(title));
            printf("Genres (separated only by a comma): ");
            fgets(genres, 100, stdin);
            printf("--------len: %d\n", strlen(genres));

            char command[400];
            strcpy(command, "search#author:");
            strncat(command, author, strlen(author)-1);
            strcat(command, "#title:");
            strncat(command, title, strlen(title)-1);
            strcat(command, "#genre:");
            strncat(command, genres, strlen(genres)-1);

            write(sd, command, 400);
        }
        else
            if (write(sd, msg, 400) <= 0){
                perror("[client]Eroare la write() spre server.\n");
                return errno;
            }

        /* citirea raspunsului dat de server
        (apel blocant pana cand serverul raspunde); Atentie si la cum se face read- vezi cursul! */

        bzero(msg, 400);
        printf("\n");
        if (read(sd, msg, 400) < 0){
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }

        printf("[server]--> %s\n", msg);
        if(strncmp("Server stopped!\n", msg, 16) == 0)
            break;
    }
    close(sd);
    return 0;
}