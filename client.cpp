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

#define PORT 2024

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server; // structura folosita pentru conectare
    char msg[100];             // mesajul trimis

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
        bzero(msg, 100);
        read(0, msg, 100);

        if (write(sd, msg, 100) <= 0){
            perror("[client]Eroare la write() spre server.\n");
            return errno;
        }

        /* citirea raspunsului dat de server
        (apel blocant pana cand serverul raspunde); Atentie si la cum se face read- vezi cursul! */

        bzero(msg, 100);
        if (read(sd, msg, 100) < 0){
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }

        printf("[server]--> %s", msg);
        if(strncmp("stop", msg, 4) == 0)
            break;
    }
    close(sd);
    return 0;
}