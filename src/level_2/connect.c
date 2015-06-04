#include "connect.h"

int port_now = -1;
const char* firstmessage = "Hello, I want all your data please :)";

void write_encoded_message(int fd,char * message,int length){
	int nb_pack = length / PACK_SIZE;
	int last_pack = length % PACK_SIZE;
	int i;
	int nb_writes = 0;
	int written = 0;
	for( i = 0; i<nb_pack; i++){
		if((written = write(fd, message + (i*PACK_SIZE), PACK_SIZE)) == -1){
			perror("write_encoded_message");
			return;
		}
		nb_writes += written;
	}
	
	if((written = write(fd, message + (i*PACK_SIZE), last_pack)) == -1){
		perror("write_encoded_message");
		return;
	}
	nb_writes += written;
	
	if(nb_writes != length){
		perror("write_encoded_message");
	}
}

void read_encoded_message(int fd,char * message,int length){
	int nb_pack = length / PACK_SIZE;
	int last_pack = length % PACK_SIZE;
	int i;
	int nb_reads = 0;
	int readed = 0;
	for( i = 0; i<nb_pack; i++){
		if((readed = read(fd, message + (i*PACK_SIZE), PACK_SIZE)) == -1){
			perror("read_encoded_message");
			return;
		}
		nb_reads += readed;
	}
	
	if((readed = read(fd, message + (i*PACK_SIZE), last_pack)) == -1){
		perror("read_encoded_message");
		return;
	}
	nb_reads += readed;
	
	if(nb_reads != length){
		perror("read_encoded_message");
	}
}

void connectman(char **tab_addr,int length) {
    
   /* char **tab_addr = (char **) arg;*/
    int nb_ip = length;
    int  nb_of_me_data;
    
    char * to_syc = "./";
    
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
				    
				    /*char messge[strlen_message];*/
				    char * messge;
				    if((messge = malloc(sizeof(char)*strlen_message)) == NULL){
				        perror("connectman");
    					return;
				    }
				   	/*printf("OK\n");*/
				   	read_encoded_message(socket_fd[i][j],messge,strlen_message);
				    /*if(read(socket_fd[i][j], &messge, strlen_message) == -1){
				    	perror("connectman");
    					return;
				    }*/
				   
				    /*printf("%s\n\n\n",messge);*/
				    int length = -1;
				    Data * data = decode(messge, &length);
				    
				    /*print_data(me_data,nb_of_me_data);
				    print_data(data,length);*/
				    me_data = filter_and_replace(me_data, &nb_of_me_data, data, length);
				    /*print_data(me_data,nb_of_me_data);*/
				    free(messge);
				    free_data(data,length);
				} else {
				    /*printf("not connected :( \n");*/
				}
		    }
			
        }
    }
    char * encoded_data = encode(me_data, nb_of_me_data);
    /*printf("OK\n");*/
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
            		write_encoded_message(socket_fd[i][j],encoded_data,size);
				    /*if(write(socket_fd[i][j],encoded_data,size) == -1){
				    	perror("connectman");
    					return;
				    };*/
				    
				   	close(socket_fd[i][j]);
        		}
        	
       		}
       	}
    
    }
    
	/*printf("%s\n",encoded_data);*/
	
	/*printf("OK\n");*/
	if(update_folder(me_data,nb_of_me_data) == -1){
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
   			pthread_exit(NULL);
        }	
        
        if (strcmp(buff, firstmessage) == 0) {
            int nb = 0;
            char * to_syc = "./";
            Data * data = get_data_form_dir( to_syc, &nb);
            /*printf("OK\n");*/
            char * encodeed_message = encode(data, nb);
            /*printf("OK\n");*/
            int strlen_message = strlen(encodeed_message)+1;
            if(write(newSock, &strlen_message, sizeof (int)) == -1){
            	perror("connexion_manager");
   				pthread_exit(NULL);
            }
            
            write_encoded_message(newSock,encodeed_message,strlen_message);
            /*if(write(newSock, encodeed_message, strlen_message) == -1){
            	perror("connexion_manager");
   				pthread_exit(NULL);
            }*/
            
            /*printf("%s\n",encodeed_message);*/
            printf("Recuperation of files...\n");
            free_data(data,nb);
            int size = -1;
           	
           	if(read(newSock,&size,sizeof(int)) == -1){
           		perror("connexion_manager");
   				pthread_exit(NULL);
           	}
           	/*printf("%d\n",size);*/
            /*char msg[size];*/
            char * msg;
            if((msg = malloc(sizeof(char)*size)) == NULL){
           		perror("connexion_manager");
   				pthread_exit(NULL);
           	}
            
            read_encoded_message(newSock,msg,size);
            /*if(read(newSock,msg,size) == -1){
            	perror("connexion_manager");
   				pthread_exit(NULL);
            }*/
            /*printf("%s\n",msg);*/
            
            data = decode(msg,&nb);
            /*print_data(data,nb);*/
            /*printf("OK\n");*/
            if(update_folder(data,nb) == -1){
            	perror("connexion_manager");
   				pthread_exit(NULL);
            }
            
            free_encoded_message(encodeed_message);
            free(msg);
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
   		pthread_exit(NULL);
    }
    if (setAcceptSocket(&addr, socket_fd) == -1) {
        perror("SET ACCEPT SOCK");
   		pthread_exit(NULL);
    }
	
    printf("INITIALIZE SERVEUR IN PORT= %d\n", htons(addr.sin_port));
    
    while (1) {
        printf("WAIT NEW CONNEXION...\n");
        if (listen(socket_fd, 5) == -1) {
        	perror("switchman");
   			pthread_exit(NULL);
        }

        int newSock = accept(socket_fd, NULL, NULL);
        if(newSock == -1){
        	perror("switchman");
   			pthread_exit(NULL);
        }
        
        printf("Connect accepted by serveur\n");
        pthread_t new_manager;
		
        if (pthread_create(&new_manager, NULL, connexion_manager, (void*) (intptr_t) newSock) == -1) {
            perror("pthread_create");
   			pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
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
