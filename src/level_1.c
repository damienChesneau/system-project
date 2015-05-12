#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#define PORT 2000 /* 3450 a 3650 */
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd) ;
int getSockAddr(int port, struct sockaddr_in* serv);

int main(int argc, char* argv[]) {
    char* ip_addr = "127.0.0.1";
    struct sockaddr_in addr;
    
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(getSockAddr(PORT+1,&addr) == EXIT_FAILURE){
    	perror(argv[0]);
    	return EXIT_FAILURE;
    }
    if(argc <2){
    	if(setAcceptSocket(&addr,socket_fd) == EXIT_FAILURE){
		  	perror(argv[0]);
		  	return EXIT_FAILURE;
		  }
		  
			while(1){
				int newSock = accept(socket_fd, NULL, NULL);
				/*printf("Bonjour\n");*/
				if(newSock != EXIT_FAILURE){
					while(1){
					char buff[10];
					read(newSock, buff, 10);
					printf("%s\n",buff);
					}
				}
			}
    }else{
    	if(inet_aton(ip_addr,&(addr.sin_addr.s_addr)) == -1){
				perror(argv[0]);
				return EXIT_FAILURE;
			}	
		  if(connect(socket_fd, (struct sockaddr *)&addr,  sizeof (struct sockaddr) )== EXIT_FAILURE ){
				perror(argv[0]);
				return EXIT_FAILURE;
			}
			write(socket_fd,ip_addr,10);
    }
    
    	
    return EXIT_SUCCESS;
}

int getSockAddr(int port, struct sockaddr_in* serv) {
    
    if (port <= 1024){
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

