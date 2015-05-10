#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<poll.h>
#include<dirent.h>
#include<unistd.h>
#define PORT 3450 /* 3450 a 3650 */
#define OFFSET 200
#define BUF 256
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

 */
typedef int (*Fd_open)(int,struct dirent*);
int fd_open_out(int fd_in,struct dirent* file);
int fd_open_in(int fd_out,struct dirent* file);

int copy_all(int fd,Fd_open func);
int copy(int fd_out,int fd_in);
int isDirectory(struct dirent* info,char* path);
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
    	struct pollfd multi_fd[OFFSET];
    	socklen_t addrlen[OFFSET]; 
    	
    	for(i = 0; i<OFFSET; i++){
    		
    		addrlen[i] = sizeof(struct sockaddr_in);
    		client_fd[i] = accept(sock_fd[i],(struct sockaddr*)psock[i],&addrlen[i]);
    		
    		if(client_fd[i] == EXIT_FAILURE){
    			perror(argv[0]);
    			return EXIT_FAILURE;
    		}
    		
    		multi_fd[i].fd = client_fd[i];
    		multi_fd[i].events = POLLIN | POLLPRI;
    	}
    	
    	int state = poll(multi_fd,OFFSET,-1);
    	
    	if(state>0){
    		for(i = 0; i<OFFSET; i++){
    			
    			if(multi_fd[i].revents & POLLIN){
    			
    			}
    			
    			if(multi_fd[i].revents  & POLLPRI){
    			
    			}
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

int fd_open_out(int fd_in,struct dirent* file){
	int fd_out;
	if(isDirectory(file,file->d_name)){
		/*Create the directory */
	}else{
		
		if((fd_out = creat(file->d_name,O_WRONLY))== EXIT_FAILURE){
			perror("copy_out");
			return EXIT_FAILURE;
		}
		
		if(copy(fd_out,fd_in) == EXIT_FAILURE){
			perror("fd_open_in");
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}


int fd_open_in(int fd_out,struct dirent* file){
	int fd_in;
	if(isDirectory(file,file->d_name)){
		/*Create the directory */
	}else{
		
		if((fd_in = open(file->d_name,O_RDONLY)) == EXIT_FAILURE){
			perror("fd_open_in");
			return EXIT_FAILURE;
		}
		
		if(copy(fd_out,fd_in) == EXIT_FAILURE){
			perror("fd_open_in");
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}

int copy(int fd_in,int fd_out){
	int nb_reads;
	char buffer[10];
	
	
	while((nb_reads = read(fd_in,buffer,10))==10){
		
		if(write(fd_out,buffer,10)== EXIT_FAILURE){
			perror("copy");
			return EXIT_FAILURE;
		}
	}
		
	if(nb_reads ==-1){
		perror("copy");
		return EXIT_FAILURE;
	}else{
		if(write(fd_out,buffer,nb_reads)==EXIT_FAILURE){
				perror("copy");
				return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}

int copy_all(int fd,Fd_open func){
		struct dirent ** reader;
		int nb_file;
		int i;
		
		if((nb_file = scandir("./",&reader,NULL,alphasort)) == EXIT_FAILURE){
				perror("copy_all");
				return EXIT_FAILURE;
		}
		
		for(i = 0; i<nb_file; i++){
			if(func(fd,reader[i]) == EXIT_FAILURE){
				perror("copy_all");
				return EXIT_FAILURE;
			}
		}
		
		return EXIT_SUCCESS;
}

int isDirectory(struct dirent* info,char* path){
	struct stat fich;
		
	if(stat(path,&fich) == EXIT_FAILURE){
			perror("isDirectory");
			return EXIT_FAILURE;
	}
	
	if(S_ISDIR(fich.st_mode))
		return 1;
	return 0;
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

