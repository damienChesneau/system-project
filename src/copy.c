#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "copy.h"
#include <errno.h>
#include <string.h>

#define _BSD_SOURCE  1

#define _SVID_SOURCE 1
int get_timestamp_of_file(char * filename);

int get_mode(char * filename) {
    struct stat st;
    if(stat(filename, &st) == -1){
    	perror("get_mode");
    	return EXIT_FAILURE;
    }
    return st.st_mode;
}

int update_folder(char * dir,Data * data, int size){
	int i = 0;
	for(i = 0; i<size; i++){
		if(get_timestamp_of_file(data[i].path) < data[i].timestamp){
			char path[510];
			
			if(dir[strlen(dir)-1] == '/'){
				snprintf(path,510,"%s%s",dir,data[i].path);
			}else{
				snprintf(path,510,"%s/%s",dir,data[i].path);
			}
			
			int fd = creat(path,get_mode(dir));
			printf("%s\n",path);
			if(fd == -1){
				perror("update_folder");
				return EXIT_FAILURE;
			}
			
			if(write(fd,data[i].data,strlen(data[i].data)) == -1){
				perror("update_folder");
				return EXIT_FAILURE;
			}
			close(fd);
		} 
	}	
	
	return EXIT_SUCCESS;
}

int fd_open_out(int fd_in, struct dirent* file) {
    int fd_out;
    if (isDirectory(file, file->d_name)) {
        /*Create the directory */
    } else {

        if ((fd_out = creat(file->d_name, O_WRONLY)) == EXIT_FAILURE) {
            perror("copy_out");
            return EXIT_FAILURE;
        }

        if (copy(fd_out, fd_in) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int fd_open_in(int fd_out, struct dirent* file) {
    int fd_in;
    if (isDirectory(file, file->d_name)) {
        /*Create the directory */
    } else {

        if ((fd_in = open(file->d_name, O_RDONLY)) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }

        if (copy(fd_out, fd_in) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int copy(int fd_in, int fd_out) {
    int nb_reads;
    char buffer[10];


    while ((nb_reads = read(fd_in, buffer, 10)) == 10) {

        if (write(fd_out, buffer, 10) == EXIT_FAILURE) {
            perror("copy");
            return EXIT_FAILURE;
        }
    }

    if (nb_reads == -1) {
        perror("copy");
        return EXIT_FAILURE;
    } else {
        if (write(fd_out, buffer, nb_reads) == EXIT_FAILURE) {
            perror("copy");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int copy_all(int fd, Fd_open func) {
    struct dirent ** reader;
    int nb_file;
    int i;

    if ((nb_file = scandir("./", &reader, NULL, alphasort)) == EXIT_FAILURE) {
        perror("copy_all");
        return EXIT_FAILURE;
    }

    for (i = 0; i < nb_file; i++) {
        if (func(fd, reader[i]) == EXIT_FAILURE) {
            perror("copy_all");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int isDirectory(struct dirent* info, char* path) {
    struct stat fich;

    if (stat(path, &fich) == EXIT_FAILURE) {
        perror("isDirectory");
        return EXIT_FAILURE;
    }

    if (S_ISDIR(fich.st_mode))
        return 1;
    return 0;
}

int count_dir_files(const char * dir) {
    int nbr = 0;
    DIR * repertory = opendir(dir);
    if (!repertory) {
        perror("opendir() failed");
    } else {
        struct dirent * readedFile = NULL;
        errno = 0;
        int i = 0;
        while (readedFile = readdir(repertory)) {
            if (strcmp(readedFile->d_name, ".") != 0 && strcmp(readedFile->d_name, "..") != 0) {
                nbr++;
            }
        }
        if (errno) {
            perror("readdir() failed");
        }
    }
    return nbr;
}

char** get_all_files_from_dir(const char * dir, int nb_of_data, char ** files) {
    DIR * repertory = opendir(dir);
    if (!repertory) {
        perror("opendir() failed");
    } else {
        struct dirent * readedFile = NULL;
        errno = 0;
        int i = 0;
        while (readedFile = readdir(repertory)) {
            if (strcmp(readedFile->d_name, ".") != 0 && strcmp(readedFile->d_name, "..") != 0) {
                files[i] = readedFile->d_name;
                //printf("%s\n",files[i]);
                i++;
            }
        }
        if (errno) {
            perror("readdir() failed");
        }
    }
    //closedir(repertory);
    //    return (char**) files;
    return NULL;
}

void get_data_from_file(const char * filename, char ** buffer) {
    long length;
    FILE * f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        *buffer = malloc(length);
        if (*buffer) {
            fread(*buffer, 1, length, f);
        }
        fclose(f);
    }
}

Data* get_data_form_dir(const char * dir, int * nb_of_datas) {
    int count = count_dir_files(dir);
    int i = 0;
    *nb_of_datas = count;
    char new_dir[255];
    strcpy(new_dir, dir);
    if (new_dir[strlen(new_dir) - 1] != '/') {
        strcat(new_dir, "/");
    }
    
   	Data * datas = malloc(sizeof(Data)*count);
    char * files[count];
    char** t = get_all_files_from_dir(new_dir, count, files);
    for (i = 0; i < count; i++) {
        char * buffer;
        char tmp[255]; 
        strcpy(tmp, new_dir);
        strcat(tmp, files[i]);
        strcpy(datas[i].path, files[i]);
        get_data_from_file(tmp, &buffer);
        datas[i].data = malloc(strlen(buffer)+10);
        strcpy(datas[i].data, buffer); 
        datas[i].timestamp =  get_timestamp_of_file(tmp);  
    }   
    return datas;
}

int get_timestamp_of_file(char * filename) {
    struct stat st;
    if(stat(filename, &st) == -1){
    	/*perror("get_timestamp_of_file");
    	return EXIT_FAILURE;*/
    }
    return st.st_mtime;
}

void filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int *length){
	int max, min;
	Data * dmin,*dmax;
    if(*length > *nb_of_me_data){
    	dmax = data;
    	dmin = me_data;
    	max = *length;
    	min = *nb_of_me_data;
  	}else{
  		dmax = me_data;
  		dmin = data;
  		max = *nb_of_me_data;
  		min = *length;
  	}
  	int i,j;
  	int added = 0;
  	for(i = 0; i<max; i++){
  		int is_replaced = 0;
  		for(j = 0; j<min ; j++){
  			if(!is_replaced && strncmp(dmin[j].path,dmax[i].path,255) == 0){
  				is_replaced = 1;
  				if(dmin[j].timestamp > dmax[i].timestamp){
  					dmax[i].timestamp = dmin[j].timestamp;
  					dmax[i].data = realloc(dmax[i].data,strlen(dmin[j].data));
  					strcpy(dmax[i].data,dmin[j].data);
  				}
  				
  				if(dmin[j].timestamp < dmax[i].timestamp){
  					dmin[j].timestamp = dmax[i].timestamp;
  					dmin[j].data = realloc(dmin[j].data,strlen(dmax[i].data));
  					strcpy(dmin[j].data,dmax[i].data);
  				}
  			} 		
  		}
  		
  		if(!is_replaced){
  			added++;
  			dmin = realloc(dmin,(min+added)*sizeof(Data));
  			strncpy(dmin[min+added-1].path,dmax[j].path,255);
  			dmin[min+added-1].timestamp = dmax[j].timestamp;
  			dmin[min+added-1].data = malloc(strlen(dmax[i].data)+1);
  			strncpy(dmin[min+added-1].data,dmax[i].data,strlen(dmax[i].data));
  		}
  	}
  	*length = max;
  	*nb_of_me_data = max;
}		
