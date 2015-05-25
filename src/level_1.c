#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "copy.h"

#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#define PORT 2022 /* 3450 a 3650 */
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);
int getSockAddr(int port, struct sockaddr_in* serv);
const char* ip_addr = "127.0.0.1";
const char* firstmessage = "Hello, I want all your data please :)";
int port_now = PORT;
char * to_syc;
void * connexion_manager(void *arg);

void * connectman(void *arg) {
    char **tab_addr = (char **) arg;
    int nb_ip = 0;
    while (tab_addr[nb_ip] != NULL) {
        nb_ip++;
    }

    struct sockaddr_in addr[nb_ip];
    int socket_fd[nb_ip];
    int i = 0;
    for (i = 0; i < nb_ip; i++) {
        /*printf("%s\n",tab_addr[i]);*/
        socket_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (getSockAddr(PORT, &(addr[i])) == EXIT_FAILURE) {
            perror("GET SOCK ADDR");
            return (void *) EXIT_FAILURE;
        }

        if (inet_aton(tab_addr[i], &(addr[i].sin_addr.s_addr)) == -1) {
            perror("cnonnectman");
            return (void*) EXIT_FAILURE;
        }
        if (connect(socket_fd[i], (struct sockaddr *) &(addr[i]), sizeof (struct sockaddr)) != -1) {
            printf("Connected to %d\n", socket_fd[i]);
            write(socket_fd[i], firstmessage, strlen(firstmessage));
            int strlen_message = -1;
            if (read(socket_fd[i], &strlen_message, sizeof (int)) == -1) {
                perror("connectman");
            }
            char messge[strlen_message];
            read(socket_fd[i], &messge, strlen_message);
            printf("RESULT= %s\n", messge);
            int length = -1;
            Data * data = decode(messge, &length);
            filter_and_replace(data, length);
        } else {
            printf("not connected :( \n");
        }
    }
    pthread_exit(NULL);
}

void * connexion_manager(void *arg) {
    int newSock = (intptr_t) arg;
    if (newSock != EXIT_FAILURE) {
        char buff[10000];
        printf("SERVEUR : connection acceped\n");
        read(newSock, buff, sizeof (buff));
        if (strcmp(buff, firstmessage) == 0) {
            int nb = 0;
            to_syc = "./files_to_sync";
            Data * data = get_data_form_dir( to_syc, &nb);
            char * encodeed_message = encode(data, nb - 1);
            int strlen_message = strlen(encodeed_message);
            write(newSock, &strlen_message, sizeof (int));
            write(newSock, encodeed_message, strlen_message);
        }
    }
    pthread_exit(NULL);
}

void * switchman(void *arg) {
    (void) arg;
    struct sockaddr_in addr;
    printf("INITIALIZE SERVEUR IN PORT= %d\n", PORT);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (getSockAddr(PORT, &addr) == EXIT_FAILURE) {
        perror("GET SOCK ADDR");
        return (void *) EXIT_FAILURE;
    }
    if (setAcceptSocket(&addr, socket_fd) == EXIT_FAILURE) {
        perror("SET ACCEPT SOCK");
        return (void *) EXIT_FAILURE;
    }

    while (1) {
        printf("WAIT NEW CONNEXION...\n");
        if (listen(socket_fd, 5) == -1) {
            printf("err\n");
        }

        int newSock = accept(socket_fd, NULL, NULL);
        printf("Connect accepted by serveur\n");
        pthread_t new_manager;

        if (pthread_create(&new_manager, NULL, connexion_manager, (void*) (intptr_t) newSock) == -1) {
            perror("pthread_create");
            return (void*) EXIT_FAILURE;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    char ** ips = (char **) malloc(sizeof (char *)*argc);
    pthread_t thread1;
    if (pthread_create(&thread1, NULL, switchman, NULL) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    int i = 0;
    for (i = 1; i < argc; i++) {
        ips[i - 1] = (char *) malloc(sizeof (char)*strlen(argv[i]));
        strcpy(ips[i - 1], argv[i]);
    }
    ips[argc] = NULL;
    sleep(3);
    connectman(ips);
    while (1) {
        sleep(1000);
    }
    /*  if (inet_aton(ip_addr, &(addr.sin_addr.s_addr)) == -1) {
    //            perror(argv[0]);
    //            return EXIT_FAILURE;
    //        }
    //        if (connect(socket_fd, (struct sockaddr *) &addr, sizeof (struct sockaddr)) == EXIT_FAILURE) {
    //            perror(argv[0]);
    //            return EXIT_FAILURE;
    //        }
    //        write(socket_fd, ip_addr, 10);*/
    return EXIT_SUCCESS;
}

int getSockAddr(int port, struct sockaddr_in* serv) {
    if (port <= 1024) {
        return EXIT_FAILURE;
    }
    serv->sin_family = AF_INET;
    serv->sin_port = htons(port);
    serv->sin_addr.s_addr = INADDR_ANY;
    return EXIT_SUCCESS;
}

int setAcceptSocket(struct sockaddr_in* serv, int socket_fd) {
    if (bind(socket_fd, (struct sockaddr*) serv, sizeof (struct sockaddr_in)) == EXIT_FAILURE) {
        perror("getSockAddr");
        return EXIT_FAILURE;
    }

    if (listen(socket_fd, PORT) == EXIT_FAILURE) {
        perror("getSockAddr");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

