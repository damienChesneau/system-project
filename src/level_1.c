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
#define PORT_OFFSET 10
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.
*/
int setAcceptSocket(struct sockaddr_in* serv, int socket_fd);
int getSockAddr(int port, struct sockaddr_in* serv);
const char* ip_addr = "127.0.0.1";
const char* firstmessage = "Hello, I want all your data please :)";
int port_now = -1;
char * to_syc;
void * connexion_manager(void *arg);


void connectman(char **tab_addr,int length) {
    
   /* char **tab_addr = (char **) arg;*/
    int nb_ip = length;
    int  nb_of_me_data;
    
    to_syc = "./files_to_sync2";
    
    Data * me_data = get_data_form_dir(to_syc,&nb_of_me_data);
    
    struct in_addr local;
    
	/*printf("OK\n");*/
    if(inet_aton("127.0.0.1",&local) == -1){
    	perror("connectman");
    	return;
    }
    
    
    struct sockaddr_in addr[nb_ip][PORT_OFFSET+1];
    int socket_fd[nb_ip][PORT_OFFSET+1];
    int connect_ret[nb_ip][PORT_OFFSET+1];
    int i = 0;
    
    for (i = 0; i < nb_ip; i++) {
        int j = 0;
        for(j = 0; j<=PORT_OFFSET; j++){
    
			/*printf("OK\n");*/
		    if((socket_fd[i][j] = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		    	perror("connectman");
		    	return;
		    }
		    
		    if (getSockAddr(PORT+j, &(addr[i][j])) == -1) {
		        perror("GET SOCK ADDR");
		        return;
		    }
			
			/*printf("OK\n");*/
		    if (inet_aton(tab_addr[i], &(addr[i][j].sin_addr.s_addr)) == -1) {
		        perror("connectman");
		        return;
		    }
        	
        	while(port_now == -1);
        	if(addr[i][j].sin_addr.s_addr != local.s_addr || htons(addr[i][j].sin_port) != port_now){
				if ((connect_ret[i][j] = connect(socket_fd[i][j], (struct sockaddr *) &(addr[i][j]), sizeof (struct sockaddr))) != -1) {
				    printf("Connected to %d\n", socket_fd[i][j]);
				    
				    if(write(socket_fd[i][j], firstmessage, strlen(firstmessage)) == -1){
				    	perror("connectman");
    					return;
				    }
				    /*printf("OK\n");*/
				    int strlen_message = -1;
				    if (read(socket_fd[i][j], &strlen_message, sizeof (int)) == -1) {
				        perror("connectman");
    					return;
				    }
				    char messge[strlen_message];
				    
				   	/*printf("OK\n");*/
				    if(read(socket_fd[i][j], &messge, strlen_message) == -1){
				    	perror("connectman");
    					return;
				    }
				    
				    int length = -1;
				    /*printf("OK\n");*/
				    Data * data = decode(messge, &length);
				    
				    filter_and_replace(me_data, &nb_of_me_data, data, &length);
				    
				    free_data(data,length);
				} else {
				    /*printf("not connected :( \n");*/
				}
		    }
			
        }
    }
    /*printf("OK\n");*/
    char * encoded_data = encode(me_data, nb_of_me_data);
    int size = strlen(encoded_data) +1;
    
    for (i = 0; i < nb_ip; i++) {
    	int j = 0;
        for(j = 0; j<=PORT_OFFSET; j++){
        
        	if(connect_ret[i][j] != -1){
        		if(addr[i][j].sin_addr.s_addr != local.s_addr || htons(addr[i][j].sin_port) != port_now){
				    
				    /*printf("OK\n");*/
				    if(write(socket_fd[i][j],&size,sizeof(int)) == -1){
				    	perror("connectman");
    					return;
				   	}
				   	
				    /*printf("OK\n");*/
				    if(write(socket_fd[i][j],encoded_data,size) == -1){
				    	perror("connectman");
    					return;
				    };
				    
				   	close(socket_fd[i][j]);
        		}
        	
       		}
       	}
    
    }
    
	printf("%s\n",encoded_data);
	
	/*printf("OK\n");*/
	if(update_folder(to_syc,me_data,nb_of_me_data) == -1){
		perror("connectman");
    	return;
	}
	
	free_encoded_message(encoded_data);
	free_data(me_data,nb_of_me_data);
	
	for(i = 0; i<nb_ip;i++){
		free(tab_addr[i]);
		tab_addr[i] = NULL;
	}
}

void * connexion_manager(void *arg) {
    int newSock = (intptr_t) arg;
    if (newSock != EXIT_FAILURE) {
        char buff[10000];
        printf("SERVEUR : connection acceped\n");
        
        if(read(newSock, buff, sizeof (buff)) == -1){
        	perror("connexion_manager");
        }	
        
        if (strcmp(buff, firstmessage) == 0) {
            int nb = 0;
            to_syc = "./files_to_sync";
            Data * data = get_data_form_dir( to_syc, &nb);
            /*printf("OK\n");*/
            char * encodeed_message = encode(data, nb);
            /*printf("OK\n");*/
            int strlen_message = strlen(encodeed_message)+1;
            if(write(newSock, &strlen_message, sizeof (int)) == -1){
            	perror("connexion_manager");
            }
            
            if(write(newSock, encodeed_message, strlen_message) == -1){
            	perror("connexion_manager");
            }
            
            
            printf("Recuperation of files...\n");
            free_data(data,nb);
            int size;
           
           	if(read(newSock,&size,sizeof(int)) == -1){
           		perror("connexion_manager");
           	}
           
            char msg[size];
            
            if(read(newSock,msg,size) == -1){
            	perror("connexion_manager");
            }
            
            data = decode(msg,&nb);
            printf("%s\n",msg);
            
            if(update_folder(to_syc,data,nb) == -1){
            	perror("connexion_manager");
            }
            
            free_encoded_message(encodeed_message);
            free_data(data,nb);
            close(newSock);
        }
    }
    pthread_exit(NULL);
}

void * switchman(void *arg) {
    (void) arg;
    struct sockaddr_in addr;
    int port = PORT;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (getSockAddr(port, &addr) == -1) {
        perror("GET SOCK ADDR");
        return (void *) EXIT_FAILURE;
    }
    if (setAcceptSocket(&addr, socket_fd) == -1) {
        perror("SET ACCEPT SOCK");
        return (void *) EXIT_FAILURE;
    }
	
    printf("INITIALIZE SERVEUR IN PORT= %d\n", htons(addr.sin_port));
    
    while (1) {
        printf("WAIT NEW CONNEXION...\n");
        if (listen(socket_fd, 5) == -1) {
            printf("err\n");
        }

        int newSock = accept(socket_fd, NULL, NULL);
        if(newSock == -1){
        	perror("switchman");
        	return (void*) EXIT_FAILURE;
        }
        
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
    char ** ips = NULL;
    if(argc > 1){
     if((ips = (char **) malloc(sizeof (char *)*argc)) == NULL){
     	perror(argv[0]);
     	return EXIT_FAILURE;
     }
    }
    /*char * ips[argc+1];*/
    pthread_t thread1;
    
    if (pthread_create(&thread1, NULL, switchman, NULL) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    int i = 0;
    for (i = 1; i < argc; i++) {
        if((ips[i - 1] = (char *) malloc(sizeof (char)*strlen(argv[i]))) == NULL){
        	perror(argv[0]);
        	return EXIT_FAILURE;
        }
        strncpy(ips[i - 1], argv[i],strlen(argv[i]));
    }
    
    sleep(3);
    if(argc>1){
    	ips[argc] = NULL;
    	connectman(ips,argc-1);
    }
    free(ips);
    ips = NULL;
    while (1) {
        sleep(1000);
    }
    /*  if (inet_aton(ip_addr, &(addr.sin_addr.s_addr)) == -1) {
    //            perror(argv[0]);
    //            return EXIT_FAILURE;
    //        }
    //        if (connect(socket_fd, (struct sockaddr *) &addr, sizeof (struct sockaddr)) == -1) {
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
	int error;
    while((error = bind(socket_fd, (struct sockaddr*) serv, sizeof (struct sockaddr_in))) == -1 && htons(serv->sin_port) < PORT+PORT_OFFSET){
    	serv->sin_port = htons(htons(serv->sin_port) + 1);
    }
    
	if(error == -1){
		port_now = 0;
        perror("getSockAddr");
        return EXIT_FAILURE;
    }
	
    if (listen(socket_fd, PORT) == -1) {
    	port_now = 0;
        perror("getSockAddr");
        return EXIT_FAILURE;
    }
    
    port_now = htons(serv->sin_port);
    
    return EXIT_SUCCESS;
}

