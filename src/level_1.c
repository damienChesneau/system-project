#define _BSD_SOURCE  1

#define _SVID_SOURCE 1
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


#define PORT 2000 /* 3450 a 3650 */
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);
int getSockAddr(int port, struct sockaddr_in* serv);
const char* ip_addr = "127.0.0.1";
const char* firstmessage = "Hello, I want all your data please :)";
int port_now = PORT;
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

        socket_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (getSockAddr(PORT, &(addr[i])) == EXIT_FAILURE) {
            perror("GET SOCK ADDR");
            return (void *) EXIT_FAILURE;
        }

        if (inet_aton(tab_addr[i], &(addr[i].sin_addr.s_addr)) == -1) {
            perror("cnonnectman");
            return (void*) EXIT_FAILURE;
        }
        char buf[10000];
        if (connect(socket_fd[i], (struct sockaddr *) &(addr[i]), sizeof (struct sockaddr)) != -1) {
            write(socket_fd[i], firstmessage, strlen(firstmessage));
            if (read(socket_fd[i], buf, sizeof (buf)) == -1) {
                perror("connectman");
            }
            int nb_file = atoi(buf);
            int i;
            printf("%d\n", nb_file);
            for (i = 0; i < nb_file; i++) {
                if (read(socket_fd[i], buf, sizeof (buf)) == -1) {
                    perror("connectman");
                }
                printf("%s\n", buf);
            }
            /*Data * data = decode(buf);*/
        }
    }
    pthread_exit(NULL);
}

void * connexion_manager(void *arg) {

    int newSock = (intptr_t) arg;
    if (newSock != EXIT_FAILURE) {
        char buff[10000];
        read(newSock, buff, sizeof (buff));
        if (strcmp(buff, firstmessage) == 0) {
            /*TO DO HERE 
             CONSTRUCT STRUCT DATA*/
            /* char * dataencoded = encode(NULL, 0);*/
            struct dirent ** reader;
            int nb_file;
            if ((nb_file = scandir("./", &reader, NULL, alphasort)) == EXIT_FAILURE) {
                perror("copy_all");
                return (void *) EXIT_FAILURE;
            }
            snprintf(buff, 10000, "%d", nb_file);
            int i;
            write(newSock, buff, strlen(buff));

            for (i = 0; i < nb_file; i++) {
                write(newSock, reader[i]->d_name, strlen(reader[i]->d_name));
            }
            /*write(newSock, dataencoded, strlen(dataencoded));*/
        }
        /*printf("%s\n", buff);*/
    }
    pthread_exit(NULL);
}

void * switchman(void *arg) {
    (void) arg;
    struct sockaddr_in addr;
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
        int newSock = accept(socket_fd, NULL, NULL);
        pthread_t new_manager;

        if (pthread_create(&new_manager, NULL, connexion_manager, (void*) (intptr_t) newSock) == -1) {
            perror("pthread_create");
            return (void*) EXIT_FAILURE;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int nb = 0, i = 0;
    Data * data = NULL;
    get_data_form_dir("./files_to_sync", &nb, data); 
    for (i = 0; i < nb; i++) { /* bug herer */ 
        printf("---------------------%s\n", data[i].data);
    }

    char ** ips = (char **) malloc(sizeof (char *)*argc);
    pthread_t thread1;
    if (argc < 2) {
        if (pthread_create(&thread1, NULL, switchman, NULL) == -1) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    } else {

        int i = 0;
        for (i = 1; i < argc; i++) {
            ips[i - 1] = (char *) malloc(sizeof (char)*strlen(argv[i]));
            strcpy(ips[i - 1], argv[i]);
        }
        ips[argc] = NULL;
        connectman(ips);
    }
    while (1) {
        sleep(1000);
    }
    /*        if (inet_aton(ip_addr, &(addr.sin_addr.s_addr)) == -1) {
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

