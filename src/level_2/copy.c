#include "copy.h"

int get_timestamp_of_file(char * filename);

void print_data(Data * me_data,int size){
	int i;	
	for(i = 0; i<size; i++){
		printf("Print nb %d:\t%s\n",i,me_data[i].path);
		printf("%s\n\n",me_data[i].data);
	}
}

int is_directory(const char * filename){
    struct stat st;
    stat(filename, &st);
    return S_ISDIR(st.st_mode);
}

int get_mode(char * filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_mode;
}

int update_folder(Data * data, int size){
	int i = 0;
	for(i = 0; i<size; i++){
		if(!is_directory(data[i].path) && strncmp(data[i].data,"<<<<<DIRECTORY>>>>>",19) != 0){
			int fd = creat(data[i].path,data[i].mode);
			printf("FILE : %s\n",data[i].path);
			if(fd == -1){
				perror("update_folder");
				return -1;
			}
		
			if(write(fd,data[i].data,strlen(data[i].data)) == -1){
				perror("update_folder");
				return -1;
			}
			close(fd);
		}else{
			printf("DIR : %s\n",data[i].path);
			mkdir(data[i].path,data[i].mode);
		}
	
	}	
	
	return 1;
}

int count_dir_files(const char * dir) {
    int nbr = 0;
    DIR * repertory = opendir(dir);
    if (!repertory) {
    	printf("%s\n",dir);
        perror("opendir() failed ");
    } else {
        struct dirent * readedFile = NULL;
        int i = 0;
        while (readedFile = readdir(repertory)) {
	        if (strcmp(readedFile->d_name,"synchronizer") != 0 && readedFile->d_name[0] != '.') {
            	nbr++;
            	/*printf("%s\n",readedFile->d_name);*/
            	
				if(readedFile->d_type == DT_DIR){
					int len  = strlen(dir)+strlen(readedFile->d_name)+2;
					char temp[len];
					strncpy(temp,dir,len);
					strncat(temp,readedFile->d_name,len);
					temp[len-2] = '/';
					nbr += count_dir_files(temp);
				}
            }
        }
        
    }
    closedir(repertory);
    return nbr;
}

char** get_all_files_from_dir(const char * dir, int * i, char ** files) {
    DIR * repertory = opendir(dir);
    if (!repertory) {
        perror("opendir() failed");
    } else {
        struct dirent * readedFile = NULL;
        while (readedFile = readdir(repertory)) {
	        if (strcmp(readedFile->d_name,"synchronizer") != 0 && readedFile->d_name[0] != '.') {
				int len  = strlen(dir)+strlen(readedFile->d_name)+1;
	            if((files[*i] = (char *)malloc(sizeof(char)*len)) == NULL){
	            	perror("get_all_files_from_dir");
	            	return NULL;
	            }
				strncpy(files[*i],dir,len);
	            strncat(files[*i],readedFile->d_name,len);
	            /*printf("%s\t%d\n",files[*i],*i);*/
	            (*i) = (*i)+1;
				if(readedFile->d_type == DT_DIR){
					len ++;
					char temp[len];
					strncpy(temp,files[(*i)-1],len);
					temp[len-2] = '/';
					get_all_files_from_dir(temp,i,files);
				}
	        }
        }
    }
    closedir(repertory);
    return NULL;
}

void get_data_from_file(const char * filename, char ** buffer) {
    if(!is_directory(filename)){
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
    }else{
    	if((*buffer = malloc((sizeof(char)*20)+1)) == NULL){
		    	perror("get_data_from_files");
		    	return;
		}
    	strncpy(*buffer,"<<<<<DIRECTORY>>>>>",20);
    	(*buffer)[19] = '\0';
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
    char** t = get_all_files_from_dir(new_dir, &i, files);
    for (i = 0; i < count; i++) {
        char * buffer;
        /*char tmp[PATH_SIZE]; 
        strcpy(tmp, new_dir);
        strcat(tmp, files[i]);*/
        /*printf("%s\n",files[i]);*/
        strcpy(datas[i].path, files[i]);
        strcpy(datas[i].path,files[i]);
        get_data_from_file(files[i], &buffer);
        
        if((datas[i].data = malloc((sizeof(char)*strlen(buffer))+1)) == NULL){
        	perror("get_data_from_dir");
        	return NULL;
        }
        
        strcpy(datas[i].data, buffer); 
        datas[i].timestamp =  get_timestamp_of_file(files[i]);  
        datas[i].mode = get_mode(files[i]);
    	/*printf("OKA\t%s\t%d\n",files[i],i);*/
        free(buffer);
    	/*printf("OK%d\n",i);*/
        free(files[i]);
    	/*printf("OKB%d\n",i);*/
        files[i] = NULL;
        
    }   
    /*printf("PLOP\n");*/
    return datas;
}

int get_timestamp_of_file(char * filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_mtime;
}

Data * filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int length){
	
	int i = 0;
	int added = 0;
	for(i = 0; i<length; i++){
		int is_replaced = 0;
		int j = 0;
		for(j = 0; j<(*nb_of_me_data); j++){
			if(!is_replaced && strncmp(data[i].path,me_data[j].path,PATH_SIZE) == 0){
				is_replaced = 1;
			}
		}
		
		if(!is_replaced){
			added++;
		}
	}
	
	me_data = realloc(me_data,sizeof(Data)*(*nb_of_me_data+added));
	added = 0;
	
	for(i = 0; i<length; i++){
		int is_replaced = 0;
		int j = 0;
		for(j = 0; j<(*nb_of_me_data); j++){
			if(!is_replaced && strncmp(data[i].path,me_data[j].path,PATH_SIZE) == 0){
				is_replaced = 1;
				if(data[i].timestamp > me_data[j].timestamp){
					me_data[j].timestamp = data[i].timestamp;
					me_data[j].mode = data[i].mode;
					me_data[j].data = malloc((strlen(data[i].data)*sizeof(char))+1);
					strcpy(me_data[j].data,data[i].data);
				}
			}
		}
		if(!is_replaced){
			added++;
  			strncpy(me_data[*nb_of_me_data+added-1].path,data[i].path,PATH_SIZE);
  			me_data[*nb_of_me_data+added-1].timestamp = data[i].timestamp;
  			me_data[*nb_of_me_data+added-1].mode = data[i].mode;
  			if((me_data[*nb_of_me_data+added-1].data = malloc((sizeof(char)*strlen(data[i].data))+1)) == NULL){
  				perror("filter_and_replace");
  				return;
  			}
  			
  			strncpy(me_data[*nb_of_me_data+added-1].data,data[i].data,strlen(data[i].data));
		}
	}
	
	*nb_of_me_data = *nb_of_me_data+added;
	return me_data;
}		
