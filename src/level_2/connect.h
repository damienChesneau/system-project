#ifndef _T_CONNECT_T_
#define _T_CONNECT_T_

#define _BSD_SOURCE  1
#define _SVID_SOURCE 1

#include "copy.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PACK_SIZE 2000
#define PORT 2022 /* 3450 a 3650 */
#define PORT_OFFSET 10
/*
int port_now = -1;
const char* firstmessage = "Hello, I want all your data please :)";*/

int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);
int getSockAddr(int port, struct sockaddr_in* serv);
void write_encoded_message(int fd,char * message,int length);
void read_encoded_message(int fd,char * message,int length);
void connectman(char **tab_addr,int length);
void * connexion_manager(void *arg);
void * switchman(void *arg);

#endif
