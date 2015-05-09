#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#define PORT 3450 /* 3450 a 3650 */
#define OFFSET 200
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */

int getServSockaddr(int port, struct sockaddr_in* serv);
int getSocket(int port, struct sockaddr_in* serv);

int main(int argc, char* argv[]) {
    char* ip_addr = NULL;
    char options = 0;
    char* optstring = "d:";
    int val;
    int sock_fd[OFFSET];
   	struct sockaddr_in sock[OFFSET];
   	struct sockaddr_in* psock[OFFSET]; 
    int i;

    while (EOF != (val = getopt(argc, argv, optstring))) {
        switch (val) {
            case 'd':
                options = 1;
                ip_addr = optarg;
                break;
        }
    }
    
    for(i = 0; i<OFFSET; i++){
    	sock_fd[i] = getSocket(PORT+i,&sock[i]);
    	if(sock_fd[i] == EXIT_FAILURE){
    		perror(argv[0]);
    		return EXIT_FAILURE;
    	}
    	psock[i]= &sock[i];
    }
    if(options == 0){
    	int client_fd[OFFSET];
    	socklen_t addrlen[OFFSET]; 
    	for(i = 0; i<OFFSET; i++){
    		addrlen[i] = sizeof(struct sockaddr_in);
    		client_fd[i] = accept(sock_fd[i],(struct sockaddr*)psock[i],&addrlen[i]);
    		if(client_fd[i] == EXIT_FAILURE){
    			perror(argv[0]);
    			return EXIT_FAILURE;
    		}
    	}
    }else{
    	for(i = 0; i<OFFSET; i++){
    		int serv_fd = connect(sock_fd[i],(struct sockaddr*)psock[i],sizeof(struct sockaddr_in));
    		if(serv_fd == EXIT_FAILURE){
    			/*There we must test if the port is already used or not */
    			perror(argv[0]);
    			return EXIT_FAILURE;
    		}
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

int getSocket(int port, struct sockaddr_in* serv) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (getServSockaddr(port, serv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (bind(fd, (struct sockaddr*) serv, sizeof (struct sockaddr_in)) == EXIT_FAILURE) {
        perror("getSocket");
        return EXIT_FAILURE;
    }
    
    if (listen(fd, 2000) == EXIT_FAILURE) {
        perror("getSocket");
        return EXIT_FAILURE;
    }
    return fd;
}

