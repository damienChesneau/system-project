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
	Do the bind and listen operation on the socket socket_fd with the sockaddr serv. 
	Returns 1 on success and -1 on failure.
*/
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);

/*
	Returns a socket with port and sockaddr serv. The adress is initialized on INNADDR_ANY and the family on AF_INET. 
	Returns 1 on success and -1 on failure.
*/
int getSockAddr(int port, struct sockaddr_in* serv);

/*
	Write the message of length on the fd by blocks of 2000 max. 
	Returns 1 on success and -1 on failure.
*/
int write_encoded_message(int fd,char * message,int length);

/*
	Read the message of length on the fd by blocks of 2000 max. 
	Returns 1 on success and -1 on failure.
*/
int read_encoded_message(int fd,char * message,int length);

/*
	Do the exchange of data enter this instance and all other instances in every adress of tab_addr.
	It gets all datas of others instances, send data of latest updates to other instances and update his own folder.
*/
void connectman(char **tab_addr,int length);

/*
	Get all the latest data.
	It's called when an other instance want's to connect on it. It send all data of the current folder, get updated data and update his own folder.
*/
void * connexion_manager(void *arg);

/*
	Manage every connections on this instance with threads.
*/
void * switchman(void *arg);

#endif
