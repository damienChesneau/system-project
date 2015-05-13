#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 2000 /* 3450 a 3650 */
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);
int getSockAddr(int port, struct sockaddr_in* serv);
const char* ip_addr = "127.0.0.1";
int port_now = PORT;

void * switchman(void *arg) {
    (void) arg;
    struct sockaddr_in addr;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (getSockAddr(PORT , &addr) == EXIT_FAILURE) {
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

        if (pthread_create(&new_manager, NULL, connexion_manager, (void*)(intptr_t) newSock) == -1) {
            perror("pthread_create");
            return (void*) EXIT_FAILURE;
        }
    }
    pthread_exit(NULL);
}

void * connexion_manager(void *arg) {
    int newSock = (intptr_t) arg;
    if (newSock != EXIT_FAILURE) {
        char buff[10];
        read(newSock, buff, 10);
        printf("%s\n", buff);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t thread1;
    if (pthread_create(&thread1, NULL, switchman, NULL) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    while (1){
        sleep(500000);
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

