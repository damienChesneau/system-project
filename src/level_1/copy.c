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
		
		char path[PATH_SIZE*2];
		
		if(dir[strlen(dir)-1] == '/'){
			snprintf(path,PATH_SIZE*2,"%s%s",dir,data[i].path);
		}else{
			snprintf(path,PATH_SIZE*2,"%s/%s",dir,data[i].path);
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
	
	return EXIT_SUCCESS;
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
    closedir(repertory);
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
                if((files[i] = (char *)malloc((sizeof(char)*strlen(readedFile->d_name))+1)) == NULL){
                	perror("get_all_files_from_dir");
                	return NULL;
                }
                strncpy(files[i],readedFile->d_name,strlen(readedFile->d_name));
                i++;
            }
        }
        if (errno) {
            perror("readdir() failed");
        }
    }
    closedir(repertory);
    return NULL;
}

void get_data_from_file(const char * filename, char ** buffer) {
    long length;
    FILE * f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        if((*buffer = malloc((sizeof(char)*length)+1)) == NULL){
        	perror("get_data_from_files");
        	return;
        }
        if (*buffer) {
            fread(*buffer, 1, length, f);
        }
        (*buffer)[length] = '\0';
        fclose(f);
    }
}

Data* get_data_form_dir(const char * dir, int * nb_of_datas) {
    int count = count_dir_files(dir);
    int i = 0;
    *nb_of_datas = count;
    char new_dir[PATH_SIZE];
    strcpy(new_dir, dir);
    if (new_dir[strlen(new_dir) - 1] != '/') {
        strcat(new_dir, "/");
    }
    Data * datas;
   	if((datas = malloc(sizeof(Data)*count)) == NULL){
   		perror("get_data_from_dir");
   		return NULL;
   	}
    char * files[count];
    char** t = get_all_files_from_dir(new_dir, count, files);
    for (i = 0; i < count; i++) {
        char * buffer;
        char tmp[PATH_SIZE]; 
        strcpy(tmp, new_dir);
        strcat(tmp, files[i]);
        strcpy(datas[i].path, files[i]);
        get_data_from_file(tmp, &buffer);
        
        if((datas[i].data = malloc((sizeof(char)*strlen(buffer))+1)) == NULL){
        	perror("get_data_from_dir");
        	return NULL;
        }
        
        strcpy(datas[i].data, buffer); 
        datas[i].timestamp =  get_timestamp_of_file(tmp);  
        free(buffer);
        free(files[i]);
        files[i] = NULL;
    }   
    return datas;
}

int get_timestamp_of_file(char * filename) {
    struct stat st;
    stat(filename, &st);
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
  			if(!is_replaced && strncmp(dmin[j].path,dmax[i].path,PATH_SIZE) == 0){
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
  			strncpy(dmin[min+added-1].path,dmax[j].path,PATH_SIZE);
  			dmin[min+added-1].timestamp = dmax[j].timestamp;
  			
  			if((dmin[min+added-1].data = malloc((sizeof(char)*strlen(dmax[i].data))+1)) == NULL){
  				perror("filter_and_replace");
  				return;
  			}
  			
  			strncpy(dmin[min+added-1].data,dmax[i].data,strlen(dmax[i].data));
  		}
  	}
  	*length = max;
  	*nb_of_me_data = max;
}		
