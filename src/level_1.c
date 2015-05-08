#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>

/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */

int getServSockaddr(int port, struct sockaddr_in* serv);
int getSocket(int port, struct sockaddr_in* serv);
int ecoute(int fd);
int envoie(int fd);

int main(int argc, char* argv[]) {
    char* ip_addr = NULL;
    char options = 0;
    char* optstring = "d:";
    int val;

    while (EOF != (val = getopt(argc, argv, optstring))) {
        switch (val) {
            case 'd':
                options = 1;
                ip_addr = optarg;
                break;
        }
    }
    return EXIT_SUCCESS;
}

int getServSockaddr(int port, struct sockaddr_in* serv) {
    if (port <= 1024){
        return EXIT_FAILURE;
    }        

    serv->sin_family = AF_INET;
    serv->sin_port = htons(port);
    serv->sin_addr.s_addr = INADDR_ANY;
    return EXIT_SUCCESS;
}

int envoie(int fd) {
    /* connect function ? */
    return fd;
}

int ecoute(int fd) {
    if (listen(fd, 2000) == EXIT_FAILURE) {
        perror("ecoute");
        return EXIT_FAILURE;
    }

    /* accept function ?*/
    return fd;
}

int getSocket(int port, struct sockaddr_in* serv) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (getServSockaddr(port, serv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (bind(fd, (struct sockaddr*) serv, sizeof (struct sockaddr_in)) == EXIT_FAILURE) {
        perror("getSocket");
        return EXIT_FAILURE;
    }
    return fd;
}

